#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include "include/usbrs232.h"
#include "include/message.h"
#include "include/dllist.h"
#include "include/timertx.h"
#include "include/configurers232.h"
#include "include/actionmsgavr.h"

#include "include/actionwsmsg.h"

//data structure required for reception
struct {
	uint8_t bufReceive[100];
	uint8_t chrBuffR;
	uint8_t countBuf;
	uint8_t codMsg;
	uint8_t codMsgSet;
	uint8_t lengthMsg;
	uint8_t msgError;
} serialRxData;

void resetStartSerialRxData() {
	serialRxData.countBuf = 0;
	serialRxData.codMsg = 0;
	serialRxData.codMsgSet = 0;
	serialRxData.lengthMsg = 0;
}

//data structure required for transmission
struct {
	uint8_t bufSend[100];
	uint8_t sizeBufSend;
	uint8_t msgToSend;
	uint8_t msgReadyBuf;
	uint8_t msgOk;
	uint8_t replyMsg;
	uint8_t replyCount;
	uint8_t nextMsg;
	uint8_t reqCs;
} serialTxData;

void resetStartSerialTxData() {
	serialTxData.sizeBufSend = 0;
	serialTxData.msgToSend = 0;
	serialTxData.msgReadyBuf = 0;
	serialTxData.msgOk = 0;
	serialTxData.replyMsg = 0;
	serialTxData.replyCount = MAX_REPLYCOUNT;
	serialTxData.reqCs = 0;
}

void setTXDataMsgOk(uint8_t val) {
	serialTxData.msgOk = val;
}

void setTXDataReplyMsg(uint8_t val) {
	serialTxData.replyMsg = val;
}

uint8_t isTXDataMsgReadyBuf() {
	return serialTxData.msgReadyBuf;
}

//variables used for fifo dilist.c
pthread_mutex_t mutexSend = PTHREAD_MUTEX_INITIALIZER;
struct dllist_t* fifoSend;

//variables used for thread
pthread_t threadRs232;
int  iretRs232;
void * ptrEmpty;

pthread_t threadSend;
int  iretSend;

int activeSerial = 1;

//----------------------------------------------------------------------------------------
void serialErrorToWs(uint8_t type);
void serialTxStart();
void sendMessage();
void sendMsgFifo();
void manageReplyMsg();
uint8_t ritLengthMsgRX(uint8_t cod);
void serialReceive();
void * mainRs232(void *ptr);
void * mainSend(void *ptr);
void stampaRicevi(uint8_t tipo);
void stampaInvia(int rit);

void startSerialThread() {
	InitTimer();
	iretRs232 = pthread_create(&threadRs232,NULL,mainRs232,ptrEmpty);
	if(iretRs232) {
		fprintf(stderr,"Error - pthread_create() return code: %d\n",iretRs232);
		exit(EXIT_FAILURE);
	}
	iretSend = pthread_create(&threadSend,NULL,mainSend,ptrEmpty);
	if(iretSend) {
		fprintf(stderr,"Error - pthread_create() return code: %d\n",iretSend);
		exit(EXIT_FAILURE);
	}
}

void stopSerialThread() {
	activeSerial = 0;
	void* val;
	pthread_join(iretRs232, &val);
	pthread_join(iretSend, &val);
	closeRs232();
	
}

void openRs232() {
	serialConfData.fd = open(serialConfData.device, O_RDWR | O_NOCTTY | O_SYNC);
	if(serialConfData.fd < 0) {
		wsSerialStates.states = 0;
		wsSerialStates.codE = errno;
		serialConfData.isOpen = 0;
		transmitMsgWsSerialStates();
		fprintf(stderr,"error %d opening %s: %s\n", errno, serialConfData.device, strerror (errno));
		return;
	}
	if(setInterfaceAttribs(serialConfData.fd) < 0) {
		wsSerialStates.states = 0;
		wsSerialStates.codE = errno;
		serialConfData.isOpen = 0;
		transmitMsgWsSerialStates();
		return;
	}
	initTestCommandStatus();
	setStartSerial(MAX_DELAYSTART);
	fprintf(stderr,"serial open\n");
	serialConfData.isOpen = 1;
	resetFIFOSend();
	resetStartSerialTxData();
	serialTxData.nextMsg = 0;
	//serialTxData.startSerial = 1;
	wsSerialStates.states = 1;
	wsSerialStates.codE = 0;
	transmitMsgWsSerialStates();
	//transmitRs232(0,1,AVRT_SYSTEMCMD,setMsgTsystemCmd(5), DIM_SYSTEMCMD); // 5 = suspend transmission
}


void closeRs232() {
	if(serialConfData.isOpen) {
		serialConfData.isOpen = 0;
		close(serialConfData.fd);
		fprintf(stderr,"serial close\n");
		wsSerialStates.states = 0;
		wsSerialStates.codE = 0;
		transmitMsgWsSerialStates();
	}
}


void transmitRs232(uint8_t instant,uint8_t wsCod,uint8_t msgCod,uint8_t* msg,size_t len) {
	fprintf(stderr,"fifo insert msgCod = %u \n",msgCod);
	if(instant) appendMsg(fifoSend,1,wsCod,msgCod,msg,len);
	else  pushMsg(fifoSend,0,wsCod,msgCod,msg,len);
}

void resetFIFOSend() {
	deleteAll(fifoSend);
}

// threads

void * mainRs232(void *ptr) {
	resetStartSerialRxData();
	fprintf(stderr,"open thread seriale RX\n");
	serialTxData.nextMsg = 0;
	while(activeSerial) {
		serialReceive();
	};
	fprintf(stderr,"close thread seriale RX\n");
}

void * mainSend(void *ptr) {
	fifoSend = initDllist(&mutexSend);	//initialize the fifo
	//testFifo();
	resetStartSerialTxData();
	//serialTxData.startSerial = 1;
	fprintf(stderr,"open thread trasmissione seriale aperto\n");
	while(activeSerial) {
		runDelayTimer();
		if(serialConfData.isOpen) {
			if(isTXRqtCs()) {
				resetTXRqtCs();
				// 0 = enable transmission command status
				serialTxData.reqCs = 1;
				//transmitRs232(1,1,AVRT_SYSTEMCMD,setMsgTsystemCmd(0), DIM_SYSTEMCMD); 
				setTXRqtCs(MIN_DELAYRQTCS);
			}
			if(isSendStatus()) {
				resetSendStatus();
				dispatchCommandStatus();
				setSendStatus(MINTIME_SENDSTATUS);
			}
			manageReplyMsg();
			if(isStartSerialDelay())	//tx start connection timer timertx.c
				serialTxStart();
			sendMessage();
			//sendMsgFifo();
			if(serialTxData.msgToSend) {	//message to send
				int rit = write(serialConfData.fd, serialTxData.bufSend, serialTxData.sizeBufSend);
				if(rit < 0) {
					fprintf(stderr,"\nrit = %u codMsg %u size %u\n\n",rit,serialTxData.bufSend[0],serialTxData.sizeBufSend);
				}
				stampaInvia(rit);
				//stampaRicevi(4);
				//fprintf(stderr,"fine Invio\n\n");
				serialTxData.msgToSend = 0;	// message sent
			}
		}
	}
	fprintf(stderr,"thread tasmissione seriale chiuso\n");
}

// section receive ----------------------------------------------------------------------------------------
//function of management of received data from the avr
void serialReceive() {
	if(serialConfData.isOpen) {
		
		//if(isRXTimeoutDelay()) resetStartSerialRxData();	//rx timeOut timer timertx.c
		int n = read(serialConfData.fd,&serialRxData.chrBuffR,1);
		stampaRicevi(1);
		
		if(n > 0) {
			serialRxData.bufReceive[serialRxData.countBuf] = serialRxData.chrBuffR;
			serialRxData.countBuf++;
			if(serialRxData.countBuf > 35) resetStartSerialRxData();
			if(!serialRxData.codMsgSet) {
				serialRxData.codMsg = serialRxData.bufReceive[0];
				serialRxData.codMsgSet = 1;
				serialRxData.lengthMsg = ritLengthMsgRX(serialRxData.codMsg);
				//stampaRicevi(2);
				if(!serialRxData.lengthMsg) {
					serialRxData.msgError = 1;
				}
			}
			if(serialRxData.lengthMsg && (serialRxData.lengthMsg == serialRxData.countBuf)) {
				//resetRXTimeout();
				switch(serialRxData.codMsg) {	// select the correct message management actionmsgavr.c
					case AVRR_SYSTEMERROR:
						actionSystemError(&serialRxData.bufReceive[1],DIM_SYSTEMERROR);
					break;
					case AVRR_SYSTEMOK:
						actionSystemOk(&serialRxData.bufReceive[1],DIM_SYSTEMOK);
						resetStartSerialTxData();
						serialTxData.msgOk = 1;
					break;
					case AVRR_COMMANDSTATUS:
						fprintf(stderr,"ricevuto command status");
						actionCommandStatus(&serialRxData.bufReceive[1],DIM_COMMANDSTATUS);
						resetStartSerialTxData();
						serialTxData.msgOk = 1;
					break;
					case AVRR_DEFAULTDATA:
						actionDefaultData(&serialRxData.bufReceive[1],DIM_DEFAULTDATA);
						resetStartSerialTxData();
						serialTxData.msgOk = 1;
					break;
				}
				if(serialRxData.codMsg != AVRR_COMMANDSTATUS) {
					stampaRicevi(3);
					stampaRicevi(4);
				}
				resetStartSerialRxData();
				if(serialRxData.codMsg != AVRR_COMMANDSTATUS) {
					fprintf(stderr,"fine ricezione\n\n");
				}
				serialTxData.nextMsg = 1;
			}
		} else {
			//serialRxData.bufReceive[n] = '\0';
			fprintf(stderr,"letti %d testo %s\n", n, serialRxData.bufReceive);
		}
	
	}
}

//returns the length of the message
uint8_t ritLengthMsgRX(uint8_t cod) {
	switch(cod) {
		case AVRR_SYSTEMERROR:
			fprintf(stderr,"\nritLengthMsgRX codMsg = %u DIM_SYSTEMERROR + 1 = %lu \n\n",cod,DIM_SYSTEMERROR + 1);
		return DIM_SYSTEMERROR + 1;;
		case AVRR_SYSTEMOK:
			fprintf(stderr,"\nritLengthMsgRX codMsg = %u DIM_SYSTEMOK + 1 = %lu \n\n",cod,DIM_SYSTEMOK + 1);
		return DIM_SYSTEMOK + 1;
		case AVRR_COMMANDSTATUS:
			//fprintf(stderr,"\nritLengthMsgRX codMsg = %u DIM_COMMANDSTATUS + 1 = %lu \n\n",cod,DIM_COMMANDSTATUS + 1);
		return DIM_COMMANDSTATUS + 1;
		case AVRR_DEFAULTDATA:
			fprintf(stderr,"\nritLengthMsgRX codMsg = %u DIM_DEFAULTDATA + 1 = %lu \n\n",cod,DIM_DEFAULTDATA + 1);
		return DIM_DEFAULTDATA + 1;
		default:
			fprintf(stderr,"\nritLengthMsgRX codMsg = %u \n\n",cod);	
		return 0;
	}
}


// section trasmit ----------------------------------------------------------------------------------------

// manage message repetition
void manageReplyMsg() {
	if((serialTxData.replyMsg) && (serialTxData.msgReadyBuf)) {
		serialTxData.msgToSend = 1;
		serialTxData.replyCount--;
	}
	if(!serialTxData.replyCount) {
		serialTxData.replyMsg = 0;
		resetStartSerialTxData();
	}
}

void printsendMsgFifo(int direct,int codMsg) {
	switch(codMsg) {
		case AVRT_SYSTEMCMD:
			printSystemCmdCmd(0);
		break;
		default:
			fprintf(stderr, "\n---------------------------------------------\n");
			fprintf(stderr, "\n fifo --> avr cod = %d diretto %d isMoveCmdNotPresent() %d \n", codMsg,direct,isMoveCmdNotPresent());
		break;
	}
}

void sendMessage() {
	sendMsgFifo();
	if((serialTxData.reqCs) &&(serialTxData.nextMsg)) {
		uint8_t* reqMsg = setMsgTsystemCmd(0);
		serialTxData.bufSend[0] = AVRT_SYSTEMCMD;
		for(uint8_t i = 0; i<DIM_SYSTEMCMD; i++) serialTxData.bufSend[i + 1] = reqMsg[i];
		serialTxData.sizeBufSend = DIM_SYSTEMCMD + 1;
		serialTxData.reqCs = 0;
		serialTxData.msgReadyBuf = 1;
		serialTxData.msgToSend = 1;
		serialTxData.msgOk = 0;
		serialTxData.nextMsg = 0;
	}
}

// handles the release of the next message
void sendMsgFifo() {
	if(serialTxData.nextMsg) {
		struct dllData* msgData = ritLastMsgNode(fifoSend);
		if(msgData != NULL) {
			//printsendMsgFifo(msgData->direct,msgData->msgCod);
			if((!msgData->direct) && (msgData->msgCod == AVRT_MOVE)) {
				fprintf(stderr, "\n\ncomando move v %d\n\n",isMoveCmdNotPresent());
				if(isMoveCmdNotPresent()) {
					serialTxData.sizeBufSend = msgData->length;
					deleteLastMsg(fifoSend);
					serialTxData.msgReadyBuf = 1;
					serialTxData.msgToSend = 1;
					serialTxData.msgOk = 0;
					serialTxData.nextMsg = 0;
				}
			} else {
				memcpy(serialTxData.bufSend,msgData->msg,msgData->length);
				serialTxData.sizeBufSend = msgData->length;
				deleteLastMsg(fifoSend);
				serialTxData.msgReadyBuf = 1;
				serialTxData.msgToSend = 1;
				serialTxData.msgOk = 0;
				serialTxData.nextMsg = 0;
			}
		}
	}
}

uint8_t* setMsgTsystemCmd(uint8_t cmd) {
	wsSystemCmd.cmd = cmd;
	return (uint8_t*) &wsSystemCmd;
}


uint8_t* setMsgTDirectMove() {
	wsDirectMove.speedTarget = 1000;
	wsDirectMove.forward = 1;
	return (uint8_t*) &wsDirectMove;
}


// generates the start message 
void serialTxStart() {
		resetStartSerial(); //tx start timer reset
		resetSendStatus();
		setSendStatus(MINTIME_SENDSTATUS); // start dispach command status
		resetTXRqtCs();
		setTXRqtCs(MIN_DELAYRQTCS);
		fprintf(stderr,"serialTxStart\n");
		//uint8_t* msgSystem = setMsgTsystemCmd(2); 
		//serialTxData.bufSend[0] = AVRT_SYSTEMCMD;
		//for(uint8_t i=0; i<DIM_SYSTEMCMD; i++) serialTxData.bufSend[i + 1] = msgSystem[i];
		//serialTxData.sizeBufSend = DIM_SYSTEMCMD + 1;
		
	//transmitRs232(uint8_t instant,uint8_t wsCod = fifo,uint8_t msgCod,uint8_t* msg,size_t len)
		transmitRs232(0,1,AVRT_SYSTEMCMD,setMsgTsystemCmd(5), DIM_SYSTEMCMD); // 5 = suspend transmission
		transmitRs232(0,1,AVRT_SYSTEMCMD,setMsgTsystemCmd(7), DIM_SYSTEMCMD); // 7 = set directCmd
		transmitRs232(0,1,AVRT_SYSTEMCMD,setMsgTsystemCmd(2), DIM_SYSTEMCMD); // 2 = request for default parameters
		//transmitRs232(0,1,AVRT_SYSTEMCMD,setMsgTsystemCmd(0), DIM_SYSTEMCMD); // 0 = enable transmission
		transmitRs232(0,1,AVRT_DIRECTMOVE,setMsgTDirectMove(), DIM_DIRECTMOVE); 
		//transmitRs232(0,1,AVRT_SYSTEMCMD,setMsgTsystemCmd(0), DIM_SYSTEMCMD); // 0 = enable transmission
		//transmitRs232(0,1,AVRT_SYSTEMCMD,setMsgTsystemCmd(0), DIM_SYSTEMCMD); // 0 = enable transmission

		fprintf(stderr,"AVRT_SYSTEMCMD %d DIMT_SYSTEMCMD %lu serialTxData.sizeBufSend  %d serialTxData.bufSend[0] %d\n ",AVRT_SYSTEMCMD,DIM_SYSTEMCMD,serialTxData.sizeBufSend,serialTxData.bufSend[0]);
		serialTxData.msgReadyBuf = 1;
		serialTxData.msgToSend = 1;
		serialTxData.nextMsg = 1;
}

//handles the timeout message
void serialErrorToWs(uint8_t type) {
	if(type == 1) {
		//resetTXTimeout();
		fprintf(stderr,"timeout error TX timeout %d\n",type);
	}
}

//---------------------------------------------------------------------------------------------------------

void stampaInvia(int rit) {
	if(serialTxData.bufSend[0] = AVRT_SYSTEMCMD)
		if(serialTxData.bufSend[1] = 0) return;
	fprintf(stderr,"inviato messaggio\n messaggio:");
		for( int t = 0; t < serialTxData.sizeBufSend; t++) 
			fprintf(stderr," pos %d valore %d",t,serialTxData.bufSend[t]);
		fprintf(stderr,"\ninviati n byte %d = dim buff invio %d\n",rit,serialTxData.sizeBufSend);
}

void stampaRicevi(uint8_t tipo) {
	if(tipo == 1) {
		//fprintf(stderr,"ricevuto messaggio\nletto byte val %d\n",serialRxData.chrBuffR);
	}
	if(tipo == 2) {
		fprintf(stderr,"messaggio deve essere lungh %d byte - codmsg %u ",serialRxData.lengthMsg,serialRxData.codMsg);
		fprintf(stderr,"serialRxData.countBuf %d\n",serialRxData.countBuf);
	}
	if(tipo == 3) {
		fprintf(stderr,"stato flag serialRxData countBuf %d codMsg %d codMsgSet %d lengthMsg %d\n",
			serialRxData.countBuf,serialRxData.codMsg,
			serialRxData.codMsgSet,serialRxData.lengthMsg);
	}
	if(tipo == 4) {
		/*fprintf(stderr,"stato flag serialTxData sizeBufSend %d msgToSend %d \n",
			serialTxData.sizeBufSend,
			serialTxData.msgToSend);
		fprintf(stderr,"msgReadyBuf %d msgOk %d replyMsg %d replyCount %d\n",
			serialTxData.msgReadyBuf,
			serialTxData.msgOk,
			serialTxData.replyMsg,
			serialTxData.replyCount);*/
	}
}


#include <stdio.h>
#include <websock/websock.h>
#include <pthread.h>
#include "include/wsserver.h"
#include "include/tools.h"
#include "include/receivewsmsg.h"
#include "include/usbrs232.h"
#include "include/variables.h"

int active = 1;
// if active == 0  threadRs232 terminate

int users = 0;
// number of users connected with the web socket

libwebsock_context *wctx = NULL;

int sendBinaryAllUsers(unsigned char *in_data, size_t len_data) {
	unsigned int count = 0;
	int flags =  WS_FRAGMENT_FIN | WS_OPCODE_BINARY;
	libwebsock_client_state *current;
	if (wctx->clients_HEAD == NULL) {
		return 0;
	}
	for (current = wctx->clients_HEAD; current != NULL; current = current->next) {
		libwebsock_send_fragment(current,  in_data, len_data, flags);
		count++;
	}

	return count;
}


int onMessage(libwebsock_client_state *state, libwebsock_message *msg) {
	switch (msg->opcode) {
		case WS_OPCODE_TEXT:
			libwebsock_send_text_with_length(state, msg->payload, msg->payload_len);
			fprintf(stderr, "opcode: %d\n", msg->opcode);
		break;
		case WS_OPCODE_BINARY:
			fprintf(stderr, "opcode: %d\n", msg->opcode);
			for(int i=0; i< msg->payload_len; i++)
				fprintf(stderr, "%u-%c,",msg->payload[i],msg->payload[i]);
			fprintf(stderr, "\n");
			receiveBinaryWsMsg(msg->payload,msg->payload_len);
		break;
		default:
			fprintf(stderr, "Unknown opcode: %d\n", msg->opcode);
		break;
	}
	return 0;
}

int onOpen(libwebsock_client_state *state) {
	users++;
	fprintf(stderr, "onopen: %d\n", state->sockfd);
	return 0;
}

int onClose(libwebsock_client_state *state) {
	users--;
  fprintf(stderr, "onclose: %d\n", state->sockfd);
  return 0;
}


int main(int argc, char *argv[]) {
	pthread_t threadRs232;
	int  iretRs232;
	void * ptrEmpty;

	testHostEndians();
	iretRs232 = pthread_create(&threadRs232,NULL,mainRs232,ptrEmpty);
	if (argc != 2) {
		fprintf(stderr, "Usage: %s <port to listen on>\n\nNote: You must be root to bind to port below 1024\n", argv[0]);
		exit(0);
	}

	if(iretRs232) {
		fprintf(stderr,"Error - pthread_create() return code: %d\n",iretRs232);
		exit(EXIT_FAILURE);
	}

	wctx = libwebsock_init();
	if (wctx == NULL ) {
		fprintf(stderr, "Error during libwebsock_init.\n");
		exit(EXIT_FAILURE);
	}
	libwebsock_bind(wctx, "0.0.0.0", argv[1]);
	fprintf(stderr, "libwebsock listening on port %s\n", argv[1]);
	wctx->onmessage = onMessage;
	wctx->onopen = onOpen;
	wctx->onclose = onClose;

	libwebsock_wait(wctx);
	active = 0;
	pthread_join(threadRs232,NULL);
	//perform any cleanup here.
	fprintf(stderr, "Exiting.\n");
	exit(EXIT_SUCCESS);
}

/*
*	 tools.js objects
* statesManager
*  pageManager
*	 tools.js functions
* eventClick(gobj,listId,listFcb)
*  eventChange(obj,listId,listFcb)
* getData(listId,listField,listInType)
* setData(listId,listField,listInType,data)
* enableElements(listId)
* disableElements(listId)
*drawSelectOptions(listId,group,ListNameOpz)
*	type input code
	empty or h = hidden, ro and rp = read only (plain), i = input,
*	 c  = checkbox, pc = prepend checkbox, s or sm = select(multiple) b = button
*/


/*
	object comSocketGroup
*		socketOpen group
* web socket message:	NO_SOCKETOPEN
* direction: 		T  (T trasmit R recive)
* name of field:	dict = {host: "",port: ""}
* name of button:	Connect
* label of field:	host port 
* html id field:	idsoc_host idsoc_post idsoc_connect
* type of input field:	i i b
* option group:			
* option for html selec input:	  

*		socketClose group
* web socket message:	NO_SOCKETCLOSE
* direction: 		T  (T trasmit R recive)
* name of field:	dict = {}
* name of button:	Disconnect
* label of field:	
* html id field:	idsoc_disconnect
* type of input field:	b
* option group:			
* option for html selec input:	

*		socketStates group
* web socket message:	NO_SOCKETSTATES
* direction: 		R  (T trasmit R recive)
* name of field:	dict = {states: "",codE: ""}
* name of button:	
* label of field:	states error code
* html id field:	idsoc_states idsoc_codE
* type of input field:	rp rp
* option group:			
* option for html selec input:	 

*/
var comSocketGroup = {
	data: {host: config['webSocket']['hostWs'],port: config['webSocket']['portWs']},
	socketOpenId : ["idsoc_host","idsoc_post"],
	socketOpenField : ["host","port"],
	socketOpenInType : ["i","i"],
	socketCloseId : [],
	socketCloseField : [],
	socketCloseInType : [],
	socketStatesId : ["idsoc_states","idsoc_codE"],
	socketStatesField : ["states","codE"],
	socketStatesInType : ["rp","rp"],
	start:function() {
		eventClick(this,["idsoc_connect"],["callBConnect"]);
		eventClick(this,["idsoc_disconnect"],["callBDisconnect"]);
		statesManager.records(['browserOk','connectWs'],this);
		setData(this.socketOpenId,this.socketOpenField,this.socketOpenInType,this.data);
	},
	callBConnect:function(data,e) {
		var param = getData(this.socketOpenId,this.socketOpenField,this.socketOpenInType);
		var host = param["host"].trim() + ':' + param["port"].trim();
		clientWS.connect(host);
	},
	callBDisconnect:function(data,e) {
		clientWS.disconnect();
	},
	callBackStates: function(modStates) {
		// function call from statesManager.setStates(states)
		var all = ["idsoc_host","idsoc_post","idsoc_connect","idsoc_disconnect"];
		if('browserOk' in modStates)
			if(!modStates['browserOk']) {
				disableElements(all);
				return;
			}
		if('connectWs' in modStates) {
			if(modStates['connectWs']) {
				enableElements(["idsoc_disconnect"]);
				disableElements(["idsoc_host","idsoc_post","idsoc_connect"]);
				setMessage("connectMessage","groupSocketE","Connected","");
			} else {
				disableElements(["idsoc_disconnect"]);
				enableElements(["idsoc_host","idsoc_post","idsoc_connect"]);
				setMessage("disconnectMessage","groupSocketE","disconnected",statesManager.getErrorWs());
			}
		}
	}
};


	// record object to pageManager for receive message through
	// function call from pageManager.dispatchMsg(nameMsg,data)
pageManager.recordReceiveGroup(comSocketGroup,[]);



/*
	object comSerialGroup
*		serialOpen group
* web socket message:	WSMSG_SERIALOPEN
* direction: 		T  (T trasmit R recive)
* name of field:	dict = {device: "",baud: "",parity: "",stop: "",data: ""}
* name of button:	Connect
* label of field:	device baud parity stop data 
* html id field:	idser_device idser_baud idser_parita idser_stop idser_dati idser_connect
* type of input field:	s s s s s b
* option group:			serial
* option for html selec input:	devices baud parita stop dati 

*		serialClose group
* web socket message:	WSMSG_SERIALCLOSE
* direction: 		T  (T trasmit R recive)
* name of field:	dict = {}
* name of button:	Disconnect
* label of field:	
* html id field:	idser_disconnect
* type of input field:	b
* option group:			
* option for html selec input:	

*		serialStates group
* web socket message:	WSMSG_SERIALSTATES
* direction: 		R  (T trasmit R recive)
* name of field:	dict = {states: "",codE: ""}
* name of button:	
* label of field:	states error code
* html id field:	idser_states idser_codE
* type of input field:	rp rp
* option group:			
* option for html selec input:	 

*/
var comSerialGroup = {
	codError: 0,
	connectSerial: false,
	serialOpenId : ["idser_device","idser_baud","idser_parity","idser_stop","idser_data"],
	serialOpenField : ["device","baud","parity","stop","data"],
	serialOpenInType : ["s","s","s","s","s"],
	serialOpenValType: ["s","i","i","i","i"],		//i=integer,f=float,s=string,b=bool
	start:function() {
		eventClick(this,["idser_connect"],["callBConnect"]);
		drawSelectOptions(["idser_device","idser_baud","idser_parity","idser_stop","idser_data"],"serial",["devices","baud","parity","stop","data"]);
		eventClick(this,["idser_disconnect"],["callBDisconnect"]);
		statesManager.records(["browserOk","connectWs","connectSerial"],this);	// ex. [connectWs,connectSerial]
		setData(["idser_states","idser_codE"],["states","codE"],["rp","rp"],{states: "disconnected", codE: ""});
	},
	trasmitData:function(msg) {
		if(msg == "serialOpen") 
			trasmitMsg.trasmitWs("serialOpen",
				getData(this.serialOpenId,this.serialOpenField,this.serialOpenInType,this.serialOpenValType));
		if(msg == "serialClose") trasmitMsg.trasmitWs("serialClose",{}); //No data
	},
	receiveData:function(msg,data) {
		// function call from pageManager.dispatchMsg(nameMsg,data)
		this.codError = data["codE"];
		if(data["states"]) this.connectSerial = true;
		else this.connectSerial = false;
		statesManager.setStates({'connectSerial': this.connectSerial});
	},
	callBConnect:function(data,e) {
	},
	callBDisconnect:function(data,e) {
	},
	manageStates:function(msg,data) {
		statesManager.setStates({});
	},
	callBackStates: function(modStates) {
		// function call from statesManager.setStates(states)
		var all = ["idser_device","idser_baud","idser_parity","idser_stop","idser_data","idser_connect",
				"idser_disconnect"];
		if('browserOk' in modStates)
			if(!modStates['browserOk']) {
				disableElements(all);
				return;
			}
		var connect = ["idser_device","idser_baud","idser_parita","idser_stop","idser_dati","idser_connect"]
		if('connectWs' in modStates) {
			if(modStates['connectWs']) enableElements(connect);
			else disableElements(all);
		}
		if('connectSerial' in modStates) {
			if(modStates['connectSerial']) {
				enableElements(["idser_disconnect"]);
				disableElements(connect);
				setMessage("connectMessage","groupSerialE","Connected","");
			} else {
				enableElements(connect);
				disableElements(["idser_connect"]);
				var txtErr = "";
				if(this.codError) txtErr = "Error Code " + this.codError;
				setMessage("disconnectMessage","groupSerialE","disconnected",txtErr);
			}
		}
	}
};

	// record object to pageManager for receive message through
	// function call from pageManager.dispatchMsg(nameMsg,data)
pageManager.recordReceiveGroup(comSerialGroup,["serialStates"]);


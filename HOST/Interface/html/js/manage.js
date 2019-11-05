


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
	socketOpenValType : ["s","i"],	// i=integer,f=float,s=string,b=bool
	socketCloseId : [],
	socketCloseField : [],
	socketCloseInType : [],
	socketStatesId : ["idsoc_states","idsoc_codE"],
	socketStatesField : ["states","codE"],
	socketStatesInType : ["rp","rp"],
	socketStatesValType : ["i","i"],	// i=integer,f=float,s=string,b=bool
	start:function() {
		eventClick(this,["idsoc_connect"],["callBConnect"]);
		eventClick(this,["idsoc_disconnect"],["callBDisconnect"]);
		statesManager.records(['browserOk','connectWs'],this);
		setData(this.socketOpenId,this.socketOpenField,this.socketOpenInType,this.data);
	},
	callBConnect:function(data,e) {
		var param = getData(this.socketOpenId,this.socketOpenField,this.socketOpenInType,this.socketOpenValType);
		var host = param["host"].trim() + ':' + param["port"];
		clientWS.connect(host);
	},
	callBDisconnect:function(data,e) {
		clientWS.disconnect();
		statesManager.setStates({connectWs: false, connectSerial: false});
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
		if(msg == "serialClose") trasmitMsg.trasmitWs("serialClose",{});
	},
	receiveData:function(msg,data) {
		this.codError = data["codE"];
		if(data["states"]) this.connectSerial = true;
		else this.connectSerial = false;
		statesManager.setStates({'connectSerial': this.connectSerial});
	},
	callBConnect:function(data,e) {
		this.trasmitData('serialOpen');
	},
	callBDisconnect:function(data,e) {
		this.trasmitData('serialClose');
	},
	manageStates:function(msg,data) {
		statesManager.setStates({});
	},
	callBackStates: function(modStates) {
		var all = ["idser_device","idser_baud","idser_parity","idser_stop","idser_data","idser_connect",
				"idser_disconnect"];
		if('browserOk' in modStates)
			if(!modStates['browserOk']) {
				disableElements(all);
				return;
			}
		var connect = ["idser_device","idser_baud","idser_parity","idser_stop","idser_data","idser_connect"]
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
				disableElements(["idser_disconnect"]);
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


var groupParamAvrGroup = {
	paramAvrId : ["idpa_enablePID","idpa_Kp","idpa_Ki","idpa_Kd","idpa_FBSampleTime","idpa_ImaxSum", "idpa_IminSum","idpa_maxPWM","idpa_minPWM","idpa_engineStartThreshold","idpa_directCmd"],
	paramAvrField : ["enablePID","Kp","Ki","Kd","FBSampleTime","ImaxSum","IminSum","maxPWM","minPWM","engineStartThreshold","directCmd"],
	paramAvrInType : ["ro","ro","ro","ro","ro","ro","ro","ro","ro","ro","ro"],
	paramAvrValType: ["i","f","f","f","i","i","i","i","i","i","i"],		//i=integer,f=float,s=string,b=bool
	msgCmd: 2,
	start:function() {
		eventClick(this,["idpa_paramAvr","idpa_recovers"],["callBRead_avr_status","callBWrite_EEPROM"]);
		statesManager.records(["browserOk","connectWs","connectSerial"],this);
	},
	trasmitData:function(msg) {
		if(msg == "systemCmd") trasmitMsg.trasmitWs("systemCmd",{"cmd" : this.msgCmd});
	},
	setDirectCmd(dcmd) {
		var data = getData(this.paramAvrId,this.paramAvrField,this.paramAvrInType,this.paramAvrValType);
		data["directCmd"] = dcmd;		
		setData(this.paramAvrId,this.paramAvrField,this.paramAvrInType,data);
	},
	receiveData:function(msg,data) {
		// function call from pageManager.dispatchMsg(nameMsg,data)
		setData(this.paramAvrId,this.paramAvrField,this.paramAvrInType,data);
	},
	callBRead_avr_status:function(data,e) {
		this.msgCmd = 2;
		this.trasmitData("systemCmd");
	},
	callBWrite_EEPROM:function(data,e) {
		this.msgCmd = 4;
		this.trasmitData("systemCmd");
	},
	manageStates:function(msg,data) {
		//statesManager.setStates({});	
	},
	callBackStates: function(modStates) {
		// function call from statesManager.setStates(states)
		var all = ["idpa_paramAvr","idpa_recovers"];
		if('browserOk' in modStates)
			if(!modStates['browserOk']) {
				disableElements(all);
				return;
			}
		if('connectWs' in modStates) {
			if(modStates['connectWs']) enableElements(all);
			else disableElements(all);
		}
		if('connectSerial' in modStates) {
			if(modStates['connectSerial']) enableElements(all);
			else disableElements(all);
		}
	}
};

	// record object to pageManager for receive message through
	// function call from pageManager.dispatchMsg(nameMsg,data)
pageManager.recordReceiveGroup(groupParamAvrGroup,["paramAvr"]);


var groupParam2avrGroup = {
	param2avrId : ["idp2a_engineStartThreshold","idp2a_enablePID","idp2a_Kp",
		"idp2a_Ki","idp2a_Kd","idp2a_FBSampleTime","idp2a_ImaxSum","idp2a_IminSum","idp2a_maxPWM",
		"idp2a_minPWM","idp2a_directCmd","idp2a_fifo"],
	param2avrField : ["engineStartThreshold","enablePID","Kp","Ki","Kd","FBSampleTime","ImaxSum",
		"IminSum","maxPWM","minPWM","directCmd","fifo"],
	param2avrInType : ["i","c","i","i","i","i","i","i","i","i","c","c"],
	param2avrValType: ["i","b","f","f","f","i","i","i","i","i","b","b"],		//i=integer,f=float,s=string,b=bool
	start:function() {
		eventClick(this,["idp2a_getStatus","idp2a_send","idp2a_fifo"],["callBGet_Avr_Status","callBSend","callBAdd_list"]);
		statesManager.records(["browserOk","connectWs","connectSerial"],this);
	},
	trasmitData:function(msg) {
		if(msg == "param2avr") {
			var data = getData(this.param2avrId,this.param2avrField,this.param2avrInType,this.param2avrValType);
			data["fifo"] = 0;
			trasmitMsg.trasmitWs("param2avr",data);
		}
	},
	receiveData:function(msg,data) {
		// function call from pageManager.dispatchMsg(nameMsg,data)
	},
	get_avr_status:function() {
		var data = getData(groupParamAvrGroup.paramAvrId,groupParamAvrGroup.paramAvrField, groupParamAvrGroup.paramAvrInType,groupParamAvrGroup.paramAvrValType);
		var dataO = {
			"engineStartThreshold" : data["engineStartThreshold"],
			"enablePID" : data["enablePID"], 
			"Kp" : data["Kp"],
			"Ki" : data["Ki"],
			"Kd" : data["Kd"], 
			"FBSampleTime" : data["FBSampleTime"], 
			"ImaxSum" : data["ImaxSum"],
			"IminSum" : data["IminSum"], 
			"maxPWM" : data["maxPWM"],
			"minPWM" : data["minPWM"], 
			"directCmd" : data["directCmd"],
			"fifo" : 0 
		};
		setData(this.param2avrId,this.param2avrField,this.param2avrInType,dataO);
	},
	callBGet_Avr_Status:function(data,e) {
		this.get_avr_status();
	},
	callBSend:function(data,e) {
		this.trasmitData("param2avr");
	},
	callBAdd_list:function(data,e) {
		var dataP = getData(this.param2avrId,this.param2avrField,this.param2avrInType,this.param2avrValType);
		listCommandGroup.addAvrItems(2,dataP);
	},
	callBenablePID:function(data,e) {
	},
	callBdirectCmd:function(data,e) {
	},
	callBfifo:function(data,e) {
	},
	manageStates:function(msg,data) {
		//statesManager.setStates({});
	},
	callBackStates: function(modStates) {
		// function call from statesManager.setStates(states)
		var all = ["idp2a_getStatus","idp2a_send","idp2a_fifo"];
		if('browserOk' in modStates)
			if(!modStates['browserOk']) {
				disableElements(all);
				return;
			}
		if('connectWs' in modStates) {
			if(modStates['connectWs']) enableElements(all);
			else disableElements(all);
		}
		if('connectSerial' in modStates) {
			if(modStates['connectSerial']) enableElements(all);
			else disableElements(all);
		}
	}
};

	// record object to pageManager for receive message through
	// function call from pageManager.dispatchMsg(nameMsg,data)
pageManager.recordReceiveGroup(groupParam2avrGroup,[]);


var groupMoveGroup = {
	moveId : ["idmv_speedTarget","idmv_nStepTarget","idmv_forward","idmv_enableStop","idmv_pause","idmv_millsPause","idmv_fifo","idmv_codCmd"],
	moveField : ["speedTarget","nStepTarget","forward","enableStop","pause","millsPause","fifo","codCmd"],
	moveInType : ["i","i","c","c","c","i","h","h"],
	moveValType: ["i","i","c","c","c","i","b","i"],		//i=integer,f=float,s=string,b=bool
	start:function() {
		eventClick(this,["idmv_send","idmv_fifo"],["callBSend","callBAdd_list"]);
		statesManager.records(["browserOk","connectWs","connectSerial"],this);
	},
	trasmitData:function(msg) {
		if(msg == "move") {
			var data = getData(this.moveId,this.moveField,this.moveInType,this.moveValType);
			data["fifo"] = 0;
			trasmitMsg.trasmitWs("move",data);
		}
	},
	receiveData:function(msg,data) {
		// function call from pageManager.dispatchMsg(nameMsg,data)
	},
	callBSend:function(data,e) {
		this.trasmitData("move");
	},
	callBAdd_list:function(data,e) {
		var dataP = getData(this.moveId,this.moveField,this.moveInType,this.moveValType);
		listCommandGroup.addMoveItems(1,dataP);
	},
	manageStates:function(msg,data) {
		//statesManager.setStates({});
	},
	callBackStates: function(modStates) {
		// function call from statesManager.setStates(states)
		var all = ["idmv_send","idmv_fifo"];
		if('browserOk' in modStates)
			if(!modStates['browserOk']) {
				disableElements(all);
				return;
			}
		if('connectWs' in modStates) {
			if(modStates['connectWs']) enableElements(all);
			else disableElements(all);
		}
		if('connectSerial' in modStates) {
			if(modStates['connectSerial']) enableElements(all);
			else disableElements(all);
		}
	}
};

	// record object to pageManager for receive message through
	// function call from pageManager.dispatchMsg(nameMsg,data)
pageManager.recordReceiveGroup(groupMoveGroup,[]);


var groupDirectMoveGroup = {
	directMoveId : ["iddm_speedTarget","iddm_forward"],
	directMoveField : ["speedTarget","forward"],
	directMoveInType : ["i","c"],
	directMoveValType : ["i","b"],		//i=integer,f=float,s=string,b=bool
	start:function() {
		eventClick(this,["iddm_send"],["callBSend"]);
		eventClick(this,["iddm_reset"],["callBReset"]);
		statesManager.records(["browserOk","connectWs","connectSerial"],this);
	},
	trasmitData:function(msg) {
		if(msg == "directMove") {
			trasmitMsg.trasmitWs("directMove",getData(
				this.directMoveId,this.directMoveField,this.directMoveInType,this.directMoveValType));
			//groupParamAvrGroup.setDirectCmd(1);
			//groupParam2avrGroup.setDirectCmd(1);
		}
		if(msg == "systemCmd") trasmitMsg.trasmitWs("systemCmd",{"cmd" : 6});
	},
	receiveData:function(msg,data) {
		// function call from pageManager.dispatchMsg(nameMsg,data)
	},
	callBSend:function(data,e) {
		this.trasmitData("directMove");
	},
	callBReset:function(data,e) {
		this.trasmitData("systemCmd");
	},
	manageStates:function(msg,data) {
		//statesManager.setStates({});
	},
	callBackStates: function(modStates) {
		// function call from statesManager.setStates(states)
		var all = ["iddm_send","iddm_reset"];
		if('browserOk' in modStates)
			if(!modStates['browserOk']) {
				disableElements(all);
				return;
			}
		if('connectWs' in modStates) {
			if(modStates['connectWs']) enableElements(all);
			else disableElements(all);
		}
		if('connectSerial' in modStates) {
			if(modStates['connectSerial']) enableElements(all);
			else disableElements(all);
		}
	}
};

	// record object to pageManager for receive message through
	// function call from pageManager.dispatchMsg(nameMsg,data)
pageManager.recordReceiveGroup(groupDirectMoveGroup,[]);



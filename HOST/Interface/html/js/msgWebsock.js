var  litteEndian = true;

var receiveMsg = {
	dv: null,
	getString:function(offset,numChar) {
		rit = '';
		for(var i=0; i<numChar; i++) {
			if(this.dv.getUint8(offset + i) != 0)
				rit += String.fromCharCode(this.dv.getUint8(offset + i));
		}
		return rit;
	},

//------------ Manage serial receive message 

	recMsgCod_5:function() {	// Receive message WSMSG_SERIALSTATES code 5
		data = {};
		data["states"] = this.dv.getUint8(1);
		data["codE"] = this.dv.getUint8(2);
		comSerialGroup.receiveData("serialStates",data);
	},


	recMsgCod_6:function() {	// Receive message WSMSG_COMMANDSTATUS code 6
		data = {};
		data["speed"] = this.dv.getUint16(1, litteEndian);
		data["position"] = this.dv.getUint16(3, litteEndian);
		data["nStepLeft"] = this.dv.getUint16(5, litteEndian);
		data["driftNstep"] = this.dv.getUint8(7);
		data["pwmHbridge"] = this.dv.getUint16(8, litteEndian);
		data["forward"] = this.dv.getUint8(10);
		data["pTerm"] = this.dv.getInt16(11, litteEndian);
		data["iTerm"] = this.dv.getInt16(13, litteEndian);
		data["dTerm"] = this.dv.getInt16(15, litteEndian);
		data["status"] = this.dv.getUint8(17);
		data["directCmd"] = this.dv.getUint16(18, litteEndian);
		data["codCmd"] = this.dv.getUint8(19);
		graphicsGroup.receiveData("commandStatus",data);
	},

	recMsgCod_7:function() {	// Receive message WSMSG_PARAMAVR code 7
		data = {};
		data["enablePID"] = this.dv.getUint8(1);
		data["Kp"] = this.dv.getFloat32(2, litteEndian);
		data["Ki"] = this.dv.getFloat32(6, litteEndian);
		data["Kd"] = this.dv.getFloat32(10, litteEndian);
		data["FBSampleTime"] = this.dv.getUint16(14, litteEndian);
		data["ImaxSum"] = this.dv.getInt16(16, litteEndian);
		data["IminSum"] = this.dv.getInt16(18, litteEndian);
		data["maxPWM"] = this.dv.getInt16(20, litteEndian);
		data["minPWM"] = this.dv.getInt16(22, litteEndian);
		data["engineStartThreshold"] = this.dv.getUint16(24, litteEndian);
		data["directCmd"] = this.dv.getUint8(26);
		data["fifo"] = this.dv.getUint8(27);
		groupParamAvrGroup.receiveData("paramAvr",data);
	},

// function call from clientWS.onMessage(evt)
	receive:function(rowData) {
		this.dv = new DataView(rowData,0);
		var typeMsg = this.dv.getUint8(0);
		this['recMsgCod_'+ typeMsg]();
	}
};


var trasmitMsg = {
	dv: null,
	setString:function(offset,numChar,str) {
		for(var i=0; i<numChar; i++) {
			if(i < str.length) {
				code = str.charCodeAt(i);
				if(code > 127) code = 32;
				this.dv.setUint8(offset + i,code);
			} else this.dv.setUint8(offset + i,0);
		}
	},
	setFloat32:function(offset,val) {
		var fval = parseFloat(valore);
		if(isNaN(fval)) fval = 0.0;
		this.dv.setFloat32(offset,fval, litteEndian);
	},
	setFloat64:function(offset,val) {
		var fval = parseFloat(valore);
		if(isNaN(fval)) fval = 0.0;
		this.dv.setFloat64(offset,fval, litteEndian);
	},

//------------ Manage serial trasmit message 
	msg_serialOpen:function(data) {	// Trasmit message WSMSG_SERIALOPEN
		var buffByte = new ArrayBuffer(28);
		this.dv = new DataView(buffByte);
		this.dv.setUint8(0,3);	// code message 3
		this.setString(1,20,data["device"]);
		this.dv.setUint32(21,data["baud"], litteEndian);
		this.dv.setUint8(25,data["parity"]);
		this.dv.setUint8(26,data["stop"]);
		this.dv.setUint8(27,data["data"]);
		return buffByte;
	},

//------------ Manage serial trasmit message End

	msg_serialClose:function(data) {	// Trasmit message WSMSG_SERIALCLOSE
		var buffByte = new ArrayBuffer(1);
		this.dv = new DataView(buffByte);
		this.dv.setUint8(0,4);	// code message 4
		return buffByte;
	},

	msg_move:function(data) {	// Trasmit message WSMSG_MOVE
		var buffByte = new ArrayBuffer(12);
		this.dv = new DataView(buffByte,0);
		this.dv.setUint8(0,8);	// code message 8
		this.dv.setUint16(1,data["speedTarget"], litteEndian);
		this.dv.setUint16(3,data["nStepTarget"], litteEndian);
		this.dv.setUint8(5,data["forward"]);
		this.dv.setUint8(6,data["enableStop"]);
		this.dv.setUint8(7,data["pause"]);
		this.dv.setUint16(8,data["millsPause"], litteEndian);
		this.dv.setInt8(10,data["fifo"]);
		this.dv.setInt8(11,data["codCmd"]);
		return buffByte;
	},

	msg_param2avr:function(data) {	// Trasmit message WSMSG_PARAM2AVR
		var buffByte = new ArrayBuffer(28);
		this.dv = new DataView(buffByte,0);
		this.dv.setUint8(0,9);	// code message 9
		this.dv.setUint8(1,data["enablePID"]);
		this.dv.setFloat32(2,data["Kp"],litteEndian);
		this.dv.setFloat32(6,data["Ki"], litteEndian);
		this.dv.setFloat32(10,data["Kd"], litteEndian);
		this.dv.setUint16(14,data["FBSampleTime"], litteEndian);
		this.dv.setInt16(16,data["ImaxSum"], litteEndian);
		this.dv.setInt16(18,data["IminSum"], litteEndian);
		this.dv.setInt16(20,data["maxPWM"], litteEndian);
		this.dv.setInt16(22,data["minPWM"], litteEndian);
		this.dv.setUint16(24,data["engineStartThreshold"], litteEndian);
		this.dv.setUint8(26,data["directCmd"]);
		this.dv.setUint8(27,0);
		return buffByte;
	},

	msg_directMove:function(data) {	// Trasmit message WSMSG_DIRECTMOVE
		var buffByte = new ArrayBuffer(4);
		this.dv = new DataView(buffByte,0);
		this.dv.setUint8(0,10);	// code message 10
		this.dv.setUint16(1,data["speedTarget"], litteEndian); 
		this.dv.setUint8(3,data["forward"]);
		return buffByte;
	},

	msg_systemCmd:function(data) {	// Trasmit message WSMSG_SYSTEMCMD
		var buffByte = new ArrayBuffer(2);
		this.dv = new DataView(buffByte);
		this.dv.setUint8(0,11);	// code message 11
		this.dv.setUint8(1,data["cmd"]);
		return buffByte;
	},

// function called by the objects that manage the message logic 
	trasmitWs:function(msg,data) {
		if(clientWS.socket != null) {
			clientWS.socket.send(trasmitMsg['msg_' + msg](data));
		}
	}
};


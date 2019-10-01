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

//------------ Manage serial: receive message 

	recMsgCod_5:function() {	// Receive message WSMSG_SERIALSTATES code 5
		data = {};
		data["states"] = this.dv.getUint8(1);
		data["codE"] = this.dv.getUint8(2);
		comSerialGroup.receiveData("serialStates",data);
	},

// function call from clientWS.onMessage(evt)
	receive:function(rowData) {
		this.dv = new DataView(rowData);
		var typeMsg = this.dv.getUint8(0);
		this['recMsgCod_'+ typeMsg]();
	}
};



var trasmitMsg = {
	dv: null,
	ritString:function(offset,val) {
		numStr = val.length;
		for(var i=0; i<numChar; i++) {
			code = valore.charCodeAt(i);
			if(code > 127) code = 32;
			if(i < numStr) this.dv.setUint8(offset + i,code);
			else this.dv.setUint8(offset + i,0);
		}
	},
	ritFloat32:function(offset,val) {
		var fval = parseFloat(valore);
		if(isNaN(fval)) fval = 0.0;
		this.dv.setFloat32(offset,fval);
	},
	ritFloat64:function(offset,val) {
		var fval = parseFloat(valore);
		if(isNaN(fval)) fval = 0.0;
		this.dv.setFloat64(offset,fval);
	},

//------------ Manage serial: trasmit message 
	msg_serialOpen:function(data) {	// Trasmit message WSMSG_SERIALOPEN
		var buffByte = new ArrayBuffer(28);
		this.dv = new DataView(buffer);
		this.dv.setUint8(0,3);	// code message 3
		data["device"] = this.getString(1,20);
		data["baud"] = this.dv.getUint32(21);
		data["parity"] = this.dv.getUint8(25);
		data["stop"] = this.dv.getUint8(26);
		data["data"] = this.dv.getUint8(27);
		return buffByte;
	},
	msg_serialClose:function(data) {	// Trasmit message WSMSG_SERIALCLOSE
		var buffByte = new ArrayBuffer(1);
		this.dv = new DataView(buffer);
		this.dv.setUint8(0,4);	// code message 4
		return buffByte;
	},

// function called by the objects that manage the message logic 
	trasmitWs:function(msg,data) {
		if(clientWS.socket != null) {
			clientWS.socket.send(trasmitMsg['msg_' + msg](data));
		}
	}
};


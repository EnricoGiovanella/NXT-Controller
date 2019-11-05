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


var listCommandGroup = {
		countCmd: 1,
		idList: "commandList",
		idBaseItem : "itemListCommand_",
		selClass: "itemCmdListSel",
		items : [],
		currentItem: 0,
		idButtons: ["cmdListUp","cmdListDown","cmdListDel","cmdListDelAll",
			"cmdListMoveUp","cmdListMoveDown","cmdListSend"],
		callbButtons: ["callBUp","callBDown","callBDel","callBDellAll","callBMoveUp","callBMoveDown","callBSend"],
	start:function() {
		eventClick(this,this.idButtons,this.callbButtons);
		statesManager.records(["browserOk","connectWs","connectSerial"],this);
		this.internalStates();
	},
	empty:function() {
		delete this.items;
		this.items = [];
		this.currentItem = 0;
	},
	addMoveItems:function(type,data) {
		data["codCmd"] = this.countCmd;
		data["fifo"] = 1;
		this.countCmd++;
		var txt = "move speed " + data["speedTarget"] + " nStep " + data["nStepTarget"];
		txt += " forward "  + data["forward"] + " enable stop " + data["enableStop"];
		txt += " pause " + data["pause"] + " milliseconds " + data["millsPause"];
		this.items.push({"codCmd": data["codCmd"],"type": type,"message": "move" ,"txt": txt,"data": data});
		this.currentItem = this.items.length-1;
		this.internalStates();
		this.drawItems();
	},
	addAvrItems:function(type,data) { 
		data["fifo"] = 1;
		var txt = "param enable PID " + data["enablePID"];
		txt += " Kp " + data["Kp"] + " Ki "  + data["Ki"] +  " Kd " + data["Kd"];
		txt += " ImaxSum " + data["ImaxSum"] + " IminSum " + data["IminSum"] + " maxPWM " +  data["maxPWM"];
		txt += " maxPWM " + data["minPWM"] + " eng. S. Thr. " + data["engineStartThreshold"];
		txt += " FB sample time " + data["FBSampleTime"] + " direct cmd " + data["directCmd"];
		this.items.push({"codCmd": 0,"type": type,"message": "param2avr","txt": txt,"data": data});
		this.currentItem = this.items.length-1;
		this.internalStates();
		this.drawItems();
	},
	selectItem:function(newCurrent) {
		$("#" + this.idBaseItem + this.currentItem).removeClass(this.selClass);
		this.currentItem = newCurrent;
		$("#" + this.idBaseItem + this.currentItem).addClass(this.selClass);
	},
	drawItems:function() {
		$("#" + this.idList).empty();
		for(var i in this.items)
			addItemListCommand(this.items["type"],this.idList,this.idBaseItem + i,this.items[i]["txt"]);
		this.selectItem(this.currentItem);
	},
	callBUp:function(data,e) {
		var pos = this.currentItem - 1;
		if(pos < 0) pos = 0;
		this.selectItem(pos);
		this.drawItems();
	},
	callBDown:function(data,e) {
		var pos = this.currentItem + 1;
		if(pos >= this.items.length) pos = this.items.length - 1;
		this.selectItem(pos);
		this.drawItems();
	},
	callBMoveUp:function(data,e) {
		if(this.currentItem <= 0) return;
		var pos = this.currentItem;
		var ele = this.items[pos];
		this.items[pos] = this.items[pos-1];
		this.items[pos-1] = ele;
		this.currentItem = pos - 1;
		this.drawItems();
	},
	callBMoveDown:function(data,e) {
		if(this.currentItem >= (this.items.length -1)) return;
		var pos = this.currentItem;
		var ele = this.items[pos];
		this.items[pos] = this.items[pos+1];
		this.items[pos+1] = ele;
		this.currentItem = pos + 1;
		this.drawItems();
	},
	callBDel:function(data,e) {
		this.items.splice(this.currentItem,1);
		if(this.currentItem >= this.items.length) this.currentItem = this.items.length;
		if(this.currentItem< 0) this.currentItem = 0;
		this.drawItems();
		this.internalStates();
	},
	callBDellAll:function(data,e) {
		this.empty();
		this.drawItems();
		this.internalStates();
	},
	callBSend:function(data,e) {
		for(var i in this.items) {
			trasmitMsg.trasmitWs(this.items[i]["message"],this.items[i]["data"]);
		}
		this.empty();
		this.drawItems();
	},
	internalStates:function() {
		if(this.items.length > 0) enableElements(this.idButtons);
		else disableElements(this.idButtons);
	}, 
	callBackStates: function(modStates) {
		this.internalStates();
	}
};

pageManager.recordReceiveGroup(listCommandGroup,[]);

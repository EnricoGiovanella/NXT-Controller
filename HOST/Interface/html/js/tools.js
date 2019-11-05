/*
* tools.js objects
* statesManager manages page states
* pageManager manages the logic of the page
* tools.js functions utility functions
* eventClick (gobj, listId, listFcb)
* eventChange (obj, listId, listFcb)
* getData (listId, listField, listInType) retrieves data from the html input
* setData (listId, listField, listInType, data) sets the html input
* enableElements (listId) enables html elements
* disableElements (listId) disables html elements
* drawSelectOptions (listId, group, ListNameOpz) adds the items to the select element

* empty or h = hidden, ro and rp = read only (plain), i = input,
* c = checkbox, pc = prepend checkbox, s or sm = select (multiple) b = button
*/

var statesManager = {
	states: {},
	errorWs: '',
	signalCb: {},
	start:function() {
		this.states = {browserOk: true, connectWs: false, connectSerial: false};
		var modified = {connectWs: false, connectSerial: false};
		// initial states
		for(var m in modified) {
			for(var i in this.signalCb[m]) {
				this.signalCb[m][i].callBackStates(modified);
			}
		}
	},
	setErrorWs:function(er) {
		this.errorWs = er;
	},
	getErrorWs:function() {
		if(this.errorWs == '') return '';
		return '  ' + this.errorWs;
	},
	addSignal:function(signal,obj) {
	},
	records:function(signallist,obj) {
		for(var i in signallist) {
			if(signallist[i] in this.signalCb) this.signalCb[signallist[i]].push(obj);
			else this.signalCb[signallist[i]] = [obj];
		}
	},
	setStates:function(states) {
		var modified = {};
		for(var ele in states) {
			if(this.states[ele] != states[ele]) {
				modified[ele] = states[ele];
				this.states[ele] = states[ele];
			}
		}
		for(var m in modified) {
			if(m in this.signalCb)
				for(var i in this.signalCb[m]) {
					this.signalCb[m][i].callBackStates(modified);
				}
		}
	},
	getStates:function(nameStates) {
		if(nameStates in this.states) return this.states[nameStates];
		return false;
	}
};

var pageManager = {
	listObjGroup: [],
	listObjRMsg: {},
	start:function() {
		for(var g in this.listObjGroup) this.listObjGroup[g].start();
	},
	recordReceiveGroup:function(objGroup,listNameRMsg) {
		this.listObjGroup.push(objGroup);
		for(var nMsg in listNameRMsg) this.listObjRMsg[listNameRMsg[nMsg]] = objGroup;
	},
	dispatchMsg:function(nameMsg,data) {	//da togliere
		this.listObjRMsg[nameMsg].receiveData(nameMsg,data);
	}
};


function eventClick(obj,listId,listFcb) {
	for( i in listId) {
		$("#" + listId[i]).click({obj: obj, id: listId[i], cbf: listFcb[i]}, function(e) {
			e.data.obj[e.data.cbf](e.data,e);
			return false;
		});
	}
}

function eventChange(obj,listId,listFcb) {
	for( i in listId) {
		$("#" + listId[i]).change({obj: obj, id: listId[i], cbf: listFcb[i]}, function(e) {
			e.data.obj[e.data.cbf](e.data,e);
			return false;
		});
	}
}

function setData(listId,listField,listInType,data) {
	for(i in listId) {
		if((listInType[i] == 'c') || (listInType[i] == 'pc'))	//checkbox
			if(data[listField[i]]) $('#' + listId[i]).prop('checked', true);
			else $('#' + listId[i]).prop('checked', false);
		else $('#' + listId[i]).val( '' + data[listField[i]]);
	}
}

function getData(listId,listField,listInType,listTypeVar) { 
	rit = {};
	for(i in listId) {
		var value = '';
		if((listInType[i] == 'c') || (listInType[i] == 'pc'))	//checkbox
			value = $('#' + listId[i]).is(":checked"); 
		else value = $('#' + listId[i]).val();
		switch(listTypeVar[i]) {
			case "i":
				rit[listField[i]] = parseInt(value);
				if(isNaN(rit[listField[i]])) rit[listField[i]] = 0;
			break;
			case "f":
				rit[listField[i]] = parseFloat(value);
				if(isNaN(rit[listField[i]])) rit[listField[i]] = 0.0;
			break;
			case "b":
				if(value) rit[listField[i]] = 1;
				else rit[listField[i]] = 0;
			break;
			default:
				rit[listField[i]] = value;
		}
	}
	return rit;
}

function enableElements(listId) {
	for( i in listId) {
		$("#" + listId[i]).prop( "disabled", false );
	}
}

function disableElements(listId) {
	for( i in listId) {
		$("#" + listId[i]).prop( "disabled", true );
	}
}

function addOptions(id,listOpz) {
	for( var t in listOpz) {
		var opz = $(new Option(listOpz[t]['e'],listOpz[t]['v'],listOpz[t]['s'],listOpz[t]['s']));
		if(listOpz[t]['d']) opz.attr("disabled","disabled");
		$("#" +id).append(opz);
	}
}

function drawSelectOptions(listId,group,ListNameOpz) {
// ListNameOpz = [{'v':valore, 'e': etichetta, 'd': diabiliitato 1true 0false, 's': select 1true 0false}]
	for(var i in listId) {
		addOptions(listId[i],config[group][ListNameOpz[i]]);
	}
}

function setMessage(cltxtcol,id,txt,txte) {
	$('#' + id).empty();
	$('#' + id).append('<span class="' + cltxtcol + '">' + txt + ' ' + txte + '</span>');
}

function addItemListCommand(type,idL,id,txt) {
	if(type = 1)
		htxt = '<li class="list-group-item" id="' + id + '"><strong>' + txt + '</strong></li>';
	if(type = 2)
		htxt = '<li class="list-group-item" id="' + id + '"><small><strong>' + txt + '</strong><small></li>';
	$('#' + idL).append($(htxt));
            
}

function addValueGraphBlock(id,value) {
	$('#' + id).html('<strong ml-3>' + ' ' + value + '</strong>');
}

function addTitleGraphBlock(id,txt) {
	$('#' + id).html(txt);
}

function createSingleGraph(idPar,ymax,plotSeries) {
	var plot = $.plot('#' + idPar,[plotSeries],{
			series: {shadowSize: 0},
			yaxis: {min: 0, max: ymax},
			xaxis: {show: false}
		});
	return plot;
}

function createMultipleGraph(idPar,ymax,dataset) {
	var plot = $.plot('#' + idPar,dataset,{
			series: {shadowSize: 0},
			yaxis: {min: 0, max: ymax},
			xaxis: {show: false}
		});
	return plot;
}



//-------------------------------------------
var comSerialGroup = {
	apriSerialeId : [],
	apriSerialeField : [],
	apriSerialeInType : [],
	start:function() {
		eventClick(this,['id','id'],['nomeCb','nomeCb']);
		eventChange(this,['id','id'],['nomeCb','nomeCb']);
		statesManager.records({connectWs: '', connectSerial: ''},this);
		drawSelectOptions(['id','id','id'],'serial',['devices','','']);
	},
	trasmitData:function(msg) {
		if (msg == 'apriSeriale')
		trasmitWs('apriSeriale',getData(this.apriSerialeId,this.apriSerialeField,this.apriSerialeInType));
	},
	receive:function(msg,data) {
		if (msg = 'statoSeriale') this.manageStates(msg,data);
	},
	manageStates:function(msg,data) {
		statesManager.setStates({connectWs: false, connectSerial: false});
	},
	callBackStates: function(modStates) {},
	nomeCb: function(data,e) {}
}



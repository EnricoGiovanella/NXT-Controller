/*
*	 tools.js objects
* statesManager gestisce gli stati della pagina
*  pageManager gestisce la logica della pagina
*	 tools.js functions funzioni di utilità
* eventClick(gobj,listId,listFcb)
*  eventChange(obj,listId,listFcb)
* getData(listId,listField,listInType) recupera i dati dall'input html
* setData(listId,listField,listInType,data) imposta l'input html
* enableElements(listId) abilita gli elementi html
* disableElements(listId) disabilita gli elementi html
* drawSelectOptions(listId,group,ListNameOpz) aggiunge gli item all'elemento select

*  empty or h = hidden, ro and rp = read only (plain), i = input,
*  c  = checkbox, pc = prepend checkbox, s or sm = select(multiple) b = button
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
	recordReceiveGroup(objGroup,listNameRMsg) {
		this.listObjGroup.push(objGroup);
		for(var nMsg in listNameRMsg) this.listObjRMsg[listNameRMsg[nMsg]] = objGroup;
	},
	dispatchMsg(nameMsg,data) {
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

function getData(listId,listField,listInType) {
	rit = {};
	for(i in listId) {
		if((listInType[i] == 'c') || (listInType[i] == 'pc'))	//checkbox
			if($('#' + listId[i]).is(":checked")) rit[listField[i]] = 1;
			else rit[listField[i]] = 0;
		else rit[listField[i]] = $('#' + listId[i]).val();  
	}
	return rit;
}

function setData(listId,listField,listInType,data) {
	for(i in listId) {
		if((listInType[i] == 'c') || (listInType[i] == 'pc'))	//checkbox
			if(data[listField[i]]) $('#' + listId[i]).prop('checked', true);
			else $('#' + listId[i]).prop('checked', false);
		else $('#' + listId[i]).val(data[listField[i]]);
	}
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


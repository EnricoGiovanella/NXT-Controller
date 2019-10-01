var clientWS = {
	socket: null,
	start:function() {
		if(!("WebSocket" in window)) statesManager.setStates({browserOk: false});
	},
	connect:function(host) {
		try {
			this.socket = new WebSocket(host);
			this.socket.binaryType = 'arraybuffer';

			this.socket.onopen = function(evt) { clientWS.onOpen(evt) };
			this.socket.onclose = function(evt) {  clientWS.onClose(evt) };
			this.socket.onmessage = function(evt) {  clientWS.onMessage(evt) };
			this.socket.onerror = function(evt) {  clientWS.onError(evt) };
			statesManager.setStates({connectWs: true});
			statesManager.setErrorWs('');
		} catch(e) {
			statesManager.setStates({connectWs: false});
			statesManager.setErrorWs(e);
		}
	},
	onOpen:function(evt) {
		statesManager.setStates({connectWs: true});
	},
	onClose:function(evt) {
		statesManager.setStates({connectWs: false});
		this.socket = null;
	},
	onMessage:function(evt) {
		if (evt.data instanceof ArrayBuffer) {
			receiveMsg.receive(evt.data);
		}
	},
	onError:function(evt) {
		statesManager.setErrorWs("Error occurred.");
	},
	disconnect:function() {
		this.socket.close()
		statesManager.setStates({connectWs: false});
	}
};


var config = {
	'webSocket': {	//websocket host
		hostWs: 'ws://127.0.0.1',
		portWs: '5000'
	},
	'serial': {	//rs232 v=return value, e=visible text, d=disabled, s=selected
		devices: [
			{'v': '/dev/ttyACM0', 'e': '/dev/ttyACM0', 'd': 0, 's': 1},
			{'v': '/dev/ttyS0', 'e': '/dev/ttyS0', 'd': 0, 's': 0},
			{'v': '/dev/ttyS1', 'e': '/dev/ttyS1', 'd': 0, 's': 0},
			{'v': '/dev/ttyS2', 'e': '/dev/ttyS2', 'd': 1, 's': 0}
		],
		baud: [
			{'v': '9600', 'e': '9600', 'd': 0, 's': 0},
			{'v': '19200', 'e': '19200', 'd': 0, 's': 0},
			{'v': '38400', 'e': '38400', 'd': 0, 's': 0},
			{'v': '57600', 'e': '57600', 'd': 0, 's': 0},
			{'v': '115200', 'e': '115200', 'd': 0, 's': 1}
		],
		parity: [
			{'v': '0', 'e': 'none', 'd': 0, 's': 1},
			{'v': '1', 'e': 'odd', 'd': 0, 's': 0},
			{'v': '2', 'e': 'even', 'd': 0, 's': 0}
		],
		stop: [
			{'v': '1', 'e': '1', 'd': 0, 's': 1},
			{'v': '2', 'e': '2', 'd': 0, 's': 0}
		],
		data: [
			{'v': '7', 'e': '7', 'd': 0, 's': 0},
			{'v': '8', 'e': '8', 'd': 0, 's': 1}
		]
	}
};

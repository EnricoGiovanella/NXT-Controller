var graphicsGroup = {
	graphs: {
		"graphRpm": {id: "graphRpm1", title: "Rpm motor speed graphic",
			idValue: ["graphDirValue","graphRpmValue","graphPosValue","graphHBValue"],
			objPlot: null, ymax: 1700, series: {rpm: []}, maxPoint: 600, seriesOV: [], maxPointOV: 3000,
			datasetEngine: [
				{ label: "speed (rpm x 10)", data: []},
				{ label: "position", data: []},
				{ label: "rpm Hbridge", data: []}
			]
		},
		"graphPID": {id: "graphPID2", title: "PID graphic",idValue: ["graphPtermValue","graphItermValue","graphDtermValue"],
			objPlot: null, ymax: 1700, series: {rpm: []}, maxPoint: 600, seriesOV: [], maxPointOV: 3000,
			datasetEngine: [
				{ label: "P term", data: []},
				{ label: "I term", data: []},
				{ label: "D term", data: []}
			]
		}
	},
	fakeXaxis: 0,
	objPlotOV: null,
	seriesPopupOV: [],
	flagPopup: false,
	start:function() {
		this.graphs["graphRpm"]["datasetEngine"][0]["data"] = [];	//speed
		this.graphs["graphRpm"]["datasetEngine"][1]["data"] = [];	//position
		this.graphs["graphRpm"]["datasetEngine"][2]["data"] = [];	//hBridge

		this.graphs["graphPID"]["datasetEngine"][0]["data"] = [];	//PTerm
		this.graphs["graphPID"]["datasetEngine"][1]["data"] = [];	//ITerm
		this.graphs["graphPID"]["datasetEngine"][2]["data"] = [];	//DTerm
		for(var i = 0; i< this.graphs["graphRpm"]["maxPoint"]; i++) {
			this.fakeXaxis = i;
			this.graphs["graphRpm"]["datasetEngine"][0]["data"].push([i,0]);	//speed
			this.graphs["graphRpm"]["datasetEngine"][1]["data"].push([i,0]);	//position
			this.graphs["graphRpm"]["datasetEngine"][2]["data"].push([i,0]);	//hBridge

			this.graphs["graphPID"]["datasetEngine"][0]["data"].push([i,0]);	//PTerm
			this.graphs["graphPID"]["datasetEngine"][1]["data"].push([i,0]);	//ITerm
			this.graphs["graphPID"]["datasetEngine"][2]["data"].push([i,0]);	//DTerm

			addValueGraphBlock(this.graphs["graphRpm"]["idValue"][0],"Stop");
			addValueGraphBlock(this.graphs["graphRpm"]["idValue"][1],0);
			addValueGraphBlock(this.graphs["graphRpm"]["idValue"][2],0);
			addValueGraphBlock(this.graphs["graphRpm"]["idValue"][3],0);

			addValueGraphBlock(this.graphs["graphPID"]["idValue"][0],0);
			addValueGraphBlock(this.graphs["graphPID"]["idValue"][1],0);
			addValueGraphBlock(this.graphs["graphPID"]["idValue"][2],0);
		}
		
		
		this.graphs["graphRpm"]["objPlot"] = createMultipleGraph(this.graphs["graphRpm"]["id"],
			this.graphs["graphRpm"]["ymax"],this.graphs["graphRpm"]["datasetEngine"]);

		this.graphs["graphPID"]["objPlot"] = createMultipleGraph(this.graphs["graphPID"]["id"],
			this.graphs["graphPID"]["ymax"],this.graphs["graphPID"]["datasetEngine"]);

		this.fakeXaxis = this.graphs["graphRpm"]["maxPoint"];
	},
	receiveData:function(msg,data) {
		if(this.fakeXaxis > 10000000) {
			this.graphs["graphRpm"]["series"]["rpm"] = [[0,0]];
			this.fakeXaxis = 1;
		}

			this.graphs["graphRpm"]["datasetEngine"][0]["data"].shift();	//speed
			this.graphs["graphRpm"]["datasetEngine"][1]["data"].shift();	//position
			this.graphs["graphRpm"]["datasetEngine"][2]["data"].shift();	//hBridge

			this.graphs["graphPID"]["datasetEngine"][0]["data"].shift();	//PTerm
			this.graphs["graphPID"]["datasetEngine"][1]["data"].shift();	//ITerm
			this.graphs["graphPID"]["datasetEngine"][2]["data"].shift();	//DTerm

			this.graphs["graphRpm"]["datasetEngine"][0]["data"].push([this.fakeXaxis,data["speed"]]);	//speed
			this.graphs["graphRpm"]["datasetEngine"][1]["data"].push([this.fakeXaxis,data["position"]]);	//position
			this.graphs["graphRpm"]["datasetEngine"][2]["data"].push([this.fakeXaxis,data["pwmHbridge"]]);	//hBridge

			this.graphs["graphPID"]["datasetEngine"][0]["data"].push([this.fakeXaxis,data["pTerm"]]);	//PTerm
			this.graphs["graphPID"]["datasetEngine"][1]["data"].push([this.fakeXaxis,data["iTerm"]]);	//ITerm
			this.graphs["graphPID"]["datasetEngine"][2]["data"].push([this.fakeXaxis,data["dTerm"]]);	//DTerm
		
		
		if(data["forward"] == 0)  addValueGraphBlock(this.graphs["graphRpm"]["idValue"][0],"Reverse");
		if(data["forward"] == 1) addValueGraphBlock(this.graphs["graphRpm"]["idValue"][0],"Forward");
		if(data["forward"] == 2) addValueGraphBlock(this.graphs["graphRpm"]["idValue"][0],"Stop");
		addValueGraphBlock(this.graphs["graphRpm"]["idValue"][1],data["speed"]);
		addValueGraphBlock(this.graphs["graphRpm"]["idValue"][2],data["position"]);
		addValueGraphBlock(this.graphs["graphRpm"]["idValue"][3],data["pwmHbridge"]);

		addValueGraphBlock(this.graphs["graphPID"]["idValue"][0],data["pTerm"]);
		addValueGraphBlock(this.graphs["graphPID"]["idValue"][1],data["iTerm"]);
		addValueGraphBlock(this.graphs["graphPID"]["idValue"][2],data["dTerm"]);

		
		this.graphs["graphRpm"]["objPlot"] = createMultipleGraph(this.graphs["graphRpm"]["id"],
			this.graphs["graphRpm"]["ymax"],this.graphs["graphRpm"]["datasetEngine"]);

		this.graphs["graphPID"]["objPlot"] = createMultipleGraph(this.graphs["graphPID"]["id"],
			this.graphs["graphPID"]["ymax"],this.graphs["graphPID"]["datasetEngine"]);

		this.fakeXaxis += 1;

	}
};

	// record object to pageManager for receive message through
	// function call from pageManager.dispatchMsg(nameMsg,data)
pageManager.recordReceiveGroup(graphicsGroup,["commandStatus"]);


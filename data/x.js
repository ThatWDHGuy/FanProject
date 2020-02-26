
// Function to create the html elements for a fan
function makeFanGraphElement(shelfName, fanData) {
    var fanDivElement = document.createElement("span");
    fanDivElement.id = shelfName + "." + fanData; // so you can update it later
    // create and add the stuff for a graph here
    var stuff = document.createElement("h2");
    stuff.appendChild(document.createTextNode("This is a fan graph for " + fanData));
    fanDivElement.appendChild(stuff);
    return fanDivElement;
}

// Function to create the html elements for temperature
function makeTempGraphElement(shelfName, sensorName) {
    var tempDivElement = document.createElement("span");
    tempDivElement.id = shelfName + "." + sensorName; // so you can update it later
    // create and add the stuff for a graph here
    var stuff = document.createElement("h2");
    /*stuff.onclick = function (e) {
    	//Fix this bit + install cors chrome
    	var x = document.getElementById(shelfName + "." + sensorName);
    	if (x.style.width === 500) {
    		x.style.width = 50;
    		x.style.height = 40;
    	} else {
    		x.style.width = 500;
    		x.style.height = 400;
    	}
    };*/
    stuff.appendChild(document.createTextNode("This is a temperature graph for " + shelfName + " sensor"));
    tempDivElement.appendChild(stuff);
    return tempDivElement;
}

var first = true;
var charts = {};

function addGraphElements(obj) {
    var graphDivElement = document.getElementById("graphs");
    graphDivElement.style.display = "flex";
    //var graphButtElement = document.getElementById("buttons");

    for (var dataSet in obj) {
	var dsetDivElement = document.createElement("div");
        dsetDivElement.id = dataSet; // will be "ambient", "shelf1" or "shelf2"
	dsetDivElement.style.display = "flex";
	dsetDivElement.style.flexDirection = "column";
	var headingElement = document.createElement("H2");
	headingElement.innerHTML = dataSet;
	headingElement.style.margin = "0px";
        dsetDivElement.appendChild(headingElement);
	var newGraphElement = makeTempGraphElement(dataSet, "sensor");
        dsetDivElement.appendChild(newGraphElement);
	charts[dataSet+".sensor"] = new google.visualization.LineChart(newGraphElement);

        // now add fan graphs - if there are any
        for (var fanGraph in obj[dataSet]) {
	    if (fanGraph != "sensor") {
		newGraphElement = makeFanGraphElement(dataSet, "fan")
		dsetDivElement.appendChild(newGraphElement);
		charts[dataSet+".fan"] = new google.visualization.LineChart(newGraphElement);
		break;
	    }
	}
        graphDivElement.appendChild(dsetDivElement);
    }
}

function updateGraphs(obj) {
    if (first) {
	addGraphElements(obj);
	first = false;
    }
    var newObj = {};
    for (var dataSet in obj) {
	var data = obj[dataSet];
	var d;
	for (var item in data) {
	    if (item == "sensor") {
		d = data[item];
		d.unshift(["Time", "Temp"]);
	    } else {
		var n = data[item];
		n.unshift(["Time", item]);
		for (var i = 0; i < n.length; i++) {
		    d[i].push(n[i][1]);	 
		}
	    }
	}
	newObj[dataSet] = d;
    }
    for (var dataSet in newObj) {
	var data = google.visualization.arrayToDataTable(newObj[dataSet]);
	var tempView = new google.visualization.DataView(data);
	var toHide = [];
	for (var i = 2;i < tempView.getNumberOfColumns();i++) {
	    toHide.push(i);
	}
	tempView.hideColumns(toHide);
	//var e = document.getElementById(dataSet + ".sensor");
	//var tempChart = new google.visualization.LineChart(e);
	charts[dataSet + ".sensor"].draw(tempView,
					 {	title:"Temperature",
						//curveType: "function",
						width: 500,
						height: 400,
						vAxis: {
						    maxValue: 10
						}
					 } );
	//var fanChart = null;
	//var c;
	var e = document.getElementById(dataSet + ".fan");
	if (e != null) {
	    //fanChart = new google.visualization.LineChart(e);
	    var fanView = new google.visualization.DataView(data);
	    fanView.hideColumns([1]);
	    charts[dataSet + ".fan"].draw(fanView,
					  {  title:"Fan Speed",
					     //curveType: "function",
					     width: 500,
					     height: 400,
					     vAxis: {
						 maxValue: 10
					     },
					  } );
	}
    }
}


	var confxhttp = null;

function statsResponseReceived() {
	//console.log('response '+this.readyState +' '+this.status);
	if (this.readyState == 4 && this.status == 200) {
		var graphData = JSON.parse(this.responseText);
		updateGraphs(graphData);
		confxhttp = null;
	}
}

function fireStatsRequest() {
	if (confxhttp)
		confxhttp.abort();
	confxhttp = new XMLHttpRequest();
	confxhttp.onreadystatechange = statsResponseReceived;
	confxhttp.open("GET", "/getData", true);
	confxhttp.send();
	console.log('fired');
}

var loadedComponents = 0;

function loaded() {
    console.log("going");
    loadedComponents += 1;
    if (loadedComponents == 2) {
	fireStatsRequest();
	setInterval(fireStatsRequest, 5000);
    }
}
google.charts.setOnLoadCallback(loaded);


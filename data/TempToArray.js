//java -jar /Users/Hollywood/Desktop/TestHttpServer.jar /Users/Hollywood/Dropbox/School\ 2018/Digi/data 50 1000


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

function drawVisualization() {
    var confxhttp = new XMLHttpRequest();
    confxhttp.onreadystatechange = function() {
        if (this.readyState == 4 && this.status == 200) {
            var raw = this.responseText;
            console.log(raw);
            var obj = JSON.parse(this.responseText);
            console.log(obj);
            if (first) {
                var graphDivElement = document.getElementById("graphs");
                //var graphButtElement = document.getElementById("buttons");

                for (var dataSet in obj) {
                    var dsetDivElement = document.createElement("div");
                    dsetDivElement.id = dataSet; // will be "ambient", "shelf1" or "shelf2"
                    dsetDivElement.appendChild(makeTempGraphElement(dataSet, "sensor"));
		    dsetDivElement.display = "flex";

                    // now add fan graphs - if there are any
                    for (var fanGraph in obj[dataSet]) {
                        if (fanGraph != "sensor") {
                            dsetDivElement.appendChild(makeFanGraphElement(dataSet, fanGraph));
                        }
                    }
                    graphDivElement.appendChild(dsetDivElement);
                }
            }
            first = false;
        }
        for (var dataSet in obj) {
            for (var fanGraph in obj[dataSet]) {
                if (fanGraph != "sensor") {
                    obj[dataSet][fanGraph].unshift(["Time", "Speed"]);
                } else {
                    obj[dataSet][fanGraph].unshift(["Time", "Temp"]);
                }
                var data = google.visualization.arrayToDataTable(obj[dataSet][fanGraph]);
                new google.visualization.LineChart(document.getElementById(dataSet + "." + fanGraph)).draw(
                    data, {
						title:dataSet+": "+fanGraph,
                        curveType: "function",
                        width: 500,
                        height: 400,
                        vAxis: {
                            maxValue: 10
                        }
                    });
            }
        }
    };
	
    confxhttp.open("GET", "getData", true);
    confxhttp.send();
}

function statsResponseReceived() {
	if (this.readyState == 4 && this.status == 200) {
		var obj = JSON.parse(this.responseText);
		updateGraphs(objs);
		confxhttp = null;
	}
}

function fireStatsRequest() {
	if (confxhttp)
		confxhttp.abort();
	confxhttp = new XMLHttpRequest();
	confxhttp.onreadystatechange = statsResponseReceived;
	confxhttp.open("GET", "getData", true);
	confxhttp.send();
}

google.charts.setOnLoadCallback(drawVisualization);
var refreshGraph = setInterval(drawVisualization, 5000);

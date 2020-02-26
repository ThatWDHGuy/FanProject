function GetWiFiConf(){
	var confxhttp = new XMLHttpRequest();
	confxhttp.onreadystatechange = function() {
	    if (this.readyState == 4 && this.status == 200) {
	       var obj = JSON.parse(this.responseText);
	       document.getElementById("ssid").value = obj.ssid;
	       document.getElementById("APssid").value = obj.APssid;
	       document.getElementById("password").value = obj.password;
	       document.getElementById("APpassword").value = obj.APpassword;
	       document.getElementById("historyLength").value = obj.historyLength;
	       document.getElementById("pIn").value = obj.pIn;
	       document.getElementById("iIn").value = obj.iIn;
	       document.getElementById("dIn").value = obj.dIn;
	    }
	};
	confxhttp.open("GET", "wifiConfig.dat", true);
	confxhttp.send();
}

window.onload = GetWiFiConf();
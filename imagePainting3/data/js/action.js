//--------------------------------------------------
function setAction(jsonString) {
	ACTION = JSON.parse(jsonString);
	// set ACTION values
	if (document.getElementById("ckPlaylist") != null) document.getElementById("ckPlaylist").checked = ACTION.isplaylist;
	if (document.getElementById("ckTrigger") != null) document.getElementById("ckTrigger").checked = ACTION.istrigger;
}

//--------------------------------------------------
function getAction() {
	// get ACTION values
	if (document.getElementById("ckPlaylist") != null) ACTION.isplaylist = document.getElementById("ckPlaylist").checked;
	if (document.getElementById("ckTrigger") != null) ACTION.istrigger = document.getElementById("ckTrigger").checked;
	// convert json to string
	return JSON.stringify(ACTION);
}

//--------------------------------------------------
function requestActionRead() {
	var xhr = new XMLHttpRequest();
	xhr.onload = function() {
		if (this.status == 200) {
			setAction(this.responseText);
		}
	};
	
	xhr.onerror = function() {
		updateStatus("ACTION READ ERROR : CONNECTION LOST", "red");
	};
	
	xhr.overrideMimeType("application/json");
	xhr.open("GET", address + "/actionRead", true);
	xhr.send(null);
}

//--------------------------------------------------
function requestActionWrite() {
	var xhr = new XMLHttpRequest();
	xhr.onload = function() {
		if (this.status == 200) {
			updateStatus(this.responseText, "green");
		} else {
			updateStatus(this.responseText, "red");
		}
		requestActionRead();
	};
	
	xhr.onerror = function() {
		updateStatus("ACTION WRITE ERROR : CONNECTION LOST", "red");
	};
	
	xhr.open("POST", address + "/actionWrite", true);
	xhr.setRequestHeader('Content-type', 'application/json');
	xhr.send(getAction());
}

//--------------------------------------------------
function requestAction(action) {
	var xhr = new XMLHttpRequest();
	xhr.onload = function() {
		if (this.status == 200) {
			updateStatus(this.responseText, "green");
		} else {
			updateStatus(this.responseText, "red");
		}
	};
	
	xhr.onerror = function() {
		updateStatus("ACTION ERROR : CONNECTION LOST", "red");
	};
	
	xhr.open("GET", address + action, true);
	xhr.send(null);
}

//--------------------------------------------------
function setParameter(jsonString) {
	PARAMETER = JSON.parse(jsonString);
	// set PARAMETER values
	if (document.getElementById("selectParameterBitmap") != null) document.getElementById("selectParameterBitmap").value = PARAMETER.bmp;
	if (document.getElementById("sliderParameterIndexStart") != null) {
		document.getElementById("sliderParameterIndexStart").setAttribute("min", 0);
		document.getElementById("sliderParameterIndexStart").setAttribute("max", PARAMETER.imx);
		document.getElementById("sliderParameterIndexStart").value = PARAMETER.ist;
	}
	if (document.getElementById("textParameterIndexStart") != null) document.getElementById("textParameterIndexStart").innerHTML = PARAMETER.ist + "px";
	if (document.getElementById("sliderParameterIndexStop") != null) {
		document.getElementById("sliderParameterIndexStop").setAttribute("min", 0);
		document.getElementById("sliderParameterIndexStop").setAttribute("max", PARAMETER.imx);
		document.getElementById("sliderParameterIndexStop").value = PARAMETER.isp;
	}
	if (document.getElementById("textParameterIndexStop") != null) document.getElementById("textParameterIndexStop").innerHTML = PARAMETER.isp + "px";
	if (document.getElementById("canvasParameterBitmap") != null) imgParameterBitmap.src = address + "/" + PARAMETER.bmp;
	// set PARAMETER values
	if (document.getElementById("sliderDelay") != null) document.getElementById("sliderDelay").value = PARAMETER.dly;
	if (document.getElementById("textDelay") != null) document.getElementById("textDelay").innerHTML = PARAMETER.dly + "ms";
	if (document.getElementById("sliderBrightness") != null) document.getElementById("sliderBrightness").value = PARAMETER.bts;
	if (document.getElementById("textBrightness") != null) document.getElementById("textBrightness").innerHTML = PARAMETER.bts + "%";
	if (document.getElementById("sliderWait") != null) document.getElementById("sliderWait").value = PARAMETER.wt;
	if (document.getElementById("textWait") != null) document.getElementById("textWait").innerHTML = PARAMETER.wt + "px";
	if (document.getElementById("ckWait") != null) document.getElementById("ckWait").checked = PARAMETER.iwt;
	if (document.getElementById("sliderRepeat") != null) document.getElementById("sliderRepeat").value = PARAMETER.rpt;
	if (document.getElementById("textRepeat") != null) document.getElementById("textRepeat").innerHTML = PARAMETER.rpt + "x";
	if (document.getElementById("ckInvert") != null) document.getElementById("ckInvert").checked = PARAMETER.iivt;
	if (document.getElementById("ckRepeat") != null) document.getElementById("ckRepeat").checked = PARAMETER.irpt;
	if (document.getElementById("ckBounce") != null) document.getElementById("ckBounce").checked = PARAMETER.ibnc;
	if (document.getElementById("ckRepeat") != null && document.getElementById("ckBounce") != null) updateCheckbox(document.getElementById("ckRepeat"), document.getElementById("ckBounce"));
	if (document.getElementById("ckBounce") != null && document.getElementById("ckRepeat") != null) updateCheckbox(document.getElementById("ckBounce"), document.getElementById("ckRepeat"));
	if (document.getElementById("sliderVcut") != null) document.getElementById("sliderVcut").value = PARAMETER.vc;
	if (document.getElementById("textVcut") != null) document.getElementById("textVcut").innerHTML = PARAMETER.vc + "px";
	if (document.getElementById("ckVcutOff") != null) document.getElementById("ckVcutOff").checked = PARAMETER.ivco;
	if (document.getElementById("ckVcutColor") != null) document.getElementById("ckVcutColor").checked = PARAMETER.ivcc;
	if (document.getElementById("ckVcutOff") != null && document.getElementById("ckVcutColor") != null) updateCheckbox(document.getElementById("ckVcutOff"), document.getElementById("ckVcutColor"));
	if (document.getElementById("ckVcutColor") != null && document.getElementById("ckVcutOff") != null) updateCheckbox(document.getElementById("ckVcutColor"), document.getElementById("ckVcutOff"));
	if (document.getElementById("sliderHcut") != null) document.getElementById("sliderHcut").value = PARAMETER.hc;
	if (document.getElementById("textHcut") != null) document.getElementById("textHcut").innerHTML = PARAMETER.hc + "px";
	if (document.getElementById("ckHcutOff") != null) document.getElementById("ckHcutOff").checked = PARAMETER.ihco;
	if (document.getElementById("ckHcutColor") != null) document.getElementById("ckHcutColor").checked = PARAMETER.ihcc;
	if (document.getElementById("ckHcutOff") != null && document.getElementById("ckHcutColor") != null) updateCheckbox(document.getElementById("ckHcutOff"), document.getElementById("ckHcutColor"));
	if (document.getElementById("ckHcutColor") != null && document.getElementById("ckHcutOff") != null) updateCheckbox(document.getElementById("ckHcutColor"), document.getElementById("ckHcutOff"));
	if (document.getElementById("pickerColor") != null) document.getElementById("pickerColor").value = PARAMETER.clr;
	if (document.getElementById("ckEndOff") != null) document.getElementById("ckEndOff").checked = PARAMETER.iedo;
	if (document.getElementById("ckEndColor") != null) document.getElementById("ckEndColor").checked = PARAMETER.iedc;
	if (document.getElementById("ckEndOff") != null && document.getElementById("ckEndColor") != null) updateCheckbox(document.getElementById("ckEndOff"), document.getElementById("ckEndColor"));
	if (document.getElementById("ckEndColor") != null && document.getElementById("ckEndOff") != null) updateCheckbox(document.getElementById("ckEndColor"), document.getElementById("ckEndOff"));
}

//--------------------------------------------------
function getParameter() {
	// get PARAMETER values
	if (document.getElementById("sliderParameterIndexStart") != null) PARAMETER.ist = document.getElementById("sliderParameterIndexStart").value;
	if (document.getElementById("sliderParameterIndexStop") != null) PARAMETER.isp = document.getElementById("sliderParameterIndexStop").value;
	if (document.getElementById("selectParameterBitmap") != null) PARAMETER.bmp = document.getElementById("selectParameterBitmap").value;
	// get PARAMETER values
	if (document.getElementById("sliderDelay") != null) PARAMETER.dly = document.getElementById("sliderDelay").value;
	if (document.getElementById("sliderBrightness") != null) PARAMETER.bts = document.getElementById("sliderBrightness").value;
	if (document.getElementById("sliderWait") != null) PARAMETER.wt = document.getElementById("sliderWait").value;
	if (document.getElementById("ckWait") != null) PARAMETER.iwt = document.getElementById("ckWait").checked;
	if (document.getElementById("sliderRepeat") != null) PARAMETER.rpt = document.getElementById("sliderRepeat").value;
	if (document.getElementById("ckInvert") != null) PARAMETER.iivt = document.getElementById("ckInvert").checked;
	if (document.getElementById("ckRepeat") != null) PARAMETER.irpt = document.getElementById("ckRepeat").checked;
	if (document.getElementById("ckBounce") != null) PARAMETER.ibnc = document.getElementById("ckBounce").checked;
	if (document.getElementById("sliderVcut") != null) PARAMETER.vc = document.getElementById("sliderVcut").value;
	if (document.getElementById("ckVcutOff") != null) PARAMETER.ivco = document.getElementById("ckVcutOff").checked;
	if (document.getElementById("ckVcutColor") != null) PARAMETER.ivcc = document.getElementById("ckVcutColor").checked;
	if (document.getElementById("sliderHcut") != null) PARAMETER.hc = document.getElementById("sliderHcut").value;
	if (document.getElementById("ckHcutOff") != null) PARAMETER.ihco = document.getElementById("ckHcutOff").checked;
	if (document.getElementById("ckHcutColor") != null) PARAMETER.ihcc = document.getElementById("ckHcutColor").checked;
	if (document.getElementById("pickerColor") != null) PARAMETER.clr = document.getElementById("pickerColor").value;
	if (document.getElementById("ckEndOff") != null) PARAMETER.iedo = document.getElementById("ckEndOff").checked;
	if (document.getElementById("ckEndColor") != null) PARAMETER.iedc = document.getElementById("ckEndColor").checked;
	// convert json to string
	return JSON.stringify(PARAMETER);
}

//--------------------------------------------------
function getParameterFile() {
	var parameterFile = "";
	if (document.getElementById("selectParameter") != null) parameterFile = document.getElementById("selectParameter").value
	return parameterFile;
}

//--------------------------------------------------
function requestParameterRead() {
	var xhr = new XMLHttpRequest();
	xhr.onload = function() {
		if (this.status == 200) {
			setParameter(this.responseText);
		}
	};
	
	xhr.onerror = function() {
		updateStatus("PARAMETER READ ERROR : CONNECTION LOST", "red");
	};
	
	xhr.overrideMimeType("application/json");
	xhr.open("GET", address + "/parameterRead", true);
	xhr.send(null);
}

//--------------------------------------------------
function requestParameterWrite() {
	var xhr = new XMLHttpRequest();
	xhr.onload = function() {
		if (this.status == 200) {
			updateStatus(this.responseText, "green");
		} else {
			updateStatus(this.responseText, "red");
		}
		requestParameterRead();
	};
	
	xhr.onerror = function() {
		updateStatus("PARAMETER WRITE ERROR : CONNECTION LOST", "red");
	};
	
	xhr.open("POST", address + "/parameterWrite", true);
	xhr.setRequestHeader('Content-type', 'application/json');
	xhr.send(getParameter());
}

//--------------------------------------------------
function requestParameterRestore() {
	var xhr = new XMLHttpRequest();
	xhr.onload = function() {
		if (this.status == 200) {
			updateStatus(this.responseText, "green");
		} else {
			updateStatus(this.responseText, "red");
		}
		requestParameterRead();
	};
	
	xhr.onerror = function() {
		updateStatus("PARAMETER RESTORE ERROR : CONNECTION LOST", "red");
	};
	
	xhr.open("POST", address + "/parameterRestore", true);
	xhr.setRequestHeader('Content-type', 'text/plain');
	xhr.send(getParameterFile());
}

//--------------------------------------------------
function requestParameterDefault() {
	var xhr = new XMLHttpRequest();
	xhr.onload = function() {
		if (this.status == 200) {
			updateStatus(this.responseText, "green");
		} else {
			updateStatus(this.responseText, "red");
		}
		requestParameterRead();
	};
	
	xhr.onerror = function() {
		updateStatus("PARAMETER DEFAULT ERROR : CONNECTION LOST", "red");
	};
	
	xhr.open("GET", address + "/parameterDefault", true);
	xhr.send(null);
}

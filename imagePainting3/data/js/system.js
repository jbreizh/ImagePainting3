//--------------------------------------------------
function setSystem(jsonString) {
    SYSTEM = JSON.parse(jsonString);
    if (document.getElementById("selectPlaylist") != null) {
        document.getElementById("selectPlaylist").length = 0;
        for (var file in SYSTEM.flt) {
            if (file.startsWith("playlist")&&fileExtension(file)=="json") document.getElementById("selectPlaylist").add(new Option(file, file));
        }
    }
    // set parameter values
    if (document.getElementById("selectParameter") != null) {
        document.getElementById("selectParameter").length = 0;
        for (var file in SYSTEM.flt) {
            if (file.startsWith("parameter")&&fileExtension(file)=="json") document.getElementById("selectParameter").add(new Option(file, file));
        }
    }
    if (document.getElementById("selectParameterBitmap") != null) {
        //var oldSelected = document.getElementById("selectParameterBitmap").value;
        document.getElementById("selectParameterBitmap").length = 0;
        for (var file in SYSTEM.flt) {
            if (fileExtension(file)=="bmp") document.getElementById("selectParameterBitmap").add(new Option(file, file));
            //if (file == oldSelected) document.getElementById("selectParameterBitmap").value = file;
            //if (document.getElementById("selectParameterBitmap").value != oldSelected) document.getElementById("selectParameterBitmap").value = "";
        }
        document.getElementById("selectParameterBitmap").value = PARAMETER.bmp;
    }
    // set convert values
    if (document.getElementById("sliderConvertPixels") != null) {
        document.getElementById("sliderConvertPixels").setAttribute("max", SYSTEM.ldi.pxs);
        document.getElementById("sliderConvertPixels").value = SYSTEM.ldi.pxs;
    }
    if (document.getElementById("textConvertPixels") != null) document.getElementById("textConvertPixels").innerHTML = SYSTEM.ldi.pxs + "px";
    if (document.getElementById("selectConvert") != null) document.getElementById("selectConvert").dispatchEvent(new Event('change'));
    // set generate values
    if (document.getElementById("sliderGeneratePixels") != null) {
        document.getElementById("sliderGeneratePixels").setAttribute("max", SYSTEM.ldi.pxs);
        document.getElementById("sliderGeneratePixels").value = SYSTEM.ldi.pxs;
    }
    if (document.getElementById("textGeneratePixels") != null) document.getElementById("textGeneratePixels").innerHTML = SYSTEM.ldi.pxs + "px";
    if (document.getElementById("btnGenerateRefresh") != null) document.getElementById("btnGenerateRefresh").click();
    // set system values
    if (document.getElementById("textSystemPixels") != null) document.getElementById("textSystemPixels").innerHTML = SYSTEM.ldi.pxs + "px";
    if (document.getElementById("selectSystem") != null) {
        document.getElementById("selectSystem").length = 0;
        for (var file in SYSTEM.flt) {
            document.getElementById("selectSystem").add(new Option(file, file));
        }
        if (document.getElementById("textSystemSize") != null) document.getElementById("textSystemSize").innerHTML = SYSTEM.flt[document.getElementById("selectSystem").value].fsz + "Bytes";
    }
    if (document.getElementById("canvasSystem") != null) {
        var myChart = new PieChart({
            canvas: document.getElementById("canvasSystem"),
                                   data: {	"Used": SYSTEM.fsi.ubs,	"Free": SYSTEM.fsi.fbs},
                                   colors: ["red", "green"],
        });
        // draw the chart
        myChart.draw();
    }
}

//--------------------------------------------------
function fileExtension(fileName) {
	// regex
	var extRegex = /(?:\.([^.]+))?$/;
	// retrieve extension
	return extRegex.exec(fileName)[1];
}

//--------------------------------------------------
function trimFileName(fileName, newExt) {
	// regex
	var extRegex = /(?:\.([^.]+))?$/;
	// retrieve current extension
	var currentExt = extRegex.exec(fileName)[1];
	// retrieve base name
	var baseName = fileName.substring(0, fileName.length - (currentExt.length + 1));
	// spiffs support maxi 31 characters (extension include) so we trim the baseName to 20 characters for security
	if (baseName.length > 20) {
		baseName = baseName.substring(0, 20);
	}
	// 
	var trimName;
	// keep current extension 
	if (newExt == "") {
		trimName = baseName + "." + currentExt;
	}
	// add the new extension
	else {
		trimName = baseName + "." + newExt;
	}
	return trimName;
}

//--------------------------------------------------
function download(url, name) {
	updateStatus("DOWNLOAD SUCCESS", "green");
	// create a link
	var a = document.createElement('a');
	// create a link
	a.href = url;
	// set the name of the link
	a.download = name;
	// download the link
	a.click();
}

//--------------------------------------------------
function upload(blob, name) {
	// too big? display an error
	if (blob.size > SYSTEM.fsi.fbs) {
		updateStatus("UPLOAD ERROR : NOT ENOUGH SPACE", "red");
	}
	// no problem? send the file
	else {
		var form = new FormData();
		form.append('file', blob, name);
		requestFileUpload(form);
	}
}

//--------------------------------------------------
function requestSystemRead() {
    var xhr = new XMLHttpRequest();
    xhr.onload = function() {
        if (this.status == 200) {
            setSystem(this.responseText);
        }
    };
    
    xhr.onerror = function() {
        updateStatus("SYSTEM READ ERROR : CONNECTION LOST", "red");
    };
    
    xhr.overrideMimeType("application/json");
    xhr.open("GET", address + "/systemRead", true);
    xhr.send(null);
}

//--------------------------------------------------
function requestFileUpload(form) {
    var xhr = new XMLHttpRequest();
    xhr.onload = function() {
        if (this.status == 200) {
            updateStatus(this.responseText, "green");
        } else {
            updateStatus("UPLOAD ERROR : UPLOAD FAILED", "red");
        }
        requestSystemRead();
    };
    
    xhr.upload.onprogress = function(evt) {
        var percentComplete = Math.floor(evt.loaded / evt.total * 100);
        updateStatus("UPLOAD PROGRESS :" + percentComplete + "%", "orange");
    };
    
    xhr.onerror = function() {
        updateStatus("UPLOAD ERROR : CONNECTION LOST", "red");
    };
    
    xhr.open("POST", address + "/upload", true);
    xhr.send(form);
}

//--------------------------------------------------
function requestFileDelete(fileName) {
    var xhr = new XMLHttpRequest();
    xhr.onload = function() {
        if (this.status == 200) {
            updateStatus(this.responseText, "green");
        } else {
            updateStatus(this.responseText, "red");
        }
        requestSystemRead();
    };
    
    xhr.onerror = function() {
        updateStatus("DELETE ERROR : CONNECTION LOST", "red");
    };
    
    xhr.open("DELETE", address + "/delete", true);
    xhr.setRequestHeader('Content-type', 'application/x-www-form-urlencoded');
    xhr.send(fileName);
}

//--------------------------------------------------
function requestParameterSave() {
	var xhr = new XMLHttpRequest();
	xhr.onload = function() {
		if (this.status == 200) {
			updateStatus(this.responseText, "green");
		} else {
			updateStatus(this.responseText, "red");
		}
		requestSystemRead();
	};
	
	xhr.onerror = function() {
		updateStatus("PARAMETER SAVE ERROR : CONNECTION LOST", "red");
	};
	
	xhr.open("GET", address + "/parameterSave", true);
	xhr.send(null);
}

//--------------------------------------------------
function requestPlaylistSave() {
	var xhr = new XMLHttpRequest();
	xhr.onload = function() {
		if (this.status == 200) {
			updateStatus(this.responseText, "green");
		} else {
			updateStatus(this.responseText, "red");
		}
		requestSystemRead();
	};
	
	xhr.onerror = function() {
		updateStatus("PLAYLIST SAVE ERROR : CONNECTION LOST", "red");
	};
	
	xhr.open("GET", address + "/playlistSave", true);
	xhr.send(null);
}

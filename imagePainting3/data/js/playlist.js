//--------------------------------------------------
function setPlaylist(jsonString) {
    PLAYLIST = JSON.parse(jsonString);
    // set playlist values
    if (document.getElementById("onsplaylist") != null) {
        var output = "";
        for (var i=0; i < PLAYLIST.length; i++){
            output += "<ons-list-item modifier=\"longdivider\">"+"<div class=\"left\">" + i + "</div>" + "<div class=\"center\">" + PLAYLIST[i].bmp + "</div>" + "<div class=\"right\"><ons-button modifier=\"quiet\" onclick=\"deleteItemPlaylist("+ i +");\"><ons-icon icon=\"myTrash\"></ons-icon></ons-button></div>" + "</ons-list-item>";
        }
        document.getElementById("onsplaylist").innerHTML = output;
    }
}

//--------------------------------------------------
function getPlaylist() {
    // convert json to string
    return JSON.stringify(PLAYLIST);
}

//--------------------------------------------------
function getPlaylistFile() {
	var playlistFile = "";
	if (document.getElementById("selectPlaylist") != null) playlistFile = document.getElementById("selectPlaylist").value
	return playlistFile;
}

// ------------------------------------------------------------
function deleteItemPlaylist(index) {
    PLAYLIST.splice(index,1);
    requestPlaylistWrite();
}

// ------------------------------------------------------------
function addItemPlaylist() {
	PLAYLIST.push(PARAMETER);
    requestPlaylistWrite();
}

//--------------------------------------------------
function requestPlaylistRead() {
    var xhr = new XMLHttpRequest();
    xhr.onload = function() {
        if (this.status == 200) {
            setPlaylist(this.responseText);
        }
    };
    
    xhr.onerror = function() {
        updateStatus("PLAYLIST READ ERROR : CONNECTION LOST", "red");
    };
    
    xhr.overrideMimeType("application/json");
    xhr.open("GET", address + "/playlistRead", true);
    xhr.send(null);
}


//--------------------------------------------------
function requestPlaylistWrite() {
    var xhr = new XMLHttpRequest();
    xhr.onload = function() {
        if (this.status == 200) {
            updateStatus(this.responseText, "green");
        } else {
            updateStatus(this.responseText, "red");
        }
        requestPlaylistRead();
    };
    
    xhr.onerror = function() {
        updateStatus("PLAYLIST WRITE ERROR : CONNECTION LOST", "red");
    };
    
    xhr.open("POST", address + "/playlistWrite", true);
    xhr.setRequestHeader('Content-type', 'application/json');
    xhr.send(getPlaylist());
}

//--------------------------------------------------
function requestPlaylistRestore() {
	var xhr = new XMLHttpRequest();
	xhr.onload = function() {
		if (this.status == 200) {
			updateStatus(this.responseText, "green");
		} else {
			updateStatus(this.responseText, "red");
		}
		requestPlaylistRead();
	};
	
	xhr.onerror = function() {
		updateStatus("PLAYLIST RESTORE ERROR : CONNECTION LOST", "red");
	};
	
	xhr.open("POST", address + "/playlistRestore", true);
	xhr.setRequestHeader('Content-type', 'text/plain');
	xhr.send(getPlaylistFile());
}

//--------------------------------------------------
function requestPlaylistDefault() {
	var xhr = new XMLHttpRequest();
	xhr.onload = function() {
		if (this.status == 200) {
			updateStatus(this.responseText, "green");
		} else {
			updateStatus(this.responseText, "red");
		}
		requestPlaylistRead();
	};
	
	xhr.onerror = function() {
		updateStatus("PLAYLIST DEFAULT ERROR : CONNECTION LOST", "red");
	};
	
	xhr.open("GET", address + "/playlistDefault", true);
	xhr.send(null);
}

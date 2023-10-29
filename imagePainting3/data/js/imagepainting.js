//"use strict";

// onsen menu
window.fn = {};

window.fn.open = function() {
	var menu = document.getElementById('menu');
	menu.open();
};

window.fn.load = function(page) {
	var content = document.getElementById('content');
	var menu = document.getElementById('menu');
	content.load(page)
	.then(menu.close.bind(menu));
};

// Global Variable
var address = "";
//var address = "http://192.168.46.83";
//var address = "http://192.168.46.55";
var ACTION = {};
var PARAMETER = {};
var SYSTEM = {};
var PLAYLIST = [];
var imgParameterBitmap = document.createElement('img');
var imgConvert = document.createElement('img');
var canvasGenerateTemp = document.createElement("canvas");

//--------------------------------------------------
function updateStatus(message, color) {
	if (document.getElementById("textStatus") != null) {
		document.getElementById("textStatus").innerHTML = message;
		document.getElementById("textStatus").style.color = color;
	}
	if (document.getElementById("iconStatus") != null) {
		if (color == 'red') document.getElementById("iconStatus").setAttribute('icon', 'myStatusRed');
		if (color == 'orange') document.getElementById("iconStatus").setAttribute('icon', 'myStatusOrange');
		if (color == 'green') document.getElementById("iconStatus").setAttribute('icon', 'myStatusGreen');
	}
}

//--------------------------------------------------
function updateCheckbox(checkboxFrom, checkboxTo) {
	if (checkboxFrom.checked) {
		checkboxTo.disabled = true;
	} else {
		checkboxTo.disabled = false;
	}
}

//--------------------------------------------------
document.addEventListener('init', function(event) {
	
	if (event.target.matches('#actions')) {
		// Status Event--------------------------------------------------
		document.getElementById("btnStatus").addEventListener('click', function() {
			document.getElementById("popoverStatus").show(document.getElementById("btnStatus"));
		}, false);
		
		// Actions Variable--------------------------------------------------
		var ckPlaylist = document.getElementById("ckPlaylist");
		var ckTrigger = document.getElementById("ckTrigger");
		var btnActionLight = document.getElementById("btnActionLight");
		var btnActionBurn = document.getElementById("btnActionBurn");
		var btnActionStop = document.getElementById("btnActionStop");
		var btnActionPlay = document.getElementById("btnActionPlay");
		
		// Actions Event--------------------------------------------------
		ckPlaylist.addEventListener('click', requestActionWrite, false);
		ckTrigger.addEventListener('click', requestActionWrite, false);
		btnActionLight.addEventListener('click', function() {
			requestAction("/light");
		}, false);
		btnActionBurn.addEventListener('click', function() {
			requestAction("/burn");
		}, false);
		btnActionStop.addEventListener('click', function() {
			requestAction("/stop");
		}, false);
		btnActionPlay.addEventListener('click', function() {
			requestAction("/play");
		}, false);
		
		// Main ------------------------------------------------------------
		requestActionRead();
	}
	
	if (event.target.matches('#playlist')) {
		// Status Event--------------------------------------------------
		document.getElementById("btnStatus").addEventListener('click', function() {
			document.getElementById("popoverStatus").show(document.getElementById("btnStatus"));
		}, false);
		
		// Playlist Variable--------------------------------------------------
		var selectPlaylist = document.getElementById("selectPlaylist");
		var btnPlaylistRestore = document.getElementById("btnPlaylistRestore");
		var btnPlaylistDelete = document.getElementById("btnPlaylistDelete");
		var btnPlaylistSave = document.getElementById("btnPlaylistSave");
		var btnPlaylistDefault = document.getElementById("btnPlaylistDefault");
		var btnPlaylistFill = document.getElementById("btnPlaylistFill");
		
		// Playlist Event--------------------------------------------------
		btnPlaylistSave.addEventListener('click', requestPlaylistSave, false);
		btnPlaylistRestore.addEventListener('click', requestPlaylistRestore, false);
		btnPlaylistDefault.addEventListener('click', requestPlaylistDefault, false);
		btnPlaylistDelete.addEventListener('click', function() {
			requestFileDelete(selectPlaylist.value);
		}, false);
		btnPlaylistFill.addEventListener('click', function() {
			fn.load('parameters.html');
		}, false);
		
		// Main ------------------------------------------------------------
		requestSystemRead();
		requestPlaylistRead();
	}
	
	if (event.target.matches('#parameters')) {
		
		// Status Event--------------------------------------------------
		document.getElementById("btnStatus").addEventListener('click', function() {
			document.getElementById("popoverStatus").show(document.getElementById("btnStatus"));
		}, false);
		
		// Parameters Variable--------------------------------------------------
		var selectParameter = document.getElementById("selectParameter");
		var selectParameterBitmap = document.getElementById("selectParameterBitmap");
		var canvasParameterBitmap = document.getElementById("canvasParameterBitmap");
		var sliderParameterIndexStart = document.getElementById("sliderParameterIndexStart");
		var textParameterIndexStart = document.getElementById("textParameterIndexStart");
		var sliderParameterIndexStop = document.getElementById("sliderParameterIndexStop");
		var textParameterIndexStop = document.getElementById("textParameterIndexStop");
		var sliderDelay = document.getElementById("sliderDelay");
		var textDelay = document.getElementById("textDelay");
		var sliderBrightness = document.getElementById("sliderBrightness");
		var textBrightness = document.getElementById("textBrightness");
		var ckInvert = document.getElementById("ckInvert");
		var sliderWait = document.getElementById("sliderWait");
		var textWait = document.getElementById("textWait");
		var sliderRepeat = document.getElementById("sliderRepeat");
		var textRepeat = document.getElementById("textRepeat");
		var sliderVcut = document.getElementById("sliderVcut");
		var textVcut = document.getElementById("textVcut");
		var sliderHcut = document.getElementById("sliderHcut");
		var textHcut = document.getElementById("textHcut");
		var pickerColor = document.getElementById("pickerColor");
		var ckWait = document.getElementById("ckWait");
		var ckRepeat = document.getElementById("ckRepeat");
		var ckBounce = document.getElementById("ckBounce");
		var ckVcutOff = document.getElementById("ckVcutOff");
		var ckVcutColor = document.getElementById("ckVcutColor");
		var ckHcutOff = document.getElementById("ckHcutOff");
		var ckHcutColor = document.getElementById("ckHcutColor");
		var ckAlternate = document.getElementById("ckAlternate");
		var ckEndOff = document.getElementById("ckEndOff");
		var ckEndColor = document.getElementById("ckEndColor");
		var btnParameterSave = document.getElementById("btnParameterSave");
		var btnParameterRestore = document.getElementById("btnParameterRestore");
		var btnParameterDefault = document.getElementById("btnParameterDefault");
		var btnParameterAdd = document.getElementById("btnParameterAdd");
		var btnParameterDelete = document.getElementById("btnParameterDelete");
		
		// Parameters Event--------------------------------------------------
		selectParameterBitmap.addEventListener('change', requestParameterWrite, false);
		imgParameterBitmap.addEventListener('load', function() {
			drawAction(imgParameterBitmap, canvasParameterBitmap, sliderParameterIndexStart.value, sliderParameterIndexStop.value);
		}, false);
		imgParameterBitmap.addEventListener('error', function() {
			drawError("No File", canvasParameterBitmap);
		}, false);
		sliderParameterIndexStart.addEventListener('input', function() {
			// update textParameterIndexStart < textParameterIndexStop
			sliderParameterIndexStop.value = Math.max(sliderParameterIndexStart.value, sliderParameterIndexStop.value);
			textParameterIndexStart.innerHTML = sliderParameterIndexStart.value + "px";
			textParameterIndexStop.innerHTML = sliderParameterIndexStop.value + "px";
			drawAction(imgParameterBitmap, canvasParameterBitmap, sliderParameterIndexStart.value, sliderParameterIndexStop.value);
		}, false);
		sliderParameterIndexStart.addEventListener('change', requestParameterWrite, false);
		sliderParameterIndexStop.addEventListener('input', function() {
			// update textParameterIndexStart < textParameterIndexStop
			sliderParameterIndexStart.value = Math.min(sliderParameterIndexStart.value, sliderParameterIndexStop.value);
			textParameterIndexStart.innerHTML = sliderParameterIndexStart.value + "px";
			textParameterIndexStop.innerHTML = sliderParameterIndexStop.value + "px";
			drawAction(imgParameterBitmap, canvasParameterBitmap, sliderParameterIndexStart.value, sliderParameterIndexStop.value);
		}, false);
		sliderParameterIndexStop.addEventListener('change', requestParameterWrite, false);
		sliderDelay.addEventListener('input', function() {
			textDelay.innerHTML = sliderDelay.value + "ms";
		}, false);
		sliderBrightness.addEventListener('input', function() {
			textBrightness.innerHTML = sliderBrightness.value + "%";
		}, false);
		sliderWait.addEventListener('input', function() {
			textWait.innerHTML = sliderWait.value + "px";
		}, false);
		sliderRepeat.addEventListener('input', function() {
			textRepeat.innerHTML = sliderRepeat.value + "x";
		}, false);
		sliderVcut.addEventListener('input', function() {
			textVcut.innerHTML = sliderVcut.value + "px";
		}, false);
		sliderHcut.addEventListener('input', function() {
			textHcut.innerHTML = sliderHcut.value + "px";
		}, false);
		sliderDelay.addEventListener('change', requestParameterWrite, false);
		sliderBrightness.addEventListener('change', requestParameterWrite, false);
		sliderWait.addEventListener('change', requestParameterWrite, false);
		sliderRepeat.addEventListener('change', requestParameterWrite, false);
		sliderVcut.addEventListener('change', requestParameterWrite, false);
		sliderHcut.addEventListener('change', requestParameterWrite, false);
		pickerColor.addEventListener('change', requestParameterWrite, false);
		ckRepeat.addEventListener('click', function() {
			updateCheckbox(ckRepeat, ckBounce);
		}, false);
		ckBounce.addEventListener('click', function() {
			updateCheckbox(ckBounce, ckRepeat);
		}, false);
		ckVcutOff.addEventListener('click', function() {
			updateCheckbox(ckVcutOff, ckVcutColor);
		}, false);
		ckVcutColor.addEventListener('click', function() {
			updateCheckbox(ckVcutColor, ckVcutOff);
		}, false);
		ckHcutOff.addEventListener('click', function() {
			updateCheckbox(ckHcutOff, ckHcutColor);
		}, false);
		ckHcutColor.addEventListener('click', function() {
			updateCheckbox(ckHcutColor, ckHcutOff);
		}, false);
		ckEndColor.addEventListener('click', function() {
			updateCheckbox(ckEndColor, ckEndOff);
		}, false);
		ckEndOff.addEventListener('click', function() {
			updateCheckbox(ckEndOff, ckEndColor);
		}, false);
		ckWait.addEventListener('click', requestParameterWrite, false);
		ckInvert.addEventListener('click', requestParameterWrite, false);
		ckRepeat.addEventListener('click', requestParameterWrite, false);
		ckBounce.addEventListener('click', requestParameterWrite, false);
		ckVcutOff.addEventListener('click', requestParameterWrite, false);
		ckVcutColor.addEventListener('click', requestParameterWrite, false);
		ckHcutOff.addEventListener('click', requestParameterWrite, false);
		ckHcutColor.addEventListener('click', requestParameterWrite, false);
		ckAlternate.addEventListener('click', requestParameterWrite, false);
		ckEndColor.addEventListener('click', requestParameterWrite, false);
		ckEndOff.addEventListener('click', requestParameterWrite, false);
		btnParameterSave.addEventListener('click', requestParameterSave, false);
		btnParameterRestore.addEventListener('click', requestParameterRestore, false);
		btnParameterDefault.addEventListener('click', requestParameterDefault, false);
		btnParameterAdd.addEventListener('click', function() {
			addItemPlaylist();
			fn.load('playlist.html');
		}, false);
		btnParameterDelete.addEventListener('click', function() {
			requestFileDelete(selectParameter.value);
		}, false);
		
		// Main --------------------------------------------------
		requestSystemRead();
		requestParameterRead();
	}
	
	if (event.target.matches('#convert')) {
		// Status Event--------------------------------------------------
		document.getElementById("btnStatus").addEventListener('click', function() {
			document.getElementById("popoverStatus").show(document.getElementById("btnStatus"));
		}, false);
		
		// Convert Variable--------------------------------------------------
		var canvasConvert = document.getElementById("canvasConvert");
		var selectConvert = document.getElementById("selectConvert");
		var selectConvertGamma = document.getElementById("selectConvertGamma");
		var selectConvertOrientation = document.getElementById("selectConvertOrientation");
		var sliderConvertPixels = document.getElementById("sliderConvertPixels");
		var textConvertPixels = document.getElementById("textConvertPixels");
		var btnConvertUpload = document.getElementById("btnConvertUpload");
		var btnConvertDownload = document.getElementById("btnConvertDownload");
		
		// Convert event--------------------------------------------------
		selectConvert.addEventListener('change', function() {
			var file = selectConvert.files[0];
			if (file) {
				imgConvert.file = file;
				var reader = new FileReader();
				reader.onload = (function(aImg) {
					return function(e) {
						aImg.src = e.target.result;
					};
				})(imgConvert);
				reader.readAsDataURL(file);
			} else {
				drawError("No File", canvasConvert);
				selectConvertGamma.setAttribute('disabled', '');
				selectConvertOrientation.setAttribute('disabled', '');
				sliderConvertPixels.setAttribute('disabled', '');
				btnConvertUpload.setAttribute('disabled', '');
				btnConvertDownload.setAttribute('disabled', '');
			}
		}, false);
		imgConvert.addEventListener('load', function() {
			drawConvert(imgConvert, canvasConvert, selectConvertOrientation.value, selectConvertGamma.value, sliderConvertPixels.value, sliderConvertPixels.getAttribute("max"));
			selectConvertGamma.removeAttribute('disabled', '');
			selectConvertOrientation.removeAttribute('disabled', '');
			sliderConvertPixels.removeAttribute('disabled', '');
			btnConvertUpload.removeAttribute('disabled', '');
			btnConvertDownload.removeAttribute('disabled', '');
			
		}, false);
		imgConvert.addEventListener('error', function() {
			drawError("No Convert", canvasConvert);
			selectConvertGamma.setAttribute('disabled', '');
			selectConvertOrientation.setAttribute('disabled', '');
			sliderConvertPixels.setAttribute('disabled', '');
			btnConvertUpload.setAttribute('disabled', '');
			btnConvertDownload.setAttribute('disabled', '');
			
		}, false);
		selectConvertGamma.addEventListener('change', function() {
			drawConvert(imgConvert, canvasConvert, selectConvertOrientation.value, selectConvertGamma.value, sliderConvertPixels.value, sliderConvertPixels.getAttribute("max"));
			
		}, false);
		selectConvertOrientation.addEventListener('change', function() {
			drawConvert(imgConvert, canvasConvert, selectConvertOrientation.value, selectConvertGamma.value, sliderConvertPixels.value, sliderConvertPixels.getAttribute("max"));
			
		}, false);
		sliderConvertPixels.addEventListener('change', function() {
			textConvertPixels.innerHTML = sliderConvertPixels.value + "px";
			drawConvert(imgConvert, canvasConvert, selectConvertOrientation.value, selectConvertGamma.value, sliderConvertPixels.value, sliderConvertPixels.getAttribute("max"));
			
		}, false);
		btnConvertUpload.addEventListener('click', function() {
			upload(CanvasToBMP.toBlob(canvasConvert), getFileBasename(selectConvert.files[0].name)+".bmp");
		}, false);
		btnConvertDownload.addEventListener('click', function() {
			download(CanvasToBMP.toDataURL(canvasConvert), getFileBasename(selectConvert.files[0].name)+".bmp");
		}, false);
		
		// Main --------------------------------------------------
		requestSystemRead();
	}
	
	if (event.target.matches('#generate')) {
		// Status Event--------------------------------------------------
		document.getElementById("btnStatus").addEventListener('click', function() {
			document.getElementById("popoverStatus").show(document.getElementById("btnStatus"));
		}, false);
		
		// Generate Variable--------------------------------------------------
		var canvasGenerate = document.getElementById("canvasGenerate");
		var selectGenerate = document.getElementById("selectGenerate");
		var selectGenerateGamma = document.getElementById("selectGenerateGamma");
		var selectGenerateOrientation = document.getElementById("selectGenerateOrientation");
		var sliderGeneratePixels = document.getElementById("sliderGeneratePixels");
		var textGeneratePixels = document.getElementById("textGeneratePixels");
		var btnGenerateRefresh = document.getElementById("btnGenerateRefresh");
		var btnGenerateUpload = document.getElementById("btnGenerateUpload");
		var btnGenerateDownload = document.getElementById("btnGenerateDownload");
		
		// Generate event--------------------------------------------------
		selectGenerate.addEventListener('change', function() {
			generate(canvasGenerateTemp, selectGenerate.value);
			drawConvert(canvasGenerateTemp, canvasGenerate, selectGenerateOrientation.value, selectGenerateGamma.value, sliderGeneratePixels.value, sliderGeneratePixels.getAttribute("max"));
		}, false);
		selectGenerateGamma.addEventListener('change', function() {
			drawConvert(canvasGenerateTemp, canvasGenerate, selectGenerateOrientation.value, selectGenerateGamma.value, sliderGeneratePixels.value, sliderGeneratePixels.getAttribute("max"));
		}, false);
		selectGenerateOrientation.addEventListener('change', function() {
			drawConvert(canvasGenerateTemp, canvasGenerate, selectGenerateOrientation.value, selectGenerateGamma.value, sliderGeneratePixels.value, sliderGeneratePixels.getAttribute("max"));
		}, false);
		sliderGeneratePixels.addEventListener('input', function() {
			textGeneratePixels.innerHTML = sliderGeneratePixels.value + "px";
		}, false);
		sliderGeneratePixels.addEventListener('change', function() {
			drawConvert(canvasGenerateTemp, canvasGenerate, selectGenerateOrientation.value, selectGenerateGamma.value, sliderGeneratePixels.value, sliderGeneratePixels.getAttribute("max"));
		}, false);
		btnGenerateRefresh.addEventListener('click', function() {
			generate(canvasGenerateTemp, selectGenerate.value);
			drawConvert(canvasGenerateTemp, canvasGenerate, selectGenerateOrientation.value, selectGenerateGamma.value, sliderGeneratePixels.value, sliderGeneratePixels.getAttribute("max"));
		}, false);
		btnGenerateDownload.addEventListener('click', function() {
			download(CanvasToBMP.toDataURL(canvasGenerate), selectGenerate.value + ".bmp");
		}, false);
		btnGenerateUpload.addEventListener('click', function() {
			upload(CanvasToBMP.toBlob(canvasGenerate), selectGenerate.value + ".bmp");
		}, false);
		
		//Main--------------------------------------------------
		requestSystemRead();
	}
	
	if (event.target.matches('#system')) {
		// Status Event--------------------------------------------------
		document.getElementById("btnStatus").addEventListener('click', function() {
			document.getElementById("popoverStatus").show(document.getElementById("btnStatus"));
		}, false);
		
		// System Variable--------------------------------------------------
		var selectSystem = document.getElementById("selectSystem");
		var textSystemSize = document.getElementById("textSystemSize");
		var btnSystemDelete = document.getElementById("btnSystemDelete");
		var btnSystemDownload = document.getElementById("btnSystemDownload");
		var selectSystemUpload = document.getElementById("selectSystemUpload");
		var btnSystemUpload = document.getElementById("btnSystemUpload");
		
		// System Event--------------------------------------------------
		selectSystem.addEventListener('change', function() {
			textSystemSize.innerHTML = SYSTEM.flt[selectSystem.value].fsz + "Bytes";
		}, false);
		btnSystemDelete.addEventListener('click', function() {
			requestFileDelete(selectSystem.value);
		}, false);
		btnSystemDownload.addEventListener('click', function() {
			download(address + "/" + selectSystem.value, selectSystem.value.substring(1));
		}, false);
		btnSystemUpload.addEventListener('click', function() {
			if (selectSystemUpload.files[0]) upload(selectSystemUpload.files[0], selectSystemUpload.files[0].name);
			else updateStatus("UPLOAD ERROR : NO FILE", "red");
		}, false);
		
		//Main--------------------------------------------------
		requestSystemRead();
	}
	
}, false);

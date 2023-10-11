//--------------------------------------------------
function drawError(errorFrom, canvasTo)
{
	// context
	var ctxTo=canvasTo.getContext("2d");
	// calculate the canvasTo dimension
	canvasTo.width = 150;
	canvasTo.height = 60; 
	// initialize canvasTo in black
	ctxTo.fillStyle = "black";
	ctxTo.fillRect(0, 0, canvasTo.width, canvasTo.height);
	// print the errorFrom
	ctxTo.fillStyle = "red";
	ctxTo.font = "30px Arial";
	ctxTo.textAlign = "center";
	ctxTo.fillText(errorFrom, canvasTo.width/2, canvasTo.height/2);
}

//--------------------------------------------------
function drawAction(imageFrom, canvasTo, start, stop)
{
	// context
	var ctxTo=canvasTo.getContext("2d");
	// calculate the canvasTo dimension
	canvasTo.width = imageFrom.height;
	canvasTo.height = imageFrom.width;
	// initialize canvasTo in black
	ctxTo.fillStyle = "black";
	ctxTo.fillRect(0, 0, canvasTo.width, canvasTo.height);
	// translate
	ctxTo.translate(canvasTo.width/2,canvasTo.height/2);
	// rotate
	ctxTo.rotate(-90*Math.PI/180);
	// draw imageFrom in canvasTo
	ctxTo.drawImage(imageFrom, -canvasTo.height/2, -canvasTo.width/2);
	// restore context
    ctxTo.setTransform(1, 0, 0, 1, 0, 0);
	// curtain color
	ctxTo.fillStyle = "red";
	// draw curtain
	ctxTo.fillRect(0, 0, Number(start), canvasTo.height);
	ctxTo.fillRect(Number(stop)+1, 0, canvasTo.width-Number(stop), canvasTo.height);
}

//--------------------------------------------------
function drawConvert(imageFrom, canvasTo, orientation ,gamma, pixel, pixels)
{
	// context
	var ctxTo = canvasTo.getContext("2d");
	// calculate the canvasTo dimension
	canvasTo.width = pixels;
	if(orientation==-90 || orientation==90)
	{
		canvasTo.height = imageFrom.width/imageFrom.height*pixel;
	}
	else
	{
		canvasTo.height = imageFrom.height/imageFrom.width*pixel;
	}
	// initialize canvasTo in black
	ctxTo.fillStyle = "black";
	ctxTo.fillRect(0, 0, canvasTo.width, canvasTo.height);
	// translate
	ctxTo.translate(canvasTo.width/2,canvasTo.height/2);
	// rotate
    ctxTo.rotate(orientation*Math.PI/180);
	// draw imageFrom in canvasTo
	if(orientation==-90 || orientation==90)
	{
		ctxTo.drawImage(imageFrom,-canvasTo.height/2,-pixel/2,canvasTo.height,pixel);

	}
	else
	{
		ctxTo.drawImage(imageFrom,-pixel/2,-canvasTo.height/2,pixel,canvasTo.height);
	}
	// restore context
    ctxTo.setTransform(1, 0, 0, 1, 0, 0);
	// store canvasTo in data
	var imageData = ctxTo.getImageData(0.0, 0.0, canvasTo.width, canvasTo.height);
	var data = imageData.data;
	//adjust gamma
	for (var i = 0; i < data.length; i++)
	{
		data[i] = 255 * Math.pow((data[i] / 255), gamma);
	}
	// put data in canvasTo
	ctxTo.putImageData(imageData, 0, 0);
}

	////cut the line
	//for (var i = 0; i < data.length; i += 4)
	//{
	//	if (i%(4*canvas.width)%(8*cut)<4*cut)
	//	{
	//		data[i] = 0;
	//		data[i+1] = 0;
	//		data[i+2] = 0;
	//	}
	//}

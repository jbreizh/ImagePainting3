//--------------------------------------------------
function generate(canvas, effect)
{
    if(effect=="generateCircle") generateCircle(canvas);
    else if (effect=="generateRainbow") generateRainbow(canvas);
}

//--------------------------------------------------
function generateCircle(canvas)
{
    // canvas
    var ctx=canvas.getContext("2d");
    // save context
	ctx.save();
    // initialize canvas in black
    ctx.fillStyle = "black";
    ctx.fillRect(0, 0, canvas.width, canvas.height);
    //
    for(var i=0; i<300; i++) {
        
        var x = Math.floor(Math.random()*canvas.width);
        var y = Math.floor(Math.random()*canvas.height);
        var radius = Math.floor(Math.random()*Math.min(canvas.width, canvas.height)/20);

        var r = Math.floor(Math.random()*255);
        var g = Math.floor(Math.random()*255);
        var b = Math.floor(Math.random()*255);
        
        ctx.beginPath();
        ctx.arc(x,y,radius,Math.PI*2,0,false);
        ctx.fillStyle = "rgba(" + r + "," + g + "," + b + ",1)";
        ctx.fill();
        ctx.closePath();
    }
    //restore context
	ctx.restore();
}

//--------------------------------------------------
function generateRainbow(canvas)
{
    // canvas
    var ctx=canvas.getContext("2d");
    // save context
    ctx.save();
    // initialize canvas in black
    ctx.fillStyle = "black";
    ctx.fillRect(0, 0, canvas.width, canvas.height);
    var color = ['red', 'orange', 'yellow', 'green', 'blue', 'indigo', 'violet'];
    shuffleArray(color);
    //     
    var gradient = ctx.createLinearGradient(0, 0, canvas.width, 0);
    for(var i=0; i<color.length; i++) {
        gradient.addColorStop(i/(color.length-1), color[i]); 
    }
    ctx.fillStyle = gradient;
    ctx.fillRect(0, 0, canvas.width, canvas.height);
    //restore context
	ctx.restore();
}

//----Randomize array in-place using Durstenfeld shuffle algorithm
function shuffleArray(array) {
    for (var i = array.length - 1; i > 0; i--) {
        var j = Math.floor(Math.random() * (i + 1));
        var temp = array[i];
        array[i] = array[j];
        array[j] = temp;
    }
}

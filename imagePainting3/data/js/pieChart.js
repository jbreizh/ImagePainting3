//--------------------------------------------------
function drawPieSlice(ctx, centerX, centerY, radius, startAngle, endAngle, fillColor, strokeColor) {
	ctx.save();
	ctx.fillStyle = fillColor;
	ctx.strokeStyle = strokeColor;
	ctx.beginPath();
	ctx.moveTo(centerX, centerY);
	ctx.arc(centerX, centerY, radius, startAngle, endAngle, strokeColor);
	ctx.closePath();
	ctx.fill();
	ctx.restore();
}

//--------------------------------------------------
class PieChart {
	constructor(options) {
		this.options = options;
		this.canvas = options.canvas;
		this.ctx = this.canvas.getContext("2d");
		this.colors = options.colors;
		this.totalValue = [...Object.values(this.options.data)].reduce((a, b) => a + b, 0);
		this.radius = Math.min(this.canvas.width / 2, this.canvas.height / 2);
	}
	
	//drawing the pourcent of each slice
	drawSlices() {
		var colorIndex = 0;
		var startAngle = -Math.PI / 2;
		for (var categ in this.options.data) {
			var val = this.options.data[categ];
			var sliceAngle = (2 * Math.PI * val) / this.totalValue;
			drawPieSlice(
				this.ctx,
				this.canvas.width / 2,
				this.canvas.height / 2,
				this.radius,
				startAngle,
				startAngle + sliceAngle,
				this.colors[colorIndex % this.colors.length]
				);
			startAngle += sliceAngle;
			colorIndex++;
		}
	}
	
	//drawing the pourcent of each slice
	drawLabels() {
		var colorIndex = 0;
		var startAngle = -Math.PI / 2;
		for (var categ in this.options.data) {
			var val = this.options.data[categ];
			var sliceAngle = (2 * Math.PI * val) / this.totalValue;
			var labelX = this.canvas.width / 2 + (this.radius / 2) * Math.cos(startAngle + sliceAngle / 2);
			var labelY = this.canvas.height / 2 +(this.radius / 2) * Math.sin(startAngle + sliceAngle / 2);
			var labelText = Math.round((100 * val) / this.totalValue);
			this.ctx.save();
			this.ctx.fillStyle = "white";
			this.ctx.font = "bold 20px Arial";
			this.ctx.textAlign = "center";
			this.ctx.fillText(categ + ":" + labelText + "%", labelX, labelY);
			this.ctx.restore();
			startAngle += sliceAngle;
		}
	}
	
	//
	draw() {
		this.drawSlices();
		this.drawLabels();
	}
}

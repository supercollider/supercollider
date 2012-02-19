SCPen {

	classvar font, fillColor, strokeColor;

	*font_ { arg argFont;
		font = argFont;
	}

	*string { arg str;
		str.prDrawAtPoint( Point( 0, 0 ), font ? SCFont.default, fillColor ? Color.black );
	}

	*stringAtPoint { arg str, point;
		str.prDrawAtPoint( point, font ? SCFont.default, fillColor ? Color.black );
	}

	*stringInRect { arg str, rect;
		str.prDrawInRect( rect, font ? SCFont.default, fillColor ? Color.black );
	}

	*stringCenteredIn { arg str, inRect;
		var f = font ? SCFont.default;
		str.prDrawAtPoint(str.bounds( f ).centerIn(inRect), f, fillColor ? Color.black);	}

	*stringLeftJustIn { arg str, inRect;
		var pos, bounds, f;
		f = font ? SCFont.default;
		bounds = str.prBounds( Rect.new, f );
		pos = bounds.centerIn(inRect);
		pos.x = inRect.left + 2;
		str.prDrawAtPoint(pos, f, fillColor ? Color.black);
	}

	*stringRightJustIn { arg str, inRect;
		var pos, bounds, f;
		f = font ? SCFont.default;
		bounds = str.prBounds( Rect.new, f );
		pos = bounds.centerIn(inRect);
		pos.x = inRect.right - 2 - bounds.width;
		str.prDrawAtPoint(pos, f, fillColor ? Color.black);
	}

	*image { arg img;
		img.drawAtPoint( Point( 0, 0 ), img.bounds );
	}

	*imageAtPoint { arg img, point;
		img.drawAtPoint( point, img.bounds );
	}

	*strokeColor_ { arg color;
		strokeColor = color;
		color.cocoaPrSetStroke;
	}

	*fillColor_ { arg color;
		fillColor = color;
		color.cocoaPrSetFill;
	}

	*color_ { arg color;
		color = color ? Color.black;
		fillColor = color;
		strokeColor = color;
		color.cocoaPrSetStroke.cocoaPrSetFill;
	}

	*use { arg function;
		var res;
		this.push;
//		this.prReset;
		res = function.value;
		this.pop;
		^res
	}

	*prReset{
		this.color_(nil)
	}

	*translate { arg x=0, y=0;
		_Pen_Translate
		^this.primitiveFailed
	}

	*scale { arg x=0, y=0;
		_Pen_Scale
		^this.primitiveFailed
	}

	*skew { arg x=0, y=0;
		_Pen_Skew
		^this.primitiveFailed
	}

	*rotate { arg angle=0, x=0, y=0;
		_Pen_Rotate
		^this.primitiveFailed
	}

	*width_ { arg width = 1;
		_Pen_SetWidth
		^this.primitiveFailed
	}

	// Paths:
	*path { arg function;
		var res;
		this.beginPath;
		res = function.value;
		this.endPath;
		^res
	}

	*beginPath {
		_Pen_BeginPath
		^this.primitiveFailed
	}

	*moveTo { arg point;
		_Pen_MoveTo
		^this.primitiveFailed
	}

	*lineTo { arg point;
		_Pen_LineTo
		^this.primitiveFailed
	}

	*line { arg p1, p2;
		^this.moveTo(p1).lineTo(p2);
	}

	*addArc { arg center, radius, startAngle, arcAngle;
		_Pen_AddArc
		^this.primitiveFailed
	}

	*addWedge { arg center, radius, startAngle, arcAngle;
		_Pen_AddWedge
		^this.primitiveFailed
	}

	*addAnnularWedge { arg center, innerRadius, outerRadius, startAngle, arcAngle;
		_Pen_AddAnnularWedge
		^this.primitiveFailed
	}

	*addRect { arg rect;
		_Pen_AddRect
		^this.primitiveFailed
	}

	*stroke {
		_Pen_StrokePath
		^this.primitiveFailed
	}

	*fill {
		_Pen_FillPath
		^this.primitiveFailed
	}

	*clip {
		_Pen_ClipPath
		^this.primitiveFailed
	}
	*matrix_ { arg array;
		_Pen_SetMatrix
		^this.primitiveFailed
	}

	*strokeRect { arg rect;
		_Pen_StrokeRect
		^this.primitiveFailed
	}

	*fillRect { arg rect;
		_Pen_FillRect
		^this.primitiveFailed
	}

	*strokeOval { arg rect;
		_Pen_StrokeOval
		^this.primitiveFailed
	}

	*fillOval { arg rect;
		_Pen_FillOval
		^this.primitiveFailed
	}

	*drawAquaButton { arg rect, type=0, down = false, on = false;
		_Pen_DrawAquaButton
		^this.primitiveFailed
	}

	*setSmoothing { arg flag = true;
		this.deprecated(thisMethod, Meta_SCPen.findRespondingMethodFor(\smoothing_));
		this.smoothing = flag;
	}

	*smoothing_ { arg flag = true;
		_Pen_SetSmoothing
		^this.primitiveFailed
	}

	//ADDITIONS:

	*fillStroke {
		this.draw(3);
	}

	*clearRect {arg aRect=nil;
		_Pen_ClearRect
		^this.primitiveFailed
	}

	*arcTo { arg point1, point2, radius;
		_Pen_AddArcToPoint
		^this.primitiveFailed
	}

	*curveTo { arg point, cpoint1, cpoint2;
		_Pen_AddCubicCurve
		^this.primitiveFailed
	}

	*quadCurveTo { arg point, cpoint1;
		_Pen_AddQuadCurve
		^this.primitiveFailed
	}

	*alpha_ {arg opacity;
		_Pen_SetAlpha
		^this.primitiveFailed
	}

	/*
	different blend modes:
	OS X 10.4 and > Only
	--------------------
	0 - Normal
	1 - Multiply
	2 - Screen
	3 - Overlay
	4 - Darken
	5 - Lighten
	6 - ColorDodge
	7 - ColorBurn
	8 - SoftLight
	9 - HardLight
	10 - Difference
	11 - Exclusion
	12 - Hue
	13 - Saturation
	14 - Color
	15 - Luminosity

	OS 10.5 and > Only
	--------------------
	16 - Clear
	17 - Copy
	18 - SourceIn
	19 - SourceOut
	20 - SourceATop
	21 - DestinationOver
	22 - DestinationIn
	23 - DestinationATop
	24 - XOR
	25 - PlusDarker
	26 - PlusLighter
	*/

	*blendMode_{ arg mode;
		_Pen_SetBlendMode
		^this.primitiveFailed
	}

	*setShadow { arg offsetPoint=Point(2,2), blur=0.5, color=Color.black;
		this.prSetShadow(offsetPoint, blur, color);
	}

	*prSetShadow { arg offsetPoint, blur, color;
		_Pen_SetShadow
		^this.primitiveFailed
	}

	*beginTransparencyLayer {
		_Pen_BeginTLayer
		^this.primitiveFailed
	}

	*endTransparencyLayer { // will work only for Mac Os X >= 10.3 - does nothing for others
		_Pen_EndTLayer
		^this.primitiveFailed
	}

	*draw { arg option = 0;// 0 = fill, 1 = eofill, 2 = stroke, 3 = fillstroke, 4 = eofillstroke
		_Pen_DrawPath
		^this.primitiveFailed
	}

	*joinStyle_ { arg style = 0; // 0 = miter, 1 = round, 2 = bevel
		_Pen_LineJoin
		^this.primitiveFailed
	}

	*capStyle_ { arg style = 0; // 0 = butt, 1 = round, 2 = square
		_Pen_LineCap
		^this.primitiveFailed
	}

	*lineDash_ { arg pattern; // should be a FloatArray
		_Pen_LineDash
		^this.primitiveFailed
	}

	*addOval { arg rect;
		_Pen_AddOval
		^this.primitiveFailed
	}

	*fillAxialGradient { arg startPoint, endPoint, color0, color1;
		this.prFillAxialGradient(startPoint, endPoint, color0, color1);
	}

	*fillRadialGradient { arg innerCircleCenter, outerCircleCenter, startRadius,
			endRadius, color0, color1;
		this.prFillRadialGradient(innerCircleCenter, outerCircleCenter, startRadius,
			endRadius, color0, color1)
	}

	*prFillAxialGradient { arg startPoint, endPoint, color0, color1;
		_Pen_DrawAxialGradient
		^this.primitiveFailed
	}

	*prFillRadialGradient { arg innerCircleCenter, outerCircleCenter, startRadius,
			endRadius, color0, color1;
		_Pen_DrawRadialGradient
		^this.primitiveFailed
	}

	// PRIVATE:
	*push {
		_Pen_Push
		^this.primitiveFailed
	}

	*pop {
		_Pen_Pop
		^this.primitiveFailed
	}
}

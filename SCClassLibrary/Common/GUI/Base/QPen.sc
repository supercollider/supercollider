Pen {
	classvar <font;
	//-------------------------- Global state control ------------------------------

	*push {
		_QPen_Save
		^this.primitiveFailed
	}

	*pop {
		_QPen_Restore
		^this.primitiveFailed
	}

	*use { |aFunction|
		this.push;
		aFunction.value;
		this.pop
	}

	//-------------------------- Drawing state -------------------------------------

	*fillColor_ { |aColor|
		_QPen_FillColor
		^this.primitiveFailed
	}

	*strokeColor_ { |aColor|
		_QPen_StrokeColor
		^this.primitiveFailed
	}

	*color_ { |aColor|
		this.fillColor_(aColor);
		this.strokeColor_(aColor)
	}

	*width_ { |width|
		_QPen_Width
		^this.primitiveFailed
	}

	*joinStyle_ { |style|
		_QPen_SetJoinStyle
		^this.primitiveFailed
	}

	*capStyle_ { |style|
		_QPen_SetCapStyle
		^this.primitiveFailed
	}

	*lineDash_ { |pattern|
		_QPen_SetDashPattern
		^this.primitiveFailed
	}

	*alpha_ { |value|
		_QPen_SetOpacity
		^this.primitiveFailed
	}

	*blendMode_ { |mode|
		this.nonimpl("blendMode_")
	}

	*beginTransparencyLayer {
		this.nonimpl("beginTransparencyLayer")
	}

	*endTransparencyLayer {
		this.nonimpl("endTransparencyLayer")
	}

	*clip {
		_QPen_Clip
		^this.primitiveFailed
	}

	*smoothing_ { |flag|
		_QPen_AntiAliasing
		^this.primitiveFailed
	}

	*font_ { |aFont|
		font = aFont;
		this.prSetFont(aFont)
	}

	*prSetFont { |aFont|
		_QPen_SetFont
		^this.primitiveFailed
	}

	//--------------------- Coordinate System Transform ----------------------------

	*translate { |x = 0, y = 0|
		_QPen_Translate
		^this.primitiveFailed
	}

	*scale { |x = 0, y = 0|
		_QPen_Scale
		^this.primitiveFailed
	}

	*skew { |x = 0, y = 0|
		_QPen_Shear
		^this.primitiveFailed
	}

	*rotate { |angle = 0, x = 0, y = 0|
		this.rotateDeg(angle.raddeg, x, y)
	}

	*rotateDeg { |angle = 0, x = 0, y = 0|
		_QPen_Rotate
		^this.primitiveFailed
	}

	*matrix {
		_QPen_Transform
		^this.primitiveFailed
	}

	*matrix_ { |matrixArray|
		_QPen_SetTransform
		^this.primitiveFailed
	}

	//-------------------------- Drawing Actions -----------------------------------

	*beginPath {
		_QPen_Clear;
		^this.primitiveFailed
	}

	*path {
		this.nonimpl("path")
	}

	*moveTo { |point|
		_QPen_MoveTo
		^this.primitiveFailed
	}

	*lineTo { |point|
		_QPen_LineTo
		^this.primitiveFailed
	}

	*line { |p1, p2|
		this.moveTo(p1);
		this.lineTo(p2)
	}

	*curveTo { |endPoint, cPoint1, cPoint2|
		_QPen_CubicTo
		^this.primitiveFailed
	}

	*quadCurveTo { |endPoint, cPoint|
		_QPen_QuadTo
		^this.primitiveFailed
	}

	*arcTo { |point1, point2, radius|
		_QPen_ArcTo
		^this.primitiveFailed
	}

	*addRect { |aRect|
		_QPen_AddRect
		^this.primitiveFailed
	}

	*addRoundedRect { |aRect, radiusX, radiusY|
		_QPen_AddRoundedRect
		^this.primitiveFailed
	}

	*addOval { |rect|
		_QPen_AddEllipse
		^this.primitiveFailed
	}

	*addArc { |center, radius, startAngle, arcAngle|
		this.prAddArc(center, radius, startAngle.neg, arcAngle.neg)
	}

	*prAddArc { |center, radius, startAngle, arcAngle|
		_QPen_AddArc
		^this.primitiveFailed
	}

	*addWedge { |center, radius, startAngle, sweepLength|
		this.addWedge_Deg(center, radius, (startAngle.neg).raddeg, (sweepLength.neg).raddeg)
	}

	*addWedge_Deg { |center, radius, startAngle, sweepLength|
		_QPen_AddWedge
		^this.primitiveFailed
	}

	*addAnnularWedge{ |center, innerRadius, outerRadius, startAngle, sweepLength|
		this.prAddAnnularWedge(center, innerRadius, outerRadius, startAngle.neg, sweepLength.neg)
	}

	*prAddAnnularWedge { |center, innerRadius, outerRadius, startAngle, sweepLength|
		_QPen_AddAnnularWedge
		^this.primitiveFailed
	}

	*fill {
		this.draw(0)
	}

	*stroke {
		this.draw(2)
	}

	*draw { |style|
		_QPen_Draw
		^this.primitiveFailed
	}

	*fillStroke {
		this.draw(3)
	}

	*strokeRect { |rect|
		this.addRect(rect);
		this.stroke
	}

	*fillRect { |rect|
		this.addRect(rect);
		this.fill
	}

	*strokeOval { |rect|
		this.addOval(rect);
		this.stroke
	}

	*fillOval { |rect|
		this.addOval(rect);
		this.fill
	}

	*fillAxialGradient { |startPoint, endPoint, startColor, endColor|
		_QPen_FillAxialGradient;
		^this.primitiveFailed
	}

	*fillRadialGradient { |innerCircleCenter, outerCircleCenter, startRadius, endRadius, startColor, endColor|
		_QPen_FillRadialGradient;
		^this.primitiveFailed
	}

	*setShadow { |offsetPoint, blur, color|
		this.nonimpl("setShadow")
	}

	*string { |aString|
		this.stringAtPoint(aString, Point(0, 0))
	}

	*stringAtPoint { |string, point, font, color|
		_QPen_StringAtPoint
		^this.primitiveFailed
	}

	*stringInRect { |string, rect, font, color, alignment|
		_QPen_StringInRect
		^this.primitiveFailed
	}

	*stringCenteredIn { |string, rect, font, color|
		this.stringInRect(string, rect, font, color, QAlignment(\center))
	}

	*stringLeftJustIn { |string, rect, font, color|
		this.stringInRect(string, rect, font, color, QAlignment(\left))
	}

	*stringRightJustIn { |string, rect, font, color|
		this.stringInRect(string, rect, font, color, QAlignment(\right))
	}

	*drawImage { |target, image, source, operation = 'sourceOver', opacity = 1.0|
		operation = Image.compositingOperations.indexOf(operation) ? 0;
		this.prDrawImage(target, image, source, operation, opacity)
	}

	*tileImage { |target, image, source, operation = 'sourceOver', opacity = 1.0|
		operation = Image.compositingOperations.indexOf(operation) ? 0;
		this.prTileImage(target, image, source, operation, opacity)
	}

	//---------------------- PRIVATE! -- Painter on/off-----------------------------

	*prDrawImage { |target, image, source, operation, opacity|
		_QPen_DrawImage
		^this.primitiveFailed
	}

	*prTileImage { |target, image, source, operation, opacity|
		_QPen_TileImage
		^this.primitiveFailed
	}

	*nonimpl { |methodName|
		(this.asString ++ ": " ++ methodName.asString ++ " is not implemented yet").postln
	}
}

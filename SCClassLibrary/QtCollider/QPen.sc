QPen {
  classvar <font;
  *implementsClass {^'Pen'}
//-------------------------- Global state control ------------------------------

  *push {
    _QPen_Save
    ^this.primitiveFailed;
  }

  *pop {
    _QPen_Restore
    ^this.primitiveFailed;
  }

  *use { arg aFunction;
    this.push;
    aFunction.value;
    this.pop;
  }

//-------------------------- Drawing state -------------------------------------

  *fillColor_ { arg aColor;
    _QPen_FillColor
    ^this.primitiveFailed;
  }

  *strokeColor_ { arg aColor;
    _QPen_StrokeColor
    ^this.primitiveFailed;
  }

  *color_ { arg aColor;
    this.fillColor_( aColor );
    this.strokeColor_( aColor );
  }

  *width_ { arg width;
    _QPen_Width
    ^this.primitiveFailed;
  }

  *joinStyle_ { arg style;
    _QPen_SetJoinStyle
    ^this.primitiveFailed;
  }

  *capStyle_ { arg style;
    _QPen_SetCapStyle
    ^this.primitiveFailed;
  }

  *lineDash_ { arg pattern;
    _QPen_SetDashPattern
    ^this.primitiveFailed;
  }

  *alpha_ { arg value;
    _QPen_SetOpacity
    ^this.primitiveFailed;
  }

  *blendMode_ { arg mode;
    this.nonimpl("blendMode_");
  }

  *beginTransparencyLayer {
    this.nonimpl("beginTransparencyLayer");
  }

  *endTransparencyLayer {
    this.nonimpl("endTransparencyLayer");
  }

  *clip {
    _QPen_Clip
    ^this.primitiveFailed;
  }

  *smoothing_ { arg flag;
    _QPen_AntiAliasing
    ^this.primitiveFailed;
  }

  *font_ { arg aFont;
    font = aFont;
    this.prSetFont( aFont );
  }

  *prSetFont { arg aFont;
    _QPen_SetFont
    ^this.primitiveFailed;
  }

//--------------------- Coordinate System Transform ----------------------------

  *translate { arg x=0, y=0;
    _QPen_Translate
    ^this.primitiveFailed;
  }

  *scale { arg x=0, y=0;
    _QPen_Scale
    ^this.primitiveFailed;
  }

  *skew { arg x=0, y=0;
    _QPen_Shear
    ^this.primitiveFailed;
  }

  *rotate { arg angle=0, x=0, y=0;
    this.rotateDeg( angle.raddeg, x, y );
  }

  *rotateDeg { arg angle=0, x=0, y=0;
    _QPen_Rotate
    ^this.primitiveFailed;
  }

  *matrix { _QPen_Transform }

  *matrix_ { arg matrixArray;
    _QPen_SetTransform
    ^this.primitiveFailed;
  }

//-------------------------- Drawing Actions -----------------------------------

  *beginPath {
    _QPen_Clear;
    ^this.primitiveFailed;
  }

  *path {
    this.nonimpl("path");
  }

  *moveTo { arg point;
    _QPen_MoveTo
    ^this.primitiveFailed;
  }

  *lineTo { arg point;
    _QPen_LineTo
    ^this.primitiveFailed;
  }

  *line { arg p1, p2;
    this.moveTo(p1);
    this.lineTo(p2);
  }

  *curveTo { arg endPoint, cPoint1, cPoint2;
    _QPen_CubicTo
    ^this.primitiveFailed;
  }

  *quadCurveTo { arg endPoint, cPoint;
    _QPen_CubicTo
    ^this.primitiveFailed;
  }

  *arcTo { arg point1, point2, radius;
    _QPen_ArcTo
    ^this.primitiveFailed;
  }

  *addRect { arg aRect;
    _QPen_AddRect
    ^this.primitiveFailed;
  }

  *addRoundedRect { arg aRect, radiusX, radiusY;
    _QPen_AddRoundedRect
    ^this.primitiveFailed;
  }

  *addOval { arg rect;
    _QPen_AddEllipse
    ^this.primitiveFailed;
  }

  *addArc { arg center, radius, startAngle, arcAngle;
    this.prAddArc( center, radius, startAngle.neg, arcAngle.neg );
  }

  *prAddArc { arg center, radius, startAngle, arcAngle;
    _QPen_AddArc
    ^this.primitiveFailed;
  }

  *addWedge { arg center, radius, startAngle, sweepLength;
    this.addWedge_Deg( center, radius, (startAngle.neg).raddeg, (sweepLength.neg).raddeg );
  }

  *addWedge_Deg { arg center, radius, startAngle, sweepLength;
    _QPen_AddWedge
    ^this.primitiveFailed;
  }

  *addAnnularWedge{ arg center, innerRadius, outerRadius, startAngle, sweepLength;
    this.prAddAnnularWedge( center, innerRadius, outerRadius, startAngle.neg, sweepLength.neg );
  }

  *prAddAnnularWedge { arg center, innerRadius, outerRadius, startAngle, sweepLength;
    _QPen_AddAnnularWedge
    ^this.primitiveFailed;
  }

  *fill {
    this.draw( 0 );
  }

  *stroke {
    this.draw( 2 );
  }

  *draw { arg style;
    _QPen_Draw
    ^this.primitiveFailed;
  }

  *fillStroke {
    this.draw(3);
  }

  *strokeRect { arg rect;
    this.addRect( rect );
    this.stroke;
  }

  *fillRect { arg rect;
    this.addRect( rect );
    this.fill;
  }

  *strokeOval { arg rect;
    this.addOval(rect);
    this.stroke;
  }

  *fillOval { arg rect;
    this.addOval(rect);
    this.fill;
  }

  *fillAxialGradient { arg startPoint, endPoint, startColor, endColor;
    _QPen_FillAxialGradient;
    ^this.primitiveFailed;
  }

  *fillRadialGradient { arg innerCircleCenter, outerCircleCenter, startRadius, endRadius, startColor, endColor;
    _QPen_FillRadialGradient;
    ^this.primitiveFailed;
  }

  *setShadow { arg offsetPoint, blur, color;
    this.nonimpl("setShadow");
  }

  *string { arg aString;
    this.stringAtPoint( aString, Point(0,0) );
  }

  *stringAtPoint { arg string, point, font, color;
    _QPen_StringAtPoint
    ^this.primitiveFailed;
  }

  *stringInRect { arg string, rect, font, color, alignment;
    _QPen_StringInRect
    ^this.primitiveFailed;
  }

  *stringCenteredIn { arg string, rect, font, color;
    this.stringInRect( string, rect, font, color, QAlignment(\center) );
  }

  *stringLeftJustIn { arg string, rect, font, color;
    this.stringInRect( string, rect, font, color, QAlignment(\left) );
  }

  *stringRightJustIn { arg string, rect, font, color;
    this.stringInRect( string, rect, font, color, QAlignment(\right) );
  }

//---------------------- PRIVATE! -- Painter on/off-----------------------------

  *prBegin { arg aQView;
    _QPen_Begin
    ^this.primitiveFailed;
  }

  *prEnd {
    _QPen_End
    ^this.primitiveFailed;
  }

  *nonimpl { arg methodName;
    (this.asString ++ ": " ++ methodName.asString ++ " is not implemented yet").postln;
  }
}

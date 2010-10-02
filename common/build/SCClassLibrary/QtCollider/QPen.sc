QPen {
  classvar <font;

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
    this.nonimpl("joinStyle_");
  }

  *capStyle_ { arg style;
    this.nonimpl("capStyle_");
  }

  *lineDash_ { arg pattern;
    this.nonimpl("lineDash_");
  }

  *alpha_ {
    this.nonimpl("alpha_");
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

  *matrix_ { arg matrixArray;
    _QPen_Transform
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

  *moveTo {
    _QPen_MoveTo
    ^this.primitiveFailed;
  }

  *lineTo {
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
    this.nonimpl("arcTo");
  }

  *addArc { arg center, radius, startAngle, arcAngle;
    this.nonimpl("addArc");
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

  *addWedge { arg center, radius, startAngle, sweepLength;
    this.addWedge_Deg( center, radius, (startAngle.neg).raddeg, (sweepLength.neg).raddeg );
  }

  *addWedge_Deg { arg center, radius, startAngle, sweepLength;
    _QPen_AddWedge
    ^this.primitiveFailed;
  }

  *addAnnularWedge{ arg center, innerRadius, outerRadius, startAngle, sweepLength;
    this.addAnnularWedge_Deg( center, innerRadius, outerRadius, (startAngle.neg).raddeg, (sweepLength.neg).raddeg );
  }

  *addAnnularWedge_Deg { arg center, innerRadius, outerRadius, startAngle, sweepLength;
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
    this.nonimpl( "fillAxialGradient" );
  }

  *fillRadialGradient { arg innerCircleCenter, outerCircleCenter, startRadius, endRadius, startColor, endColor;
    this.nonimpl( "fillRadialGradient" );
  }

  *setShadow { arg offsetPoint, blur, color;
    this.nonimpl("setShadow");
  }

  *string { arg aString;
    this.stringAtPoint( aString, Point(0,0) );
  }

  *stringAtPoint { arg aString, aPoint;
    _QPen_StringAtPoint
    ^this.primitiveFailed;
  }

  *stringInRect { arg aString, aRect;
    _QPen_StringInRect
    ^this.primitiveFailed;
  }

  *stringCenteredIn { arg aString, aRect;
    this.stringAtPoint( aString,
                        QtGUI.stringBounds( aString, this.font )
                             .centerIn( aRect ) );
  }

  *stringLeftJustIn { arg aString, aRect;
    var pos, bounds;
    bounds = QtGUI.stringBounds( aString, this.font );
    pos = bounds.centerIn( aRect );
    pos.x = aRect.left;
    this.stringAtPoint( aString, pos );
  }

  *stringRightJustIn { arg aString, aRect;
    var pos, bounds;
    bounds = QtGUI.stringBounds( aString, this.font );
    pos = bounds.centerIn( aRect );
    pos.x = aRect.right - bounds.width;
    this.stringAtPoint( aString, pos );
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

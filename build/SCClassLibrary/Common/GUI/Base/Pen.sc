Pen {
	*use { arg function;
		var res;
		this.push;
		res = function.value;
		this.pop;
		^res
	}

	*translate { arg x=0, y=0;
		_Pen_Translate	
		^this.primitiveFailed
	}
	*scale { arg x=0, y=0;
		_Pen_Scale	
		^this.primitiveFailed
	}
	*rotate { arg angle=0, x=0, y=0;
		_Pen_Rotate	
		^this.primitiveFailed
	}

	
	*width_ { arg width=1;
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
	*endPath {
		_Pen_EndPath
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
	*curveTo { arg point, cpoint1, cpoint2;
		_Pen_CurveTo
		^this.primitiveFailed
	}
	*quadCurveTo { arg point, cpoint1;
		_Pen_QuadCurveTo
		^this.primitiveFailed
	}
	*addArc { arg center, startAngle, arcAngle;
		_Pen_AddArc
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
	
	*drawAquaButton { arg rect, type=0, down=false, on=false;
		_Pen_DrawAquaButton
		^this.primitiveFailed
	}

	//PRIVATE:
	*push {
		_Pen_Push
		^this.primitiveFailed
	}
	*pop {
		_Pen_Pop
		^this.primitiveFailed
	}
}


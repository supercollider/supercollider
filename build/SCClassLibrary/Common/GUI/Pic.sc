Pic {
	var dataptr, <>bounds, <>depth=0, finalizer;
	
	*new { arg bounds, depth=32;
		^super.new.bounds_(bounds).depth_(depth).prNew
	}
	*read { arg pathname, depth=32;
		^super.new.bounds_(Rect.new).depth_(depth).prRead(pathname)
	}
	close {
		finalizer.finalize;
	}
	draw {
		this.prDraw(bounds.moveTo(0,0), bounds);
	}
	getLayoutSize {
		var extent;
		extent = bounds.extent
		^LayoutSize(extent.x, extent.x, extent.y, extent.y, 0)
	}
	layout { arg argBounds;
		bounds = bounds.moveTo(argBounds.left, argBounds.top);
	}
	
	// PRIVATE
	prNew {
		_Pic_New
		^this.primitiveFailed
	}
	prRead { arg pathname;
		_Pic_Read
		^this.primitiveFailed
	}
	prDraw { arg dstBounds;
		_Pic_Draw
		^this.primitiveFailed
	}
}

ScaledPic {
	var <>pic,  <>bounds;
	*new { arg pic, bounds;
		^super.newCopyArgs(pic, bounds ? pic.bounds)
	}
	draw {
		pic.prDraw(bounds);
	}
	layout { arg argBounds;
		bounds = argBounds;
	}
}


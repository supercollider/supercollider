
FlowLayout {
	var <>bounds, <>margin, <>gap;
	var left, top, maxHeight; 
	
	*new { arg bounds, margin, gap;
		^super.newCopyArgs(bounds, margin, gap).init
	}
	init {
		gap = gap ? Point(4,4);
		margin = margin ? Point(4,4);
		this.reset;
	}
	reset {
		left = bounds.left + margin.x;
		top = bounds.top + margin.y;
		maxHeight = 0;
	}
	place { arg view;
		var height, width;
		width = view.bounds.width;
		height = view.bounds.height;
		if ((left + width) > (bounds.right - margin.x), { this.nextLine });
		
		view.bounds = Rect(left, top, width, height);
		
		left = left + width + gap.x;
		maxHeight = max(maxHeight, height);
	}
	nextLine {
		left = margin.x;
		top = top + maxHeight + gap.y;
		maxHeight = 0;
	}
	innerBounds {
		^bounds.insetBy(margin.x,margin.y)
	}
	used { //round up to nearest rect
		^Rect(bounds.left,bounds.top,bounds.width,(top + maxHeight) - bounds.top + margin.y)
	}
	remaining {// round down to nearest rect
		var t;
		^Rect(bounds.left,t = top + maxHeight,bounds.width,bounds.height - t)
	}
}


	

FlowLayout {
	var <bounds, <>margin, <>gap;
	var <left, <top, <maxHeight,<maxRight; 
	
	*new { arg bounds, margin, gap;
		^super.newCopyArgs(bounds, margin, gap).init
	}
	init {
		gap = gap ? Point(4,4);
		margin = margin ? Point(4,4);
		this.reset;
	}
	reset {
		maxRight = left = bounds.left + margin.x;
		top = bounds.top + margin.y;
//"reset FlowLayout: %, %\n".postf(left, top);
		maxHeight  = 0;
	}
	place { arg view;
		var height, width,vbounds;
		vbounds = view.bounds;
		width = vbounds.width;
		height = vbounds.height;
//[left, width, bounds.right-margin.x].debug(">> FlowLayout:place [left, width, bounds.right-margin.x]");
		if ((left + width) > (bounds.right - margin.x), { this.nextLine });
		
//"placing view % [".postf(view.class.name); (view.asView !? { view.asView.instVarAt(0) }).post;
//"] at (%, %)\n".postf(left, top);
		view.bounds = Rect(left, top, width, height);

		maxRight = max(maxRight,left + width);		
		left = left + width + gap.x;
		maxHeight = max(maxHeight, height);
//[left, maxRight, maxHeight].debug("<< FlowLayout:place [left, maxRight, maxHeight]")
	}
	nextLine {
		left = bounds.left + margin.x;
		top = top + maxHeight + gap.y;
		maxHeight = 0;
//[left, top, maxHeight].debug("FlowLayout:nextLine [left, top, maxHeight]");
	}
	shift { arg x=0, y=0;
		left = left + x;
		top = top + y;
	}
	innerBounds {
		^bounds.insetBy(margin.x * 2,margin.y * 2)
	}
	bounds_ { arg b;
		var d;
//b.debug("\nFlowLayout:bounds_");
//this.dumpBackTrace;
		left = left + ( d = (b.left - bounds.left));
		maxRight = maxRight + (d);
		top = top + (d = (b.top - bounds.top));
		maxHeight = maxHeight + (d);
		bounds = b;
		// and then you need to re-place all views
		// but nextLine will be broken, see FlowView
	}
	currentBounds {
		var currentBounds;
		currentBounds = bounds;
		currentBounds.height = top + maxHeight;
		^currentBounds
	}
}


	
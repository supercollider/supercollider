
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
}


	
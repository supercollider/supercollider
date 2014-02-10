Decorator {
	place { arg view; this.subclassResponsibility(\place); }

	remove { arg view; this.subclassResponsibility(\remove); }

	clear { this.subclassResponsibility(\clear); }

	bounds { this.subclassResponsibility(\bounds); }

	bounds_ { arg bounds; this.subclassResponsibility(\bounds_); }

	gap { this.subclassResponsibility(\gap); }
	gap_ { arg gap; this.subclassResponsibility(\gap_); }

	margin { this.subclassResponsibility(\margin); }
	margin_ { arg margin; this.subclassResponsibility(\margin_); }
}

FlowLayout : Decorator {
	var <bounds, <>margin, <>gap;
	var <>left, <>top, <>maxHeight,<>maxRight;
	var <>owner;

	*new { arg bounds, margin, gap;
		^super.newCopyArgs(bounds, margin, gap).init
	}
	init {
		gap = gap ? Point(4,4);
		margin = margin ? Point(4,4);
		this.reset;
	}
	clear { this.reset; }
	reset {
		maxRight = left = bounds.left + margin.x;
		top = bounds.top + margin.y;
		maxHeight  = 0;
	}
	place { arg view;
		var height, width,vbounds;
		vbounds = view.bounds;
		width = vbounds.width;
		height = vbounds.height;
		if ((left + width) > (bounds.right - margin.x), { this.nextLine; });

		view.bounds = Rect(left, top, width, height);

		maxRight = max(maxRight,left + width);
		left = left + width + gap.x;
		maxHeight = max(maxHeight, height);
	}
	remove { }
	nextLine {
		left = bounds.left + margin.x;
		top = top + maxHeight + gap.y;
		maxHeight = 0;
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
	// rounded out to the nearest rect + margin
	used {
		^Rect(bounds.left,bounds.top,
			maxRight + margin.x - bounds.left,
			(top + maxHeight + margin.y ) - bounds.top)
	}
	// largest allocatable rect starting in the current row
	// going down as far as possible
	indentedRemaining {
		var inb;
		inb = this.innerBounds;
		^Rect(left,top,
			inb.width - (left - inb.left - margin.x),
			inb.height - (top - inb.top - margin.y))
	}
}

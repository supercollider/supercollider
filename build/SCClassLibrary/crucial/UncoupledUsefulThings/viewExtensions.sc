

+ SCContainerView {

	flow { arg func,bounds;
		var f,comp;
		f = FlowView(this,bounds ?? { this.bounds });
		func.value(f);
		^f
	}

	horz { arg func,bounds;
		var comp;
		comp = SCHLayoutView(this,bounds ?? { this.bounds });
		func.value(comp);
		^comp
	}
}

+ FlowLayout {

	used { //round up to nearest rect
		^Rect(bounds.left,bounds.top,maxRight + margin.x - bounds.left,
					(top + maxHeight + margin.y) - bounds.top)
	}
	indentedRemaining {
		var inb;
		inb = this.innerBounds;
		^Rect(left,top,
			inb.width - (left - inb.left - margin.x),
			inb.height - (top - inb.top - margin.y))
	}
	
	wouldExceedBottom { arg aBounds;
		^top + aBounds.height > bounds.bottom
	}

}

+ FlowView {

	flow { arg func,bounds;
		var f;
		f = FlowView(this,bounds ?? 
					{ decorator.indentedRemaining });
		func.value(f);
		f.resizeToFit;
		this.reflowAll;
		^f
	}

	horz { arg func,bounds;
		var comp;
		comp = SCHLayoutView(this,bounds ?? 
					{ decorator.indentedRemaining });
		func.value(comp);
		//comp.resizeToFit;
		//this.reflowAll;
		^comp
	}
}

+ MultiPageLayout {

	flow { arg func,bounds;
		^this.view.flow(func,bounds)
	}

}

/*
	this is from sc3d5 os9 version.
	not currently working in sc3 os x
	
	Pen asPen etc. not functioning.
	this is a nice layout system, we should get it working again.
	-felix
*/
/*
Layout {
	var <>minWidth, <>maxWidth, <>minHeight, <>maxHeight, <>weight;
	
	*new { arg minWidth=0, maxWidth=10000, minHeight=0, maxHeight=10000, weight = 1.0;
		^super.newCopyArgs(minWidth, maxWidth, minHeight, maxHeight, weight);
	}	
}

LayoutSize {
	var <>minWidth, <>maxWidth, <>minHeight, <>maxHeight, <>weight;
	var <>width, <>height;
	
	*new { arg minWidth=0, maxWidth=10000, minHeight=0, maxHeight=10000, weight = 1.0;
		^super.newCopyArgs(minWidth, maxWidth, minHeight, maxHeight, weight);
	}	
}

LayoutView : View {
	*new { arg pen, items, enabled=true, visible=true, layoutSize;
		^super.newCopyArgs( pen.asPen, items ?? { Array.new(4) }, 
					enabled, visible, layoutSize ?? { LayoutSize.new(nil,nil,nil,nil) })
	}
}

HLayout : LayoutView {

	getLayoutSize {
		var layouts;
		layouts = this.getLayouts;
		^LayoutSize(
			layoutSize.minWidth  ?? { layouts.sum({ arg layout; layout.minWidth; }) },
			layoutSize.maxWidth  ?? { layouts.sum({ arg layout; layout.maxWidth; }) },
			layoutSize.minHeight ?? { layouts.maxItem({ arg layout; layout.minHeight; }).minHeight },
			layoutSize.maxHeight ?? { layouts.minItem({ arg layout; layout.maxHeight; }).maxHeight },
			layoutSize.weight
		)
	}
	layout { arg argBounds;
		var layouts, extent, totalWeight, totalWidth, scaleWeight, left, top;
		
		extent = argBounds.extent;
		layouts = this.getLayouts;
		totalWeight = this.getTotalWeight(layouts);
		totalWidth = extent.x;
		
		// find views who are constrained by a minimum or maximum size and remove them from
		// the set of weights.
		scaleWeight = totalWidth.max(0) * totalWeight.max(0.01).reciprocal;
		items.do({ arg item, i;
			var weightedWidth, layout, width;
			layout = layouts.at(i);
			weightedWidth = scaleWeight * layout.weight;
			if (weightedWidth < layout.minWidth, { width = layout.minWidth },{
				if (weightedWidth > layout.maxWidth, { width = layout.maxWidth });
			});
			if (width.notNil, {
				layout.width = width;
				totalWidth = totalWidth - width;
				totalWeight = totalWeight - layout.weight;
			});
		});
		
		left = argBounds.left;
		top = argBounds.top;
		
		// now layout the views
		scaleWeight = totalWidth * totalWeight.reciprocal;
		items.do({ arg item, i;
			var weightedWidth, layout, width;
			layout = layouts.at(i);
			width = layout.width ?? { scaleWeight * layout.weight };
			item.layout( Rect.newBy(left, top, width, extent.y) );
			left = left + width;
		});
	}
}
	
			
VLayout : LayoutView {

	getLayoutSize {
		var layouts;
		layouts = this.getLayouts;
		^LayoutSize(
			layoutSize.minWidth  ?? { layouts.maxItem({ arg layout; layout.minWidth; }).minWidth },
			layoutSize.maxWidth  ?? { layouts.minItem({ arg layout; layout.maxWidth; }).maxWidth },
			layoutSize.minHeight ?? { layouts.sum({ arg layout; layout.minHeight; }) },
			layoutSize.maxHeight ?? { layouts.sum({ arg layout; layout.maxHeight; }) },
			layoutSize.weight
		)
	}
	layout { arg argBounds;
		var layouts, extent, totalWeight, totalHeight, scaleWeight, left, top;
		
		extent = argBounds.extent;
		layouts = this.getLayouts;
		totalWeight = this.getTotalWeight(layouts);
		totalHeight = extent.y;
		
		// find views who are constrained by a minimum or maximum size and remove them from
		// the set of weights.
		scaleWeight = totalHeight.max(0) * totalWeight.max(0.01).reciprocal;
		items.do({ arg item, i;
			var weightedHeight, layout, height;
			layout = layouts.at(i);
			weightedHeight = scaleWeight * layout.weight;
			if (weightedHeight < layout.minHeight, { height = layout.minHeight },{
				if (weightedHeight > layout.maxHeight, { height = layout.maxHeight });
			});
			if (height.notNil, {
				layout.height = height;
				totalHeight = totalHeight - height;
				totalWeight = totalWeight - layout.weight;
			});
		});
		
		left = argBounds.left;
		top = argBounds.top;
		
		// now layout the views
		scaleWeight = totalHeight * totalWeight.reciprocal;
		items.do({ arg item, i;
			var weightedHeight, layout, height;
			layout = layouts.at(i);
			height = layout.height ?? { scaleWeight * layout.weight };
			item.layout( Rect.newBy(left, top, extent.x, height) );
			top = top + height;
		});
	}
}
	
Margin : View {
	var <>margin;
	
	*new { arg pen, item, enabled=false, visible=true, margin;
		^super.new(pen, [item], enabled, visible).margin_(margin)
	}
	getLayoutSize {
		var layout;
		pen.use({ layout = items.at(0).getLayoutSize; });
		^LayoutSize(
			layout.minWidth + margin.left + margin.right,
			layout.maxWidth + margin.left + margin.right,
			layout.minHeight + margin.top + margin.bottom,
			layout.maxHeight + margin.top + margin.bottom,
			layoutSize.weight
		)
	}
	layout { arg argBounds;
		argBounds.left = argBounds.left + margin.left;
		argBounds.right = argBounds.right - margin.right;
		argBounds.top = argBounds.top + margin.top;
		argBounds.bottom = argBounds.bottom - margin.bottom;
		items.at(0).layout(argBounds);
	}
}

DividerBar {
	var <>pen, <>bounds, <>parent;
	
	*new { arg pen, bounds;
		^super.newCopyArgs(pen.asPen)
	}
	layout { arg argBounds; bounds = argBounds; }
	mouseDown { arg where, event;
		if (bounds.containsPoint(where), { ^this })
		^nil
	}
	mouseUp { arg where, event;
		this.mouseMoved(where, event)
	}
	draw {
		pen.use({
			Pen.foreColor = rgb(100,100,100);
			bounds.insetBy(1,1).draw;
		})
	}
}

HDividerBar : DividerBar {
	getLayoutSize { ^LayoutSize(8,8,0,10000,0) }
	mouseMoved { arg where, event;
		var proportion;
		proportion = ((where.x - parent.bounds.left) / parent.bounds.width).clip(0,1);
		parent.items.at(0).layoutSize.weight = proportion;
		parent.items.at(2).layoutSize.weight = 1 - proportion;
		parent.layout(parent.bounds);
	}
}

HDivider : HLayout {
	var <>bounds;
	*new { arg pen, viewA, viewB, enabled=true, visible=true, layoutSize, proportion = 0.5;
		var bar, instance;
		viewA = HLayout(nil,[viewA]);
		viewB = HLayout(nil,[viewB]);
		viewA.layoutSize.weight = proportion;
		viewB.layoutSize.weight = 1 - proportion;
		bar = HDividerBar.new;
		instance = super.new( pen.asPen, [viewA, bar, viewB], enabled, visible, layoutSize);
		bar.parent = instance;
		^instance
	}
	layout { arg argBounds;
		bounds = argBounds;
		^super.layout(argBounds)
	}
}


VDividerBar : DividerBar {

	getLayoutSize { ^LayoutSize(0,10000,8,8,0) }
	mouseMoved { arg where, event;
		var proportion;
		proportion = ((where.y - parent.bounds.top) / parent.bounds.height).clip(0,1);
		parent.items.at(0).layoutSize.weight = proportion;
		parent.items.at(2).layoutSize.weight = 1 - proportion;
		parent.layout(parent.bounds);
	}
}

VDivider : VLayout {
	var <>bounds;
	*new { arg pen, viewA, viewB, enabled=true, visible=true, layoutSize, proportion = 0.5;
		var bar, instance;
		viewA = VLayout(nil,[viewA]);
		viewB = VLayout(nil,[viewB]);
		viewA.layoutSize.weight = proportion;
		viewB.layoutSize.weight = 1 - proportion;
		bar = VDividerBar.new;
		instance = super.new( pen.asPen, [viewA, bar, viewB], enabled, visible, layoutSize);
		bar.parent = instance;
		^instance
	}
	layout { arg argBounds;
		bounds = argBounds;
		^super.layout(argBounds)
	}
}


*/


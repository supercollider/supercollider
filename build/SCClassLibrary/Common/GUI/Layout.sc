{\rtf1\mac\ansicpg10000\cocoartf100
{\fonttbl\f0\fnil\fcharset77 Monaco;}
{\colortbl;\red255\green255\blue255;\red0\green0\blue191;\red191\green0\blue0;}
\pard\tx560\tx1120\tx1680\tx2240\tx2800\tx3360\tx3920\tx4480\tx5040\tx5600\tx6160\tx6720\ql\qnatural

\f0\fs18 \cf2 Layout\cf0  \{\
	\cf2 var\cf0  <>minWidth, <>maxWidth, <>minHeight, <>maxHeight, <>weight;\
	\
	*new \{ \cf2 arg\cf0  minWidth=0, maxWidth=10000, minHeight=0, maxHeight=10000, weight = 1.0;\
		^\cf2 super\cf0 .newCopyArgs(minWidth, maxWidth, minHeight, maxHeight, weight);\
	\}	\
\}\
\
\cf2 LayoutSize\cf0  \{\
	\cf2 var\cf0  <>minWidth, <>maxWidth, <>minHeight, <>maxHeight, <>weight;\
	\cf2 var\cf0  <>width, <>height;\
	\
	*new \{ \cf2 arg\cf0  minWidth=0, maxWidth=10000, minHeight=0, maxHeight=10000, weight = 1.0;\
		^\cf2 super\cf0 .newCopyArgs(minWidth, maxWidth, minHeight, maxHeight, weight);\
	\}	\
\}\
\
\cf2 LayoutView\cf0  : \cf2 View\cf0  \{\
	*new \{ \cf2 arg\cf0  pen, items, enabled=\cf2 true\cf0 , visible=\cf2 true\cf0 , layoutSize;\
		^\cf2 super\cf0 .newCopyArgs( pen.asPen, items ?? \{ \cf2 Array\cf0 .new(4) \}, \
					enabled, visible, layoutSize ?? \{ \cf2 LayoutSize\cf0 .new(\cf2 nil\cf0 ,\cf2 nil\cf0 ,\cf2 nil\cf0 ,\cf2 nil\cf0 ) \})\
	\}\
\}\
\
\cf2 HLayout\cf0  : \cf2 LayoutView\cf0  \{\
\
	getLayoutSize \{\
		\cf2 var\cf0  layouts;\
		layouts = \cf2 this\cf0 .getLayouts;\
		^\cf2 LayoutSize\cf0 (\
			layoutSize.minWidth  ?? \{ layouts.sum(\{ \cf2 arg\cf0  layout; layout.minWidth; \}) \},\
			layoutSize.maxWidth  ?? \{ layouts.sum(\{ \cf2 arg\cf0  layout; layout.maxWidth; \}) \},\
			layoutSize.minHeight ?? \{ layouts.maxItem(\{ \cf2 arg\cf0  layout; layout.minHeight; \}).minHeight \},\
			layoutSize.maxHeight ?? \{ layouts.minItem(\{ \cf2 arg\cf0  layout; layout.maxHeight; \}).maxHeight \},\
			layoutSize.weight\
		)\
	\}\
	layout \{ \cf2 arg\cf0  argBounds;\
		\cf2 var\cf0  layouts, extent, totalWeight, totalWidth, scaleWeight, left, top;\
		\
		extent = argBounds.extent;\
		layouts = \cf2 this\cf0 .getLayouts;\
		totalWeight = \cf2 this\cf0 .getTotalWeight(layouts);\
		totalWidth = extent.x;\
		\
		\cf3 // find views who are constrained by a minimum or maximum size and remove them from\
\cf0 		\cf3 // the set of weights.\
\cf0 		scaleWeight = totalWidth.max(0) * totalWeight.max(0.01).reciprocal;\
		items.do(\{ \cf2 arg\cf0  item, i;\
			\cf2 var\cf0  weightedWidth, layout, width;\
			layout = layouts.at(i);\
			weightedWidth = scaleWeight * layout.weight;\
			if (weightedWidth < layout.minWidth, \{ width = layout.minWidth \},\{\
				if (weightedWidth > layout.maxWidth, \{ width = layout.maxWidth \});\
			\});\
			if (width.notNil, \{\
				layout.width = width;\
				totalWidth = totalWidth - width;\
				totalWeight = totalWeight - layout.weight;\
			\});\
		\});\
		\
		left = argBounds.left;\
		top = argBounds.top;\
		\
		\cf3 // now layout the views\
\cf0 		scaleWeight = totalWidth * totalWeight.reciprocal;\
		items.do(\{ \cf2 arg\cf0  item, i;\
			\cf2 var\cf0  weightedWidth, layout, width;\
			layout = layouts.at(i);\
			width = layout.width ?? \{ scaleWeight * layout.weight \};\
			item.layout( \cf2 Rect\cf0 .newBy(left, top, width, extent.y) );\
			left = left + width;\
		\});\
	\}\
\}\
	\
			\
\cf2 VLayout\cf0  : \cf2 LayoutView\cf0  \{\
\
	getLayoutSize \{\
		\cf2 var\cf0  layouts;\
		layouts = \cf2 this\cf0 .getLayouts;\
		^\cf2 LayoutSize\cf0 (\
			layoutSize.minWidth  ?? \{ layouts.maxItem(\{ \cf2 arg\cf0  layout; layout.minWidth; \}).minWidth \},\
			layoutSize.maxWidth  ?? \{ layouts.minItem(\{ \cf2 arg\cf0  layout; layout.maxWidth; \}).maxWidth \},\
			layoutSize.minHeight ?? \{ layouts.sum(\{ \cf2 arg\cf0  layout; layout.minHeight; \}) \},\
			layoutSize.maxHeight ?? \{ layouts.sum(\{ \cf2 arg\cf0  layout; layout.maxHeight; \}) \},\
			layoutSize.weight\
		)\
	\}\
	layout \{ \cf2 arg\cf0  argBounds;\
		\cf2 var\cf0  layouts, extent, totalWeight, totalHeight, scaleWeight, left, top;\
		\
		extent = argBounds.extent;\
		layouts = \cf2 this\cf0 .getLayouts;\
		totalWeight = \cf2 this\cf0 .getTotalWeight(layouts);\
		totalHeight = extent.y;\
		\
		\cf3 // find views who are constrained by a minimum or maximum size and remove them from\
\cf0 		\cf3 // the set of weights.\
\cf0 		scaleWeight = totalHeight.max(0) * totalWeight.max(0.01).reciprocal;\
		items.do(\{ \cf2 arg\cf0  item, i;\
			\cf2 var\cf0  weightedHeight, layout, height;\
			layout = layouts.at(i);\
			weightedHeight = scaleWeight * layout.weight;\
			if (weightedHeight < layout.minHeight, \{ height = layout.minHeight \},\{\
				if (weightedHeight > layout.maxHeight, \{ height = layout.maxHeight \});\
			\});\
			if (height.notNil, \{\
				layout.height = height;\
				totalHeight = totalHeight - height;\
				totalWeight = totalWeight - layout.weight;\
			\});\
		\});\
		\
		left = argBounds.left;\
		top = argBounds.top;\
		\
		\cf3 // now layout the views\
\cf0 		scaleWeight = totalHeight * totalWeight.reciprocal;\
		items.do(\{ \cf2 arg\cf0  item, i;\
			\cf2 var\cf0  weightedHeight, layout, height;\
			layout = layouts.at(i);\
			height = layout.height ?? \{ scaleWeight * layout.weight \};\
			item.layout( \cf2 Rect\cf0 .newBy(left, top, extent.x, height) );\
			top = top + height;\
		\});\
	\}\
\}\
	\
\cf2 Margin\cf0  : \cf2 View\cf0  \{\
	\cf2 var\cf0  <>margin;\
	\
	*new \{ \cf2 arg\cf0  pen, item, enabled=\cf2 false\cf0 , visible=\cf2 true\cf0 , margin;\
		^\cf2 super\cf0 .new(pen, [item], enabled, visible).margin_(margin)\
	\}\
	getLayoutSize \{\
		\cf2 var\cf0  layout;\
		pen.use(\{ layout = items.at(0).getLayoutSize; \});\
		^\cf2 LayoutSize\cf0 (\
			layout.minWidth + margin.left + margin.right,\
			layout.maxWidth + margin.left + margin.right,\
			layout.minHeight + margin.top + margin.bottom,\
			layout.maxHeight + margin.top + margin.bottom,\
			layoutSize.weight\
		)\
	\}\
	layout \{ \cf2 arg\cf0  argBounds;\
		argBounds.left = argBounds.left + margin.left;\
		argBounds.right = argBounds.right - margin.right;\
		argBounds.top = argBounds.top + margin.top;\
		argBounds.bottom = argBounds.bottom - margin.bottom;\
		items.at(0).layout(argBounds);\
	\}\
\}\
\
\cf2 DividerBar\cf0  \{\
	\cf2 var\cf0  <>pen, <>bounds, <>parent;\
	\
	*new \{ \cf2 arg\cf0  pen, bounds;\
		^\cf2 super\cf0 .newCopyArgs(pen.asPen)\
	\}\
	layout \{ \cf2 arg\cf0  argBounds; bounds = argBounds; \}\
\cf2 	mouseDown \{ arg \cf0 where, event\cf2 ;\
		if (bounds.containsPoint(where), \{ ^\cf0 this \})\
		^nil\
	\}\
	mouseUp \{ arg where, event;\
		this.mouseMoved(where, event)\
	\}\
	draw \{\
		pen.use(\{\
			Pen.foreColor = rgb(100,100,100);\
			bounds.insetBy(1,1).draw;\
		\})\
	\}\
\}\
\
\cf2 HDividerBar\cf0  : \cf2 DividerBar \cf0 \{\
	getLayoutSize \{ ^\cf2 LayoutSize\cf0 (8,8,0,10000,0) \}\
	mouseMoved \{ arg where, event;\
		var proportion;\
		proportion = ((where.x - parent.bounds.left) / parent.bounds.width).clip(0,1);\
		parent.items.at(0).layoutSize.weight = proportion;\
		parent.items.at(2).layoutSize.weight = 1 - proportion;\
		parent.layout(parent.bounds);\
	\}\
\}\
\
\cf2 HDivider\cf0  : \cf2 HLayout\cf0  \{\
	var <>bounds;\
	*new \{ \cf2 arg\cf0  pen, viewA, viewB, enabled=\cf2 true\cf0 , visible=\cf2 true\cf0 , layoutSize, proportion = 0.5;\
		var bar, instance;\
		viewA = HLayout(nil,[viewA]);\
		viewB = HLayout(nil,[viewB]);\
		viewA.layoutSize.weight = proportion;\
		viewB.layoutSize.weight = 1 - proportion;\
		bar = \cf2 HDividerBar.new;\cf0 \
		instance = \cf2 super\cf0 .new( pen.asPen, [viewA, bar, viewB], enabled, visible, layoutSize);\
		bar.parent = instance;\
		^instance\
	\}\
	layout \{ arg argBounds;\
		bounds = argBounds;\
		^super.layout(argBounds)\
	\}\
\}\
\
\
\cf2 VDividerBar\cf0  : \cf2 DividerBar \cf0 \{\
\
	getLayoutSize \{ ^\cf2 LayoutSize\cf0 (0,10000,8,8,0) \}\
	mouseMoved \{ arg where, event;\
		var proportion;\
		proportion = ((where.y - parent.bounds.top) / parent.bounds.height).clip(0,1);\
		parent.items.at(0).layoutSize.weight = proportion;\
		parent.items.at(2).layoutSize.weight = 1 - proportion;\
		parent.layout(parent.bounds);\
	\}\
\}\
\
\cf2 VDivider\cf0  : \cf2 VLayout\cf0  \{\
	var <>bounds;\
	*new \{ \cf2 arg\cf0  pen, viewA, viewB, enabled=\cf2 true\cf0 , visible=\cf2 true\cf0 , layoutSize, proportion = 0.5;\
		var bar, instance;\
		viewA = VLayout(nil,[viewA]);\
		viewB = VLayout(nil,[viewB]);\
		viewA.layoutSize.weight = proportion;\
		viewB.layoutSize.weight = 1 - proportion;\
		bar = \cf2 VDividerBar.new;\cf0 \
		instance = \cf2 super\cf0 .new( pen.asPen, [viewA, bar, viewB], enabled, visible, layoutSize);\
		bar.parent = instance;\
		^instance\
	\}\
	layout \{ arg argBounds;\
		bounds = argBounds;\
		^super.layout(argBounds)\
	\}\
\}\
\
\
		}
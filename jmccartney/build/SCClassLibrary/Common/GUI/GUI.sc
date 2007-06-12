{\rtf1\mac\ansicpg10000\cocoartf100
{\fonttbl\f0\fswiss\fcharset77 Helvetica;\f1\fnil\fcharset77 Monaco;}
{\colortbl;\red255\green255\blue255;\red191\green0\blue0;\red0\green0\blue191;\red191\green0\blue0;
\red0\green115\blue0;}
\pard\tx560\tx1120\tx1680\tx2240\tx2800\tx3360\tx3920\tx4480\tx5040\tx5600\tx6160\tx6720\ql\qnatural

\f0\fs24 \cf2 //
\f1\fs18 \
//SCSlider \{\
//	var dataptr;	\
//	*new \{ arg pen, bounds, action;\
//		\
//	\}\
//	value \{\
//		_SCSlider_GetValue\
//		^this.primitiveFailed\
//	\}\
//	value_ \{ arg val;\
//		_SCSlider_SetValue\
//		^this.primitiveFailed\
//	\}\
//\}\
\pard\tx560\tx1120\tx1680\tx2240\tx2800\tx3360\tx3920\tx4480\tx5040\tx5600\tx6160\tx6720\ql\qnatural
\cf0 \
\pard\tx560\tx1120\tx1680\tx2240\tx2800\tx3360\tx3920\tx4480\tx5040\tx5600\tx6160\tx6720\ql\qnatural
\cf3 HasSubViews\cf0 \
\{\
	\cf3 var\cf0  <>pen, <>items, <>enabled, <>visible, <>layoutSize;\
	*new \{ \cf3 arg\cf0  pen, items, enabled=\cf3 true\cf0 , visible=\cf3 true\cf0 , layoutSize;\
		^\cf3 super\cf0 .newCopyArgs( pen.asPen, items ?? \{ \cf3 Array\cf0 .new(4) \}, \
					enabled, visible, layoutSize ?? \{ \cf3 LayoutSize\cf0 .new \})\
	\}\
\
	add \{ \cf3 arg\cf0  item;\
		items = items.add(item);\
	\}\
	remove \{ \cf3 arg\cf0  item;\
		items.remove(item);\
	\}\
	at \{ \cf3 arg\cf0  index;\
		^items.at(index)\
	\}\
	getLayouts \{\
		^pen.use(\{\
			items.collect(\{ \cf3 arg\cf0  item; item.getLayoutSize; \});\
		\});\
	\}\
	getTotalWeight \{ \cf3 arg\cf0  layouts;\
		^layouts.sum(\{ \cf3 arg\cf0  layout; layout.weight \});\
	\}\
\}\
\
\cf3 View\cf0  : \cf3 HasSubViews\cf0  \{	\
	*new \{ \cf3 arg\cf0  items, enabled=\cf3 true\cf0 , visible=\cf3 true\cf0 , layoutSize;\
		^\cf3 super\cf0 .new(items, enabled, visible, layoutSize);\
	\}\
	draw \{\
		if (visible, \{\
			items.draw;\
		\});\
	\}\
	mouseDown \{ \cf3 arg\cf0  where, event;\
		if (enabled, \{\
			items.reverseDo(\{ \cf3 arg\cf0  item; \
				\cf3 var\cf0  tracker;\
				tracker = item.mouseDown(where, event);\
				if (tracker.notNil, \{ ^tracker \});\
			\});\
		\});\
		^\cf3 nil\cf0 \
	\}\
	mouseOver \{ \cf3 arg\cf0  where, event;\
		\cf3 var\cf0  view;\
		if (enabled, \{\
			items.reverseDo(\{ \cf3 arg\cf0  item; \
				view = item.mouseOver(where, event);\
				if (view.notNil, \{ ^view \});\
			\});\
		\});\
		^\cf3 nil\cf0 \
	\}\
	layout \{ \cf3 arg\cf0  argBounds;\
		pen.offset = argBounds.origin;\
		\cf4 //items.do(\{ arg item; item.layout(argBounds) \});\
\pard\tx560\tx1120\tx1680\tx2240\tx2800\tx3360\tx3920\tx4480\tx5040\tx5600\tx6160\tx6720\ql\qnatural
\cf0 	\}\
\}\
\
\pard\tx560\tx1120\tx1680\tx2240\tx2800\tx3360\tx3920\tx4480\tx5040\tx5600\tx6160\tx6720\ql\qnatural
\cf3 LView\cf0  : \cf3 View\cf0  \{\
	\cf4 // same as View but lays out its superimposed subviews\
\pard\tx560\tx1120\tx1680\tx2240\tx2800\tx3360\tx3920\tx4480\tx5040\tx5600\tx6160\tx6720\ql\qnatural
\cf0 	layout \{ \cf3 arg\cf0  argBounds;\
		pen.offset = argBounds.origin;\
		items.do(\{ \cf3 arg\cf0  item; item.layout(argBounds) \});\
	\}\
\}\
\
\pard\tx560\tx1120\tx1680\tx2240\tx2800\tx3360\tx3920\tx4480\tx5040\tx5600\tx6160\tx6720\ql\qnatural
\cf3 SelectView\cf0  : \cf3 HasSubViews\cf0  \{\
	\cf3 var\cf0  <>which=0;\
	draw \{\
		if (visible, \{\
			pen.use(\{\
				items.at(which).draw;\
			\});\
		\});\
	\}\
	mouseDown \{ \cf3 arg\cf0  where, event;\
		^items.at(which).mouseDown(where, event)\
	\}\
	mouseOver \{ \cf3 arg\cf0  where, event;\
		^items.at(which).mouseOver(where, event)\
	\}\
	getLayoutSize \{\
		^items.at(which).getLayoutSize\
	\}\
	layout \{ \cf3 arg\cf0  argBounds;\
		^items.at(which).layout(argBounds)\
	\}\
\}\
\
\cf3 ControlView\cf0  \{\
	\cf3 var\cf0  <>pen, <>bounds, <>control, <>margin, <>layoutSize;\
	*new \{ \cf3 arg\cf0  pen, bounds, control, margin, layoutSize;\
		^\cf3 super\cf0 .newCopyArgs(pen.asPen, bounds.asRect, control, margin ?? \{\cf3 Rect\cf0 (2,2,2,2)\},\
			layoutSize ?? \{ \cf3 LayoutSize\cf0 .new \})\
	\}\
	mouseDown \{ \cf3 arg\cf0  where, event;\
		if (bounds.containsPoint(where), \{\
			\cf3 this\cf0 .respond(where, event);\
			^\cf3 this\cf0 ; \
		\},\{ \
			^\cf3 nil\cf0  \
		\});\
	\}\
	mouseMoved \{ \cf3 arg\cf0  where, event;\
		\cf3 this\cf0 .respond(where, event);\
	\}\
	mouseUp \{ \cf3 arg\cf0  where, event;\
		\cf3 this\cf0 .respond(where, event);\
	\}\
	layout \{ \cf3 arg\cf0  argBounds;\
		\cf3 this\cf0 .bounds = argBounds.insetByRect(margin);\
	\}\
	getLayoutSize \{ ^layoutSize \}\
\}\
\
\cf3 Button\cf0 \
\{\
	\cf3 var\cf0  <>bounds, <>states, <>margin, <>state=0, down=\cf3 false\cf0 ;\
	\
	*new \{ \cf3 arg\cf0  bounds, states, margin, state=0;\
		^\cf3 super\cf0 .newCopyArgs(bounds, states, margin ?? \{\cf3 Rect\cf0 (2,2,2,2)\}, state)\
	\}\
	draw \{\
		\cf3 var\cf0  label, labelColor, btnColor, action;\
		#label, labelColor, btnColor, action = states.at(state);\
		\cf3 this\cf0 .prDraw(bounds, btnColor, labelColor, label, down);\
	\}\
	mouseDown \{ \cf3 arg\cf0  where, event;\
		if (bounds.containsPoint(where), \{ \
			down = \cf3 true\cf0 ;\
			^\cf3 this\cf0 ; \
		\},\{ \
			^\cf3 nil\cf0  \
		\});\
	\}\
	mouseMoved \{ \cf3 arg\cf0  where, event;\
		down = bounds.containsPoint(where);\
	\}\
	mouseUp \{ \cf3 arg\cf0  where, event;\
		\cf3 var\cf0  label, labelColor, btnColor, action;\
		if (bounds.containsPoint(where), \{\
			#label, labelColor, btnColor, action = states.at(state);\
			state = action.value(state) ?? \{ (state + 1) % states.size \};\
		\});\
		down = \cf3 false\cf0 ;\
	\}\
	layout \{ \cf3 arg\cf0  argBounds;\
		\cf3 this\cf0 .bounds = argBounds.insetByRect(margin);\
	\}\
	prDraw \{ \cf3 arg\cf0  rect, btnColor, labelColor, label, down;\
		\cf3 _Button_Draw\cf0 \
		^\cf3 this\cf0 .primitiveFailed\
	\}\
\}\
\
\cf3 NumberView\cf0  : \cf3 ControlView\cf0  \{\
	\cf3 var\cf0  <>value;\
	*new \{ \cf3 arg\cf0  pen, bounds, control, margin, layoutSize;\
		^\cf3 super\cf0 .new(pen, bounds, control, margin, layoutSize)\
	\}\
	layout \{ \cf3 arg\cf0  argBounds;\
		\cf3 this\cf0 .bounds = argBounds;\
	\}\
	draw \{\
		value = control.getMapped(value);\
		pen.use(\{ value.asString.drawLeftJustIn(bounds) \});\
	\}\
	respond \{\}\
\}\
\
\
\cf3 Slider\cf0  : \cf3 ControlView\cf0 \
\{\
	\cf3 classvar\cf0  knobSize = 12;\
	\cf3 var\cf0  <innerBounds, <value=0.0;\
	\cf3 var\cf0  <background, <knob;\
	*new \{ \cf3 arg\cf0  pen, bounds, control, margin, layoutSize;\
		^\cf3 super\cf0 .new(pen, bounds, control, margin, layoutSize).init\
	\}\
	init \{\
		\cf3 this\cf0 .bounds = bounds;\
		knob = \cf3 BevelRect\cf0 .new(0,0,0,0, 2, \cf3 false\cf0 );\
	\}\
	bounds_ \{ \cf3 arg\cf0  argBounds;\
		bounds = argBounds;\
		background = \cf3 BevelRect\cf0 .fromRect(bounds, 1, \cf3 true\cf0 );\
		innerBounds = bounds.insetBy(1,1);\
		\cf3 this\cf0 .calcKnob;\
	\}\
	draw \{\
		\cf3 this\cf0 .calcKnob;\
		pen.use(\{\
			background.draw;\
			knob.draw;\
		\});\
	\}\
	isHorizontal \{\
		^bounds.width > bounds.height\
	\}\
	calcValue \{ \cf3 arg\cf0  where;\
		\cf3 var\cf0  halfKnob;\
		halfKnob = knobSize * 0.5;\
		if (\cf3 this\cf0 .isHorizontal, \{\
			value = (where.x - innerBounds.left - halfKnob)/(innerBounds.width - knobSize)\
		\},\{\
			value = 1.0 - ((where.y - innerBounds.top - halfKnob)/(innerBounds.height - knobSize))\
		\});\
		value = value.clip(0.0, 1.0);\
	\}\
	\
	calcKnob \{\
		\cf3 var\cf0  k;\
		value = control.get(value);\
		if (\cf3 this\cf0 .isHorizontal, \{\
			k = value * (innerBounds.width - knobSize) + innerBounds.left;\
			knob.set(k, innerBounds.top, k + knobSize, innerBounds.bottom);\
		\},\{\
			k = innerBounds.bottom - (value * (innerBounds.height - knobSize)) - knobSize;\
			knob.set(innerBounds.left, k, innerBounds.right, k + knobSize);\
		\});\
	\}\
	respond \{ \cf3 arg\cf0  where;\
		\cf3 this\cf0 .calcValue(where);\
		control.set(value);\
	\}\
	value_ \{ \cf3 arg\cf0  val;\
		value = val;\
		control.set(value);\
	\}\
\}\
\
\cf3 Slider2D\cf0  : \cf3 ControlView\cf0 \
\{\
	\cf3 classvar\cf0  knobSize = 12;\
	\cf3 var\cf0  <innerBounds, <x, <y;\
	\cf3 var\cf0  <background, <knob;\
	*new \{ \cf3 arg\cf0  pen, bounds, control, margin, layoutSize, x=0.5, y=0.5;\
		^\cf3 super\cf0 .new(pen, bounds, control, margin, layoutSize).x_(x).y_(y).init\
	\}\
	init \{\
		\cf3 this\cf0 .bounds = bounds;\
		knob = \cf3 BevelRect\cf0 .new(0,0,0,0, 2, \cf3 false\cf0 );\
	\}\
	bounds_ \{ \cf3 arg\cf0  argBounds;\
		bounds = argBounds;\
		background = \cf3 BevelRect\cf0 .fromRect(bounds, 1, \cf3 true\cf0 );\
		innerBounds = bounds.insetBy(1,1);\
		\cf3 this\cf0 .calcKnob;\
	\}\
	draw \{\
		\cf3 this\cf0 .calcKnob;\
		pen.use(\{\
			background.draw;\
			knob.draw;\
		\});\
	\}\
	isHorizontal \{\
		^bounds.width > bounds.height\
	\}\
	calcValue \{ \cf3 arg\cf0  where;\
		\cf3 var\cf0  halfKnob;\
		halfKnob = knobSize * 0.5;\
		x = (where.x - innerBounds.left - halfKnob)/(innerBounds.width - knobSize);\
		y = 1.0 - ((where.y - innerBounds.top - halfKnob)/(innerBounds.height - knobSize));\
		x = x.clip(0.0, 1.0);\
		y = y.clip(0.0, 1.0);\
	\}\
	calcKnob \{\
		\cf3 var\cf0  kx, ky;\
		x = control.get(x, 0);\
		y = control.get(y, 1);\
		kx = x * (innerBounds.width - knobSize) + innerBounds.left;\
		ky = innerBounds.bottom - (y * (innerBounds.height - knobSize)) - knobSize;\
		knob.set(kx, ky, kx + knobSize, ky + knobSize);\
	\}\
	respond \{ \cf3 arg\cf0  where;\
		\cf3 this\cf0 .calcValue(where);\
		control.set(x, y);\
	\}\
	x_ \{ \cf3 arg\cf0  val;\
		x = val;\
		control.set(x, y);\
	\}\
	y_ \{ \cf3 arg\cf0  val;\
		y = val;\
		control.set(x, y);\
	\}\
\}\
\
\cf3 RangeSlider\cf0  : \cf3 ControlView\cf0 \
\{\
	\cf3 var\cf0  <innerBounds, <lo, <hi, <>anchor;\
	\cf3 var\cf0  <background, <knob;\
	*new \{ \cf3 arg\cf0  pen, bounds, control, margin, layoutSize, lo=0.0, hi=0.0;\
		^\cf3 super\cf0 .new(pen, bounds, control, margin, layoutSize).lo_(lo).hi_(hi).init\
	\}\
	init \{\
		knob = \cf3 View\cf0 (\cf3 Pen\cf0 (foreColor: \cf3 Color\cf0 .grey), [ \cf3 Rect\cf0 .new ]);\
		\cf3 this\cf0 .bounds = bounds;\
	\}\
	bounds_ \{ \cf3 arg\cf0  argBounds;\
		bounds = argBounds;\
		background = \cf3 BevelRect\cf0 .fromRect(bounds, 2, \cf3 true\cf0 );\
		innerBounds = bounds.insetBy(2,2);\
		\cf3 this\cf0 .calcKnob;\
	\}\
	draw \{\
		pen.use(\{\
			\cf3 Pen\cf0 .action = \cf5 \\fill\cf0 ;\
			background.draw;\
			knob.draw;\
		\});\
	\}\
	isHorizontal \{\
		^bounds.width > bounds.height\
	\}\
	calcValue \{ \cf3 arg\cf0  where, event;\
		\cf3 var\cf0  val1, val2;\
		if (\cf3 this\cf0 .isHorizontal, \{\
			val1 = (event.where.x - innerBounds.left)/(innerBounds.width);\
			val2 = (where.x - innerBounds.left)/(innerBounds.width);\
		\},\{\
			val1 = 1.0 - ((event.where.y - innerBounds.top)/(innerBounds.height));\
			val2 = 1.0 - ((where.y - innerBounds.top)/(innerBounds.height));\
		\});\
		lo = min(val1, val2).clip(0.0, 1.0);\
		hi = max(val1, val2).clip(0.0, 1.0);\
		\cf3 this\cf0 .calcKnob;\
	\}\
	calcKnob \{\
		\cf3 var\cf0  klo, khi;\
		if (\cf3 this\cf0 .isHorizontal, \{\
			klo = lo * innerBounds.width + innerBounds.left;\
			khi = hi * innerBounds.width + innerBounds.left;\
			khi = min(max(klo+1, khi), innerBounds.right);\
			knob.items.put(0, \cf3 Rect\cf0 .new(klo, innerBounds.top, khi, innerBounds.bottom) );\
		\},\{\
			klo = innerBounds.bottom - (lo * innerBounds.height);\
			khi = innerBounds.bottom - (hi * innerBounds.height);\
			khi = min(klo-1, khi);\
			knob.items.put(0, \cf3 Rect\cf0 .new(innerBounds.left, khi, innerBounds.right, klo) );\
		\});\
	\}\
	respond \{ \cf3 arg\cf0  where, event;\
		\cf3 this\cf0 .calcValue(where, event);\
		control.set(lo, hi);\
	\}\
	hi_ \{ \cf3 arg\cf0  val;\
		hi = val;\
		control.set(lo, hi);\
	\}\
	lo_ \{ \cf3 arg\cf0  val;\
		lo = val;\
		control.set(lo, hi);\
	\}\
\}\
\
\cf3 Knob\cf0  : \cf3 ControlView\cf0 \
\{\
	\cf3 var\cf0  innerBounds, radius, center, innerEdge, outerEdge;\
	\cf3 var\cf0  <value = 0.0;\
	\cf3 var\cf0  <background, <knob;\
	*new \{ \cf3 arg\cf0  pen, bounds, control, margin, layoutSize, value=0.0;\
		pen = pen ?? \{ \cf3 Pen\cf0 (action: \cf5 \\fill\cf0 , foreColor: rgb(220,220,220)) \};\
		^\cf3 super\cf0 .new(pen, bounds, control, margin, layoutSize).init\
	\}\
	init \{\
		\cf3 this\cf0 .bounds = bounds;\
	\}\
	bounds_ \{ \cf3 arg\cf0  argBounds;\
		bounds = argBounds.centerSquare;\
		innerBounds = bounds.insetByRect(margin);\
		center = bounds.center;\
		radius = (bounds.width * 0.5) - 4;\
		background = \cf3 Oval\cf0 .fromRect(innerBounds);\
		\cf3 this\cf0 .calcKnob;\
	\}\
	draw \{\
		\cf3 this\cf0 .calcKnob;\
		pen.use(\{\
			\cf3 Pen\cf0 .action = \cf5 \\fill\cf0 ;\
			background.draw;\
			\cf3 Pen\cf0 .action = \cf5 \\stroke\cf0 ;\
			\cf3 Pen\cf0 .penSize = 1@1;\
			\cf3 Pen\cf0 .foreColor = \cf3 Color\cf0 .black;\
			background.draw;\
			\cf3 Pen\cf0 .penSize = 2@2;\
			\cf3 Pen\cf0 .moveTo(innerEdge);\
			\cf3 Pen\cf0 .lineTo(outerEdge)\
		\});\
	\}\
	calcValue \{ \cf3 arg\cf0  where;\
		\cf3 var\cf0  angle;\
		angle = atan2(where.x - center.x, center.y - where.y);\
		value = ((angle + 0.75pi) / 1.5pi).clip(0.0,1.0);\
	\}\
	calcKnob \{\
		\cf3 var\cf0  angle, halfRadius;\
		value = control.get(value);\
		angle = 1.5pi * value + 0.25pi;\
		outerEdge = \cf3 Point\cf0 (\
			center.x - (radius * sin(angle)),\
			center.y + (radius * cos(angle))\
		);	\
		halfRadius = 0.5 * radius;\
		innerEdge = \cf3 Point\cf0 (\
			center.x - (halfRadius * sin(angle)),\
			center.y + (halfRadius * cos(angle))\
		);	\
	\}\
	respond \{ \cf3 arg\cf0  where;\
		\cf3 this\cf0 .calcValue(where);\
		control.set(value);\
	\}\
	layout \{ \cf3 arg\cf0  argBounds;\
		\cf3 this\cf0 .bounds = argBounds;\
	\}\
	value_ \{ \cf3 arg\cf0  val;\
		value = val;\
		control.set(value);\
	\}\
\}\
\
\
\cf3 Label\cf0  \{\
	\cf3 var\cf0  <>pen, <>bounds, <>string, <>layoutSize;\
	*new \{ \cf3 arg\cf0  pen, bounds, string, layoutSize;\
		^\cf3 super\cf0 .newCopyArgs(pen.asPen, bounds.asRect, string.asString, layoutSize ?? \{ \cf3 LayoutSize\cf0 .new \})\
	\}\
	layout \{ \cf3 arg\cf0  argBounds;\
		\cf3 this\cf0 .bounds = argBounds;\
	\}\
	draw \{\
		pen.use(\{ string.drawLeftJustIn(bounds) \});\
	\}\
	mouseOver \{ \cf3 arg\cf0  where, event;\
		if (bounds.containsPoint(where), \{\
			^\cf3 this\cf0 \
		\});\
		^\cf3 nil\cf0 \
	\}\
	getLayoutSize \{ ^layoutSize \}\
\}\
\
\cf3 CLabel\cf0  : \cf3 Label\cf0  \{\
	draw \{\
		pen.use(\{ string.drawCenteredIn(bounds) \});\
	\}\
\}\
\
\cf3 TextBox\cf0  \{\
	\cf3 var\cf0  <>pen, <>bounds, <>string, <>justify=\cf5 \\default\cf0 ;\
	*new \{ \cf3 arg\cf0  pen, bounds, string, justify=\cf5 \\default\cf0 ;\
		^\cf3 super\cf0 .newCopyArgs(pen.asPen, bounds.asRect, string, justify)\
	\}\
	draw \{\
		pen.use(\{\
			\cf3 this\cf0 .prDraw;\
		\});\
	\}\
	layout \{ \cf3 arg\cf0  argBounds;\
		\cf3 this\cf0 .bounds = argBounds;\
	\}\
	prDraw \{\
		\cf3 _TextBox_Draw\cf0 \
		^\cf3 this\cf0 .primitiveFailed\
	\}\
\}\
\
\cf3 WindowLayer\cf0  : \cf3 View\cf0  \{\
	mouseDown \{ \cf3 arg\cf0  where, event;\
		if (enabled, \{\
			items.reverseDo(\{ \cf3 arg\cf0  item; \
				if (item.bounds.containsPoint(where), \{\
					items.swap(items.indexOf(item), items.size - 1);\
					^item.mouseDown(where, event);\
				\});\
			\});\
		\});\
	\}\
\}\
\
	}
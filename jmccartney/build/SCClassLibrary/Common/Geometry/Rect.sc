{\rtf1\mac\ansicpg10000\cocoartf100
{\fonttbl\f0\fnil\fcharset77 Monaco;}
{\colortbl;\red255\green255\blue255;\red191\green0\blue0;\red0\green0\blue191;\red96\green96\blue96;
}
\pard\tx560\tx1120\tx1680\tx2240\tx2800\tx3360\tx3920\tx4480\tx5040\tx5600\tx6160\tx6720\ql\qnatural

\f0\fs18 \cf0 \
\pard\tx560\tx1120\tx1680\tx2240\tx2800\tx3360\tx3920\tx4480\tx5040\tx5600\tx6160\tx6720\ql\qnatural
\cf2 // the drawing routines in these classes use Quickdraw.\
// If you want CoreGraphics drawing, use methods in Pen.\cf0 \
\
\cf3 Rect\cf0  \{\
	\cf3 var\cf0  <>left=0, <>top=0, <>width=0, <>height=0;\
	\
	*new \{ \cf3 arg\cf0  left=0, top=0, width=0, height=0;\
		^\cf3 super\cf0 .newCopyArgs(left, top, width, height)\
	\}\
	*newSides \{ \cf3 arg\cf0  left=0, top=0, right=0, bottom=0;\
		^\cf3 super\cf0 .newCopyArgs(left, top, right-left, bottom-top)\
	\}\
	*fromPoints \{ \cf3 arg\cf0  pt1, pt2;\
		^\cf3 super\cf0 .newCopyArgs(\
			pt1.x min: pt2.x, \
			pt1.y min: pt2.y,\
			absdif(pt1.x, pt2.x),\
			absdif(pt1.y, pt2.y)\
		)\
	\}\
	*fromRect \{ \cf3 arg\cf0  rect;\
		^\cf3 this\cf0 .new(rect.left, rect.top, rect.width, rect.height)\
	\}\
	*aboutPoint \{ \cf3 arg\cf0  point, dx, dy;\
		^\cf3 this\cf0 .new(point.x-dx, point.y-dy, 2*dx, 2*dy)\
	\}\
	\
	set \{ \cf3 arg\cf0  argLeft=0, argTop=0, argWidth=0, argHeight=0;\
		left = argLeft;\
		top = argTop;\
		width = argWidth;\
		height = argHeight;\
	\}\
	setExtent \{ \cf3 arg\cf0  argWidth=0, argHeight=0;\
		width = argWidth;\
		height = argHeight;\
	\}\
        \
	origin \{ ^\cf3 Point\cf0 .new(left, top) \}\
	extent \{ ^\cf3 Point\cf0 .new(width, height) \}\
	center \{ ^\cf3 Point\cf0 .new(left + (width * 0.5), top + (height * 0.5)) \}\
	\
	bottom \{ ^top + height \}\
	right \{ ^left + width \}\
	\
	leftTop \{ ^\cf3 Point\cf0 .new(\cf3 this\cf0 .left, \cf3 this\cf0 .top) \}\
	rightTop \{ ^\cf3 Point\cf0 .new(\cf3 this\cf0 .right, \cf3 this\cf0 .top) \}\
	leftBottom \{ ^\cf3 Point\cf0 .new(\cf3 this\cf0 .left, \cf3 this\cf0 .bottom) \}\
	rightBottom \{ ^\cf3 Point\cf0 .new(\cf3 this\cf0 .right, \cf3 this\cf0 .bottom) \}\
	\
	moveBy \{ \cf3 arg\cf0  h, v;\
		^\cf3 this\cf0 .class.new(left + h, top + v, width, height)\
	\}\
	moveTo \{ \cf3 arg\cf0  h, v;\
		^\cf3 this\cf0 .class.new(h, v, width, height)\
	\}\
	moveToPoint \{ \cf3 arg\cf0  aPoint;\
		^\cf3 this\cf0 .class.new(aPoint.x, aPoint.y, width, height)\
	\}\
	resizeBy \{ \cf3 arg\cf0  h, v;\
		^\cf3 this\cf0 .class.new(left, top, width + h, height + v)\
	\}\
	resizeTo \{ \cf3 arg\cf0  h, v;\
		^\cf3 this\cf0 .class.new(left, top, h, v)\
	\}\
	insetBy \{ \cf3 arg\cf0  h, v;\
		^\cf3 this\cf0 .class.new(left + h, top + v, width - h - h, height - v - v)\
	\}\
	insetAll \{ \cf3 arg\cf0  a, b, c, d;\
		^\cf3 this\cf0 .class.new(left + a, top + b, width - a - c, height - b - d)\
	\}\
	insetByRect \{ \cf3 arg\cf0  r;\
		^\cf3 this\cf0 .copy.insetAll(r.left, r.top, r.right, r.bottom)\
	\}\
	centerSquare \{\
		\cf3 var\cf0  pos, center;\
		if (width > height, \{ \
			pos = (width - height) * 0.5 + left;\
			^\cf3 Rect\cf0 (pos, top, height, height)\
		\},\{\
			pos = (height - width) * 0.5 + top;\
			^\cf3 Rect\cf0 (left, pos, width, width)\
		\});\
	\}\
	centerIn \{ \cf3 arg\cf0  inRect;\
		\cf3 var\cf0  pos, spacing;\
		spacing  = (inRect.extent - \cf3 this\cf0 .extent) * 0.5;\
		^inRect.origin - \cf3 this\cf0 .origin + spacing;\
	\}\
	\
	containsPoint \{ \cf3 arg\cf0  aPoint;\
		^ aPoint.x.inclusivelyBetween(left, left + width) \
			and: \{ aPoint.y.inclusivelyBetween(top, top + height) \}\
	\}\
	intersects \{ \cf3 arg\cf0  aRect;\
		if (aRect.right < \cf3 this\cf0 .left, \{ ^\cf3 false\cf0  \});\
		if (aRect.bottom < \cf3 this\cf0 .top, \{ ^\cf3 false\cf0  \});\
		if (aRect.left > \cf3 this\cf0 .right, \{ ^\cf3 false\cf0  \});\
		if (aRect.top > \cf3 this\cf0 .bottom, \{ ^\cf3 false\cf0  \});\
		^\cf3 true\cf0 \
	\}\
	\
	& \{ \cf3 arg\cf0  aRect; ^\cf3 this\cf0  sect: aRect \}\
	| \{ \cf3 arg\cf0  aRect; ^\cf3 this\cf0  union: aRect \}\
	\
	union \{ \cf3 arg\cf0  aRect;\
		^\cf3 this\cf0 .class.newSides( left min: aRect.left, top min: aRect.top,\
			\cf3 this\cf0 .right max: aRect.right, \cf3 this\cf0 .bottom max: aRect.bottom)\
	\}\
	sect \{ \cf3 arg\cf0  aRect;\
		^\cf3 this\cf0 .class.newSides( left max: aRect.left, top max: aRect.top,\
			\cf3 this\cf0 .right min: aRect.right, \cf3 this\cf0 .bottom min: aRect.bottom)\
	\}\
\
	printOn \{ \cf3 arg\cf0  stream;\
		stream << \cf3 this\cf0 .class.name << \cf4 ".new("\cf0  \
			<<* [left, top, width, height] << \cf4 ")"\cf0 ;\
	\}\
	storeOn \{ \cf3 arg\cf0  stream;\
		stream << \cf3 this\cf0 .class.name << \cf4 ".new("\cf0  \
			<<<* [left, top, width, height] << \cf4 ")"\cf0 ;\
	\}\
	\
	draw \{ \cf3 arg\cf0  color, operation=2;\
		\cf3 _Rect_Draw\cf0 \
		^\cf3 this\cf0 .primitiveFailed\
	\}\
	\
	asRect \{ ^\cf3 this\cf0  \}\
	bounds \{ ^\cf3 Rect\cf0 .new(left, top, width, height) \}\
	layout \{ \cf3 arg\cf0  argBounds; \
		\cf3 this\cf0 .set(argBounds.left, argBounds.top, argBounds.width, argBounds.height);\
	\}\
\
\}\
\
\cf3 Oval\cf0  : \cf3 Rect\cf0  \{\
	draw \{ \cf3 arg\cf0  color, operation=2;\
		\cf3 _Oval_Draw\cf0 \
		^\cf3 this\cf0 .primitiveFailed\
	\}\
\}\
\
\cf3 RoundRect\cf0  : \cf3 Rect\cf0  \{\
	\cf3 var\cf0  <>radius=2;\
	*newSides \{ \cf3 arg\cf0  left=0, top=0, right=0, bottom=0, radius=2;\
		^\cf3 super\cf0 .newSides(left, top, right, bottom).radius_(radius)\
	\}\
	*new \{ \cf3 arg\cf0  left=0, top=0, width=0, height=0, radius=2;\
		^\cf3 super\cf0 .new(left, top, width, height).radius_(radius)\
	\}\
	*fromRect \{ \cf3 arg\cf0  rect, radius=2;\
		^\cf3 super\cf0 .fromRect(rect).radius_(radius)\
	\}\
	draw \{ \cf3 arg\cf0  color, operation=2;\
		\cf3 _RoundRect_Draw\cf0 \
		^\cf3 this\cf0 .primitiveFailed\
	\}\
\}\
\
\cf3 Arc\cf0  : \cf3 Rect\cf0  \{\
	\cf3 var\cf0  <>startAngle=0, <>arcAngle=90;\
	*new \{ \cf3 arg\cf0  left=0, top=0, width=0, height=0, startAngle=0, arcAngle=90;\
		^\cf3 super\cf0 .new(left, top, width, height).startAngle_(startAngle).arcAngle_(arcAngle)\
	\}\
	*newSides \{ \cf3 arg\cf0  left=0, top=0, right=0, bottom=0, startAngle=0, arcAngle=90;\
		^\cf3 super\cf0 .newSides(left, top, right, bottom).startAngle_(startAngle).arcAngle_(arcAngle)\
	\}\
	*fromRect \{ \cf3 arg\cf0  rect, startAngle=0, arcAngle=90;\
		^\cf3 super\cf0 .fromRect(rect).startAngle_(startAngle).arcAngle_(arcAngle)\
	\}\
	draw \{ \cf3 arg\cf0  color, operation=2;\
		\cf3 _Arc_Draw\cf0 \
		^\cf3 this\cf0 .primitiveFailed\
	\}\
\}\
\
\
\cf3 BevelRect\cf0  : \cf3 Rect\cf0  \{\
	\cf3 var\cf0  <>bevelWidth=2, <>inout=\cf3 false\cf0 ;\
	*new \{ \cf3 arg\cf0  left=0, top=0, width=0, height=0, bevelWidth=2, inout=\cf3 false\cf0 ;\
		^\cf3 super\cf0 .new(left, top, width, height).bevelWidth_(bevelWidth).inout_(inout)\
	\}\
	*newSides \{ \cf3 arg\cf0  left=0, top=0, right=0, bottom=0, bevelWidth=2, inout=\cf3 false\cf0 ;\
		^\cf3 super\cf0 .newSides(left, top, right, bottom).bevelWidth_(bevelWidth).inout_(inout)\
	\}\
	*fromRect \{ \cf3 arg\cf0  rect, bevelWidth=2, inout=\cf3 false\cf0 ;\
		^\cf3 super\cf0 .fromRect(rect).bevelWidth_(bevelWidth).inout_(inout)\
	\}\
	draw \{ \cf3 arg\cf0  color, operation=2;\
		\cf3 _BevelRect_Draw\cf0 \
		^\cf3 this\cf0 .primitiveFailed\
	\}\
\}\
\
}
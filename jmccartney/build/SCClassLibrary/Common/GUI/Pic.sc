{\rtf1\mac\ansicpg10000\cocoartf100
{\fonttbl\f0\fnil\fcharset77 Monaco;}
{\colortbl;\red255\green255\blue255;\red0\green0\blue191;\red0\green0\blue0;\red191\green0\blue0;
}
\pard\tx560\tx1120\tx1680\tx2240\tx2800\tx3360\tx3920\tx4480\tx5040\tx5600\tx6160\tx6720\ql\qnatural

\f0\fs18 \cf2 Pic\cf3  \{\
	\cf2 var\cf3  dataptr, <>bounds, <>depth=0, finalizer;\
	\
	*new \{ \cf2 arg\cf3  bounds, depth=32;\
		^\cf2 super\cf3 .new.bounds_(bounds).depth_(depth).prNew\
	\}\
	*read \{ \cf2 arg\cf3  pathname, depth=32;\
		^\cf2 super\cf3 .new.bounds_(\cf2 Rect\cf3 .new).depth_(depth).prRead(pathname)\
	\}\
	close \{\
		finalizer.finalize;\
	\}\
	draw \{\
		\cf2 this\cf3 .prDraw(bounds.moveTo(0,0), bounds);\
	\}\
	getLayoutSize \{\
		\cf2 var\cf3  extent;\
		extent = bounds.extent\
		^\cf2 LayoutSize\cf3 (extent.x, extent.x, extent.y, extent.y, 0)\
	\}\
	layout \{ \cf2 arg\cf3  argBounds;\
		bounds = bounds.moveTo(argBounds.left, argBounds.top);\
	\}\
	\
	\cf4 // PRIVATE\
\cf3 	prNew \{\
		\cf2 _Pic_New\cf3 \
		^\cf2 this\cf3 .primitiveFailed\
	\}\
	prRead \{ \cf2 arg\cf3  pathname;\
		\cf2 _Pic_Read\cf3 \
		^\cf2 this\cf3 .primitiveFailed\
	\}\
	prDraw \{ \cf2 arg\cf3  dstBounds;\
		\cf2 _Pic_Draw\cf3 \
		^\cf2 this\cf3 .primitiveFailed\
	\}\
\}\
\
\cf2 ScaledPic\cf3  \{\
	\cf2 var\cf3  <>pic,  <>bounds;\
	*new \{ \cf2 arg\cf3  pic, bounds;\
		^\cf2 super\cf3 .newCopyArgs(pic, bounds ? pic.bounds)\
	\}\
	draw \{\
		pic.prDraw(bounds);\
	\}\
	layout \{ \cf2 arg\cf3  argBounds;\
		bounds = argBounds;\
	\}\
\}\
\
}
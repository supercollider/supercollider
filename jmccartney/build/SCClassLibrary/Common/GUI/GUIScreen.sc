{\rtf1\mac\ansicpg10000\cocoartf100
{\fonttbl\f0\fnil\fcharset77 Monaco;}
{\colortbl;\red255\green255\blue255;\red0\green0\blue191;\red96\green96\blue96;\red191\green0\blue0;
}
\pard\tx560\tx1120\tx1680\tx2240\tx2800\tx3360\tx3920\tx4480\tx5040\tx5600\tx6160\tx6720\ql\qnatural

\f0\fs18 \cf0 \
\pard\tx560\tx1120\tx1680\tx2240\tx2800\tx3360\tx3920\tx4480\tx5040\tx5600\tx6160\tx6720\ql\qnatural
\cf2 SCWindow\cf0  \{\
	\cf2 classvar\cf0  <>allWindows;\
	\
	\cf2 var\cf0  dataptr, <name, <>onClose, <view;\
	\
	*new \{ \cf2 arg\cf0  name = \cf3 "panel"\cf0 , bounds, resizable = \cf2 true\cf0 , border = \cf2 true\cf0 ;\
		^\cf2 super\cf0 .new.initSCWindow(name, bounds, resizable, border)\
	\}\
	initSCWindow \{ \cf2 arg\cf0  argName, argBounds, resizable, border;\
		name = argName.asString;\
		argBounds = argBounds ? \cf2 Rect\cf0 (128, 64, 400, 400);\
		allWindows = allWindows.add(\cf2 this\cf0 );\
		view = \cf2 SCTopView\cf0 (\cf2 nil\cf0 , argBounds.moveTo(0,0));\
		\cf2 this\cf0 .prInit(name, argBounds, resizable, border, view);\
	\}\
\
	asView \{ ^view \}\
\
	*closeAll \{\
		\cf2 var\cf0  list;\
		list = allWindows.copy;\
		allWindows = \cf2 Array\cf0 .new(8);\
		list.do(\{ \cf2 arg\cf0  window; window.close; \});\
	\}\
		\
	close \{\
		\cf2 this\cf0 .prClose;\
	\}\
	closed \{\
		dataptr = \cf2 nil\cf0 ;\
		onClose.value; \cf4 // call user function\
\pard\tx560\tx1120\tx1680\tx2240\tx2800\tx3360\tx3920\tx4480\tx5040\tx5600\tx6160\tx6720\ql\qnatural
\cf0 		allWindows.remove(\cf2 this\cf0 );\
	\}\
	isClosed \{ ^dataptr.isNil \}\
\
	fullScreen \{\
		\cf2 _SCWindow_BeginFullScreen\cf0 \
		^\cf2 this\cf0 .primitiveFailed\
	\}\
	endFullScreen \{\
		\cf2 _SCWindow_EndFullScreen\cf0 \
		^\cf2 this\cf0 .primitiveFailed\
	\}\
	front \{\
		\cf2 _SCWindow_ToFront\cf0 \
		^\cf2 this\cf0 .primitiveFailed\
	\}\
	refresh \{\
		\cf2 _SCWindow_Refresh\cf0 \
		^\cf2 this\cf0 .primitiveFailed\
	\}\
	minimize \{\
		\cf2 _SCWindow_Minimize\cf0 \
		^\cf2 this\cf0 .primitiveFailed\
	\}\
	alpha_ \{ \cf2 arg\cf0  alpha;\
		\cf2 _SCWindow_SetAlpha\cf0 \
		^\cf2 this\cf0 .primitiveFailed\
	\}\
	\
	name_ \{ \cf2 arg\cf0  argName;\
		name = argName;\
		\cf2 this\cf0 .prSetName(argName);\
	\}\
	bounds_ \{ \cf2 arg\cf0  argBounds;\
		\cf2 this\cf0 .prSetBounds(argBounds);\
	\}\
	bounds \{\
		^\cf2 this\cf0 .prGetBounds(\cf2 Rect\cf0 .new);\
	\}\
	\
	play \{ \cf2 arg\cf0  function;\
		\cf2 AppClock\cf0 .play(\{ \
			if (dataptr.notNil, \{\
				function.value;\
			\});\
		\});\
		\
	\}\
			\
	\cf4 // PRIVATE\
\cf0 	\cf4 // primitives\
\cf0 	prInit \{ \cf2 arg\cf0  argName, argBounds, resizable, border;\
		\cf2 _SCWindow_New\cf0 \
		^\cf2 this\cf0 .primitiveFailed\
	\}\
	prClose \{\
		\cf2 _SCWindow_Close\cf0 \
		^\cf2 this\cf0 .primitiveFailed\
	\}\
	prSetName \{ \cf2 arg\cf0  argName;\
		\cf2 _SCWindow_SetName\cf0 \
		^\cf2 this\cf0 .primitiveFailed\
	\}\
	prGetBounds \{ \cf2 arg\cf0  argBounds;\
		\cf2 _SCWindow_GetBounds\cf0 \
		^\cf2 this\cf0 .primitiveFailed\
	\}\
	prSetBounds \{ \cf2 arg\cf0  argBounds;\
		\cf2 _SCWindow_SetBounds\cf0 \
		^\cf2 this\cf0 .primitiveFailed\
	\}\
\}\
}
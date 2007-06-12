{\rtf1\mac\ansicpg10000\cocoartf100
{\fonttbl\f0\fnil\fcharset77 Monaco;}
{\colortbl;\red255\green255\blue255;\red0\green0\blue191;\red0\green0\blue0;\red191\green0\blue0;
}
\pard\tx560\tx1120\tx1680\tx2240\tx2800\tx3360\tx3920\tx4480\tx5040\tx5600\tx6160\tx6720\ql\qnatural

\f0\fs18 \cf2 Pen\cf3  \{\
	*use \{ \cf2 arg\cf3  function;\
		\cf2 var\cf3  res;\
		\cf2 this\cf3 .push;\
		res = function.value;\
		\cf2 this\cf3 .pop;\
		^res\
	\}\
\
	*translate \{ \cf2 arg\cf3  x=0, y=0;\
		\cf2 _Pen_Translate\cf3 	\
		^\cf2 this\cf3 .primitiveFailed\
	\}\
	*scale \{ \cf2 arg\cf3  x=0, y=0;\
		\cf2 _Pen_Scale\cf3 	\
		^\cf2 this\cf3 .primitiveFailed\
	\}\
	*rotate \{ \cf2 arg\cf3  angle=0, x=0, y=0;\
		\cf2 _Pen_Rotate\cf3 	\
		^\cf2 this\cf3 .primitiveFailed\
	\}\
\
	\
	*width_ \{ \cf2 arg\cf3  width=1;\
		\cf2 _Pen_SetWidth\cf3 \
		^\cf2 this\cf3 .primitiveFailed\
	\}\
\
	\cf4 // Paths:\
\cf3 	*path \{ \cf2 arg\cf3  function;\
		\cf2 var\cf3  res;\
		\cf2 this\cf3 .beginPath;\
		res = function.value;\
		\cf2 this\cf3 .endPath;\
		^res\
	\}\
	*beginPath \{\
		\cf2 _Pen_BeginPath\cf3 \
		^\cf2 this\cf3 .primitiveFailed\
	\}\
	*endPath \{\
		\cf2 _Pen_EndPath\cf3 \
		^\cf2 this\cf3 .primitiveFailed\
	\}\
	*moveTo \{ \cf2 arg\cf3  point;\
		\cf2 _Pen_MoveTo\cf3 \
		^\cf2 this\cf3 .primitiveFailed\
	\}\
	*lineTo \{ \cf2 arg\cf3  point;\
		\cf2 _Pen_LineTo\cf3 \
		^\cf2 this\cf3 .primitiveFailed\
	\}\
	*curveTo \{ \cf2 arg\cf3  point, cpoint1, cpoint2;\
		\cf2 _Pen_CurveTo\cf3 \
		^\cf2 this\cf3 .primitiveFailed\
	\}\
	*quadCurveTo \{ \cf2 arg\cf3  point, cpoint1;\
		\cf2 _Pen_QuadCurveTo\cf3 \
		^\cf2 this\cf3 .primitiveFailed\
	\}\
	*addArc \{ \cf2 arg\cf3  center, startAngle, arcAngle;\
		\cf2 _Pen_AddArc\cf3 \
		^\cf2 this\cf3 .primitiveFailed\
	\}\
	*addRect \{ \cf2 arg\cf3  rect;\
		\cf2 _Pen_AddRect\cf3 \
		^\cf2 this\cf3 .primitiveFailed\
	\}\
	*stroke \{\
		\cf2 _Pen_StrokePath\cf3 \
		^\cf2 this\cf3 .primitiveFailed\
	\}\
	*fill \{\
		\cf2 _Pen_FillPath\cf3 \
		^\cf2 this\cf3 .primitiveFailed\
	\}\
	*matrix_ \{ \cf2 arg\cf3  array;\
		\cf2 _Pen_SetMatrix\cf3 \
		^\cf2 this\cf3 .primitiveFailed\
	\}\
	\
	*strokeRect \{ \cf2 arg\cf3  rect;\
		\cf2 _Pen_StrokeRect\cf3 \
		^\cf2 this\cf3 .primitiveFailed\
	\}\
	*fillRect \{ \cf2 arg\cf3  rect;\
		\cf2 _Pen_FillRect\cf3 \
		^\cf2 this\cf3 .primitiveFailed\
	\}\
	*strokeOval \{ \cf2 arg\cf3  rect;\
		\cf2 _Pen_StrokeOval\cf3 \
		^\cf2 this\cf3 .primitiveFailed\
	\}\
	*fillOval \{ \cf2 arg\cf3  rect;\
		\cf2 _Pen_FillOval\cf3 \
		^\cf2 this\cf3 .primitiveFailed\
	\}\
	\
	*drawAquaButton \{ \cf2 arg\cf3  rect, type=0, down=\cf2 false\cf3 , on=\cf2 false\cf3 ;\
		\cf2 _Pen_DrawAquaButton\cf3 \
		^\cf2 this\cf3 .primitiveFailed\
	\}\
\
	\cf4 //PRIVATE:\
\cf3 	*push \{\
		\cf2 _Pen_Push\cf3 \
		^\cf2 this\cf3 .primitiveFailed\
	\}\
	*pop \{\
		\cf2 _Pen_Pop\cf3 \
		^\cf2 this\cf3 .primitiveFailed\
	\}\
\}\
\
}
{\rtf1\mac\ansicpg10000\cocoartf100
{\fonttbl\f0\fnil\fcharset77 Monaco;}
{\colortbl;\red255\green255\blue255;\red0\green0\blue0;\red0\green0\blue191;\red96\green96\blue96;
\red191\green0\blue0;}
\pard\tx560\tx1120\tx1680\tx2240\tx2800\tx3360\tx3920\tx4480\tx5040\tx5600\tx6160\tx6720\ql\qnatural

\f0\fs18 \cf2 \
\cf3 String\cf2 [char] : \cf3 RawArray\cf2  \{\
	\
	asSymbol \{ \
		\cf3 _StringAsSymbol\cf2  \
		^\cf3 this\cf2 .primitiveFailed\
	\}\
	asInteger \{ \
		\cf3 _String_AsInteger\cf2 \
		^\cf3 this\cf2 .primitiveFailed\
	\}\
	asFloat \{ \
		\cf3 _String_AsFloat\cf2 \
		^\cf3 this\cf2 .primitiveFailed\
	\}\
	\
	compare \{ \cf3 arg\cf2  aString; \cf3 _StringCompare\cf2  \}\
	< \{ \cf3 arg\cf2  aString; ^\cf3 this\cf2 .compare(aString) < 0 \}\
	== \{ \cf3 arg\cf2  aString; ^\cf3 this\cf2 .compare(aString) == 0 \}\
	!= \{ \cf3 arg\cf2  aString; ^\cf3 this\cf2 .compare(aString) != 0 \}\
	hash \{ \cf3 _StringHash\cf2  \}\
	\
	asString \{ ^\cf3 this\cf2  \}\
	asCompileString \{ ^\cf4 "\\"" ++ this ++ "\cf2 \\\cf4 ""\cf2  \}\
	species \{ ^\cf3 String\cf2  \}\
	\
	postln \{ \cf3 _PostLine\cf2  \}\
	post \{ \cf3 _PostString\cf2  \}\
	postcln \{ \cf4 "// "\cf2 .post; \cf3 this\cf2 .postln; \}\
	postc \{ \cf4 "// "\cf2 .post; \cf3 this\cf2 .post; \}\
\
	newTextWindow \{ \cf3 arg\cf2  title=\cf4 "Untitled"\cf2 , makeListener=\cf3 false\cf2 ;\
		\cf3 _NewTextWindow\cf2  \
		^\cf3 this\cf2 .primitiveFailed\
	\}\
	openTextFile \{ \cf3 arg\cf2  selectionStart=0, selectionLength=0;\
		\cf3 _OpenTextFile\cf2  \
		^\cf3 this\cf2 .primitiveFailed\
	\}\
	\
	*fromUser \{ \cf3 arg\cf2  prompt=\cf4 "Enter string :"\cf2 , default=\cf4 ""\cf2 ;\
		\cf3 _GetStringFromUser\cf2 \
		^\cf3 this\cf2 .primitiveFailed\
	\}\
	\
	error \{ \cf4 "ERROR:\\n"\cf2 .post; \cf3 this\cf2 .postln; \}\
	warn \{ \cf4 "WARNING:\\n"\cf2 .post; \cf3 this\cf2 .postln \}\
	inform \{ ^\cf3 this\cf2 .postln \}\
	++ \{ \cf3 arg\cf2  anObject; ^\cf3 this\cf2  prCat: anObject.asString; \}\
	+ \{ \cf3 arg\cf2  anObject; ^\cf3 this\cf2  prCat: \cf4 " "\cf2  prCat: anObject.asString; \}\
	catArgs \{ \cf3 arg\cf2  ... items; ^\cf3 this\cf2 .catList(items) \}\
	scatArgs \{ \cf3 arg\cf2  ... items; ^\cf3 this\cf2 .scatList(items) \}\
	ccatArgs \{ \cf3 arg\cf2  ... items; ^\cf3 this\cf2 .scatList(items) \}\
	catList \{ \cf3 arg\cf2  list; \
		\cf5 // concatenate this with a list as a string\
\cf2 		\cf3 var\cf2  string;\
		string = \cf3 this\cf2 .copy;\
		list.do(\{ \cf3 arg\cf2  item, i;\
			string = string ++ item;\
		\});\
		^string\
	\}\
	scatList \{ \cf3 arg\cf2  list; \
		\cf3 var\cf2  string;\
		string = \cf3 this\cf2 .copy;\
		list.do(\{ \cf3 arg\cf2  item, i;\
			string = string primCat: \cf4 " "\cf2  ++ item;\
		\});\
		^string\
	\}\
	ccatList \{ \cf3 arg\cf2  list; \
		\cf3 var\cf2  string;\
		string = \cf3 this\cf2 .copy;\
		list.do(\{ \cf3 arg\cf2  item, i;\
			string = string primCat: \cf4 ", "\cf2  ++ item;\
		\});\
		^string\
	\}\
\
	compile \{ ^\cf3 thisProcess\cf2 .interpreter.compile(\cf3 this\cf2 ); \}\
	interpret \{ ^\cf3 thisProcess\cf2 .interpreter.interpret(\cf3 this\cf2 ); \} \
	interpretPrint \{ ^\cf3 thisProcess\cf2 .interpreter.interpretPrint(\cf3 this\cf2 ); \}\
\
	draw \{\
		\cf3 this\cf2 .drawAtPoint(\cf3 Point\cf2 (0,0), \cf3 Font\cf2 .default, \cf3 Color\cf2 .black);\
	\}\
	drawAtPoint \{ \cf3 arg\cf2  point, font, color;\
		\cf3 _String_DrawAtPoint\cf2 \
		^\cf3 this\cf2 .primitiveFailed\
	\}\
	drawInRect \{ \cf3 arg\cf2  rect, font, color;\
		\cf3 _String_DrawInRect\cf2 \
		^\cf3 this\cf2 .primitiveFailed\
	\}\
	drawCenteredIn \{ \cf3 arg\cf2  inRect;\
		\cf3 this\cf2 .drawAtPoint(\cf3 this\cf2 .bounds.centerIn(inRect).origin);\
	\}\
	drawLeftJustIn \{ \cf3 arg\cf2  inRect;\
		\cf3 var\cf2  pos, bounds;\
		bounds = \cf3 this\cf2 .bounds;\
		pos = bounds.centerIn(inRect);\
		pos.x = inRect.left + 2;\
		\cf3 this\cf2 .drawAtPoint(pos);\
	\}\
	drawRightJustIn \{ \cf3 arg\cf2  inRect;\
		\cf3 var\cf2  pos, bounds;\
		bounds = \cf3 this\cf2 .bounds;\
		pos = bounds.centerIn(inRect);\
		pos.x = inRect.right - 2 - bounds.width;\
		\cf3 this\cf2 .drawAtPoint(pos);\
	\}\
	\
	*readNew \{ \cf3 arg\cf2  file;\
		^file.readAllString;\
	\}\
	prCat \{ \cf3 arg\cf2  aString; \cf3 _ArrayCat\cf2  \}\
	\
	printOn \{ \cf3 arg\cf2  stream;\
		stream.putAll(\cf3 this\cf2 );\
	\}\
	storeOn \{ \cf3 arg\cf2  stream;\
		stream.putAll(\cf3 this\cf2 .asCompileString);\
	\}\
	\
	bounds \{ ^\cf3 this\cf2 .prBounds(\cf3 Rect\cf2 .new) \}\
	getLayoutSize \{\
		\cf3 arg\cf2  extent;\
		extent = \cf3 this\cf2 .bounds.extent;\
		^\cf3 LayoutSize\cf2 (extent.x+1, extent.x+1, extent.y+1, extent.y+1, 0);\
	\}\
\
	prBounds \{ \cf3 arg\cf2  rect;\
		\cf3 _String_GetBounds\cf2 \
		^\cf3 this\cf2 .primitiveFailed\
	\}\
	\
	inspectorClass \{ ^\cf3 StringInspector\cf2  \}\
	\
	\cf5 /// unix\
\cf2 	\
	\cf5 // runs a unix command and returns the result code.\
\cf2 	\cf5 //unixCmd \{ _String_System ^this.primitiveFailed \}\
\cf2 	\
	\cf5 // runs a unix command and sends stdout to the post window\
\cf2 	unixCmd \{ \cf3 _String_POpen\cf2  ^\cf3 this\cf2 .primitiveFailed \}\
\
	gethostbyname \{ \cf3 _GetHostByName\cf2  ^\cf3 this\cf2 .primitiveFailed \}\
	\
	\cf5 /// code gen\
\cf2 	codegen_UGenCtorArg \{ \cf3 arg\cf2  stream; \
		stream << \cf3 this\cf2 .asCompileString; \
	\}\
	ugenCodeString \{ \cf3 arg\cf2  ugenIndex, isDecl, inputNames=#[], inputStrings=#[];\
		\cf3 _UGenCodeString\cf2 \
		^\cf3 this\cf2 .primitiveFailed\
	\}\
\}\
\
}
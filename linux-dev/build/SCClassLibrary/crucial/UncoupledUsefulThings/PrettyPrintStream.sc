{\rtf1\mac\ansicpg10000\cocoartf100
{\fonttbl\f0\fnil\fcharset77 Monaco;}
{\colortbl;\red255\green255\blue255;\red0\green0\blue191;\red191\green0\blue0;\red96\green96\blue96;
}
\pard\tx560\tx1120\tx1680\tx2240\tx2800\tx3360\tx3920\tx4480\tx5040\tx5600\tx6160\tx6720\ql\qnatural

\f0\fs18 \cf0 \
\cf2 PrettyPrintStream\cf0  : \cf2 CollStream\cf0  \{ \cf3 // well prettier anyway\
\cf0 \
	\cf2 var\cf0  tabs=0;\
\
	nl \{\
		super.nl;\
		tabs.do(\{		\cf2 this\cf0 .tab; \});	\
	\}\
	\
	storeArgs \{ \cf2 arg\cf0   args,itemsPerLine=1;\
		\cf2 this\cf0  << \cf4 "("\cf0 ;\
		tabs = tabs + 1;\
		this.nl;\
		args.storeItemsOn(\cf2 this\cf0 ,itemsPerLine);\
		tabs = tabs - 1;\
		this.nl;\
		\cf2 this\cf0  << \cf4 ")"\cf0 ;\
	\}\
		\
\}}
{\rtf1\mac\ansicpg10000\cocoartf100
{\fonttbl\f0\fnil\fcharset77 Monaco;}
{\colortbl;\red255\green255\blue255;\red0\green0\blue191;\red0\green115\blue0;\red191\green0\blue0;
}
\pard\tx560\tx1120\tx1680\tx2240\tx2800\tx3360\tx3920\tx4480\tx5040\tx5600\tx6160\tx6720\ql\qnatural

\f0\fs18 \cf0 \
\cf2 NetAddr\cf0  \{\
	\cf2 var\cf0  <addr, <>port, <hostname, <socket = -1;\
	*new \{ \cf2 arg\cf0  hostname, port;\
		^\cf2 super\cf0 .newCopyArgs(hostname.gethostbyname, port, hostname);\
	\}\
	hostname_ \{ \cf2 arg\cf0  inHostname;\
		hostname = inHostname;\
		addr = inHostname.gethostbyname;\
	\}\
	\
	send \{ \cf2 arg\cf0  oscAddress ... args;\
		\cf2 _OSC_Send\cf0 \
		^\cf2 this\cf0 .primitiveFailed;\
	\}\
	sendNoTags \{ \cf2 arg\cf0  oscAddress ... args;\
		\cf2 _OSC_SendNoTags\cf0 \
		^\cf2 this\cf0 .primitiveFailed;\
	\}\
	\
	sendArray \{ \cf2 arg\cf0  array;\
		\cf2 this\cf0 .performList(\cf3 \\send\cf0 , array);\
	\}\
\
\cf4 ///////////////\
\cf0 \
	sendRaw \{ \cf2 arg\cf0  rawArray;\
		\cf2 _NetAddr_SendRaw\cf0 \
		^\cf2 this\cf0 .primitiveFailed;\
	\}\
	sendMsg \{ \cf2 arg\cf0  ... args;\
		\cf2 this\cf0 .sendBundle(\cf2 nil\cf0 , args);\
	\}\
	sendBundle \{ \cf2 arg\cf0  time ... args;\
		\cf2 _NetAddr_SendBundle\cf0 \
		^\cf2 this\cf0 .primitiveFailed;\
	\}\
	\
	== \{ \cf2 arg\cf0  that; \
		^(\cf2 this\cf0 .port == that.port) and: \{ \cf2 this\cf0 .addr == that.addr \}\
	\}\
	hash \{ \cf2 arg\cf0  that;\
		^addr.hash bitXor: port.hash\
	\} \
\}\
}
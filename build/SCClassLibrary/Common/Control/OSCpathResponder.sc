{\rtf1\mac\ansicpg10000\cocoartf102
{\fonttbl\f0\fnil\fcharset77 Monaco;\f1\fswiss\fcharset77 Helvetica;\f2\fswiss\fcharset77 Helvetica-Bold;
}
{\colortbl;\red255\green255\blue255;\red191\green0\blue0;\red0\green0\blue191;\red0\green115\blue0;
}
\pard\tx560\tx1120\tx1680\tx2240\tx2800\tx3360\tx3920\tx4480\tx5040\tx5600\tx6160\tx6720\ql\qnatural

\f0\fs18 \cf0 \
\cf2 /* \
OSCpathDispatcher dispatches OSC commands to OSCresponders\
whose cmdNames are actually command paths.\
\
path: an array of the indices in the OSC message that are the path values\
\
*/\cf0 \
\cf3 OSCpathDispatcher\cf0  : \cf3 OSCresponder\cf0  \{\
\
	\cf3 classvar\cf0  <>cmdPathSizes;\
	\cf3 var\cf0  <>all;\
	\cf3 var\cf0  <>pathIndices;\
	\
	*initClass \{\
		cmdPathSizes = \cf3 IdentityDictionary\cf0 .new;		\
		cmdPathSizes.put(\cf4 '/b_set'\cf0 ,	2);\
		cmdPathSizes.put(\cf4 '/b_setn'\cf0 ,	2);\
		cmdPathSizes.put(\cf4 '/c_set'\cf0 ,	1);\
		cmdPathSizes.put(\cf4 '/c_setn'\cf0 ,	1);\
		cmdPathSizes.put(\cf4 '/n_set'\cf0 ,	2);\
		cmdPathSizes.put(\cf4 '/n_setn'\cf0 ,	2);\
		cmdPathSizes.put(\cf4 '/tr'\cf0 ,		2);\
	\}	\
	*new \{  \cf3 arg\cf0  addr, cmdName, action, pathIndices;\
		^\cf3 super\cf0 .new(addr, cmdName, action).init(pathIndices);\
	\}\
	init \{ \cf3 arg\cf0  argPathIndices;\
		pathIndices = argPathIndices ? \cf3 Array\cf0 .series(cmdPathSizes.at(cmdName),1);\
		all = \cf3 Set\cf0 .new;\
	\}\
	value \{ \cf3 arg\cf0  time, msg; \
		\cf3 var\cf0  cmdPath, match, responder;\
		cmdPath = [cmdName] ++ pathIndices.collect(\{ \cf3 arg\cf0  i; msg.at(i) \});\
		responder = \cf3 OSCpathResponder\cf0 (addr, cmdPath);\
		match = all.findMatch(responder);\
		if (match.notNil, \{ \
			match.value(time, msg); \
			^\cf3 true\cf0  \
		\});\
		pathIndices.size.do(\{ \cf3 arg\cf0  i;\
			responder.path.put(i,\cf3 nil\cf0 );\
			match = all.findMatch(responder);\
			if (match.notNil, \{ \
				match.value(time, msg); \
				^\cf3 true\cf0  \
			\});\
		\});\
		^\cf3 false\cf0 \
	\}	\
	addChild \{ \cf3 arg\cf0  responder;\
		\cf3 var\cf0  old;\
		old = all.findMatch(responder);\
		if(old.notNil,\{ all.remove(old) \});\
		all.add(responder);\
	\}\
	removeChild \{ \cf3 arg\cf0  responder;\
		 all.remove(responder)\
	\}\
\}\
\
\cf3 OSCpathResponder\cf0  : \cf3 OSCresponder\cf0  \{\
	\cf3 var\cf0  <>path;\
	\cf3 var\cf0  <>dispatcher;\
	\
	*new \{ \cf3 arg\cf0  addr, cmdPath, action; \
		\cf3 var\cf0  cmdName, path;\
		#cmdName ...path = cmdPath;\
		^\cf3 super\cf0 .newCopyArgs(addr, cmdName, action, path);\
	\}\
	\
	findDispatcher \{\
		\cf3 var\cf0  responder, match;\
		responder = \cf3 OSCpathDispatcher\cf0 (addr, cmdName);\
		match = all.findMatch(responder);\
		if(match.isNil, \{\
			match = responder.add;\
		\});\
		match.addChild(\cf3 this\cf0 );\
		^match;\
	\}		\
	add \{ \
		dispatcher = \cf3 this\cf0 .findDispatcher;\
		dispatcher.addChild(\cf3 this\cf0 );\
	\}\
	\
	remove \{ \
		dispatcher.removeChild(\cf3 this\cf0 );\
	\}\
		\
	== \{ \cf3 arg\cf0  that;\
		^(path == that.path) \
	\}\
\}		\
\
\cf2 /*\
\pard\tx560\tx1120\tx1680\tx2240\tx2800\tx3360\tx3920\tx4480\tx5040\tx5600\tx6160\tx6720\ri-6160\ql\qnatural

\f1\fs24 \cf2 (\
	var s, commandpath, response, aSynth, nodeID, triggerID;\
	s = Server.local;\
	s.boot;\
	triggerID = 1;\
	aSynth = \{arg freq = 1, triggerID = 1; SendTrig.kr(SinOsc.kr(freq), triggerID, 666); \}.play;\
	nodeID = aSynth.nodeID;\
	commandpath = [
\f2\b '/tr', nodeID, triggerID
\f1\b0 ];\
	response = \{ arg time, responder, message; message.postln \};\
\
	o = OSCpathResponder(s.addr, commandpath, response);\
	o.add;\
\
)\
\
o.remove\
\
(\
	var s, commandpath, response, aSynth, nodeID, triggerID;\
	s = Server.local;\
	commandpath = [
\f2\b '/tr', nil, triggerID
\f1\b0 ];\
	response = \{ arg time, responder, message; message.postln \};\
\
	o = OSCpathResponder(s.addr, commandpath, response);\
	o.add;\
)\
\
*/\cf0 \
\
}
{\rtf1\mac\ansicpg10000\cocoartf100
{\fonttbl\f0\fnil\fcharset77 Monaco;}
{\colortbl;\red255\green255\blue255;\red191\green0\blue0;\red0\green0\blue0;\red0\green0\blue191;
\red0\green115\blue0;\red96\green96\blue96;}
\pard\tx560\tx1120\tx1680\tx2240\tx2800\tx3360\tx3920\tx4480\tx5040\tx5600\tx6160\tx6720\ql\qnatural

\f0\fs18 \cf2 // this is probably not working anymore. see NetAddr.sc instead for the newer way.\
\cf3 \
\cf2 // Open Sound Control support\
\cf3 \
\cf4 OSCNode\cf3  \{\
	\cf4 classvar\cf3  <>sc; \cf2 // this contains the "/sc" default behaviour for all input ports.\
\cf3 \
	\cf2 // a not-very-secure password for access to the compiler\
\cf3 	\cf2 // setting password to a Symbol opens SC to potential hacking\
\cf3 	\cf4 classvar\cf3  <>password; \
\
	\cf4 var\cf3  <name, <>children, <>function, <>value, <port;\
	\
	*new \{ \cf4 arg\cf3  name, function, children, value;\
		^\cf4 super\cf3 .newCopyArgs(name, children, function ? \{ \cf4 arg\cf3  node, x; x \}, value);\
	\}\
	*tree \{ \cf4 arg\cf3  children, argName, argValue;\
		children = children.collect(\{ \cf4 arg\cf3  subtree; \
			\cf4 var\cf3  name, thing, value;\
			#name, thing, value = subtree;\
			if (thing.isKindOf(\cf4 Array\cf3 ), \{\
				\cf4 this\cf3 .tree(thing, name, value); \
			\},\{\
				\cf4 OSCNode\cf3 (name, thing, \cf4 nil\cf3 , value);\
			\});\
		\});\
		^\cf4 OSCNode\cf3 (argName, \cf4 nil\cf3 , children, argValue);\
	\}\
	add \{ \cf4 arg\cf3  child;\
		if (children.isNil, \{ children = \cf4 Array\cf3 .new(4) \});\
		children = children.add(child);\
	\}\
	remove \{ \cf4 arg\cf3  child;\
		if (children.notNil, \{ children.take(child); \});\
	\}\
	empty \{\
		children = \cf4 nil\cf3 ;\
	\}\
	name_ \{ \cf4 arg\cf3  theName; name = theName.asSymbol; \}\
	\
	\cf2 // these are for send and reply on the same machine.\
\cf3 	send \{ \cf4 arg\cf3  oscAddress ... args;\
		\cf4 _OSCNode_Send\cf3 \
		^\cf4 this\cf3 .primitiveFailed;\
	\}\
	reply \{ \cf4 arg\cf3  oscAddress ... args;\
		\cf4 _OSCNode_Send\cf3 \
		^\cf4 this\cf3 .primitiveFailed;\
	\}\
		\
	*initClass \{\
		\cf2 // Build the default behaviour for all input ports.\
\cf3 		\cf2 // You might want to comment some or all of this out for installations \
\cf3 		\cf2 // so you can't be hacked..\
\cf3 		sc = \cf4 this\cf3 .tree([\
			[\cf5 \\interpreter\cf3 , [\
				\cf2 // Warning: the 'run' method makes SC vulnerable executing any\
\cf3 				\cf2 // code sent to it over the net. If you don't want to worry about it\
\cf3 				\cf2 // then never set the password and you'll be safe.\
\cf3 				[\cf5 \\run\cf3 , \{ \cf4 arg\cf3  node, msgPassword, string; \
					if (password.isKindOf(\cf4 Symbol\cf3 ) and: \{ password === msgPassword \}, \
					\{\
						string.interpret; \cf2 // a powerful tool for both Good and Evil...\
\cf3 					\})\
				\}],\
				[\cf5 \\set\cf3 , \{ \cf4 arg\cf3  node, varName, value; \cf2 // set one of the variables a-z in the interpreter\
\cf3 					\cf4 thisProcess\cf3 .interpreter.perform(varName.asSetter, value);\
				\}],\
				[\cf5 \\clearAll\cf3 , \{ \cf4 thisProcess\cf3 .interpreter.clearAll; \}],\
				[\cf5 \\executeFile\cf3 , \{ \cf4 arg\cf3  node, pathname; \cf4 thisProcess\cf3 .interpreter.executeFile(pathname); \}]\
			]],\
			\
			[\cf5 \\library\cf3 , [\
				[\cf5 \\choose\cf3 , \{ \cf4 arg\cf3  node, names; \cf4 Library\cf3 .prLibMenuChoose(names); \}],\
				[\cf5 \\perform\cf3 , \{ \cf4 arg\cf3  node, names, selector ... args; \
					\cf4 var\cf3  obj;\
					obj = \cf4 Library\cf3 .at(names); \
					if (obj.notNil, \{\
						obj.performList(selector, args);\
					\});\
				\}]\
			]],\
			\
			[\cf5 \\obj\cf3 , [\
				[\cf5 \\perform\cf3 , \{ \cf4 arg\cf3  node, id, selector ... args; \
					\cf4 var\cf3  obj;\
					obj = \cf4 ObjectTable\cf3 .at(id); \
					if (obj.notNil, \{\
						obj.performList(selector, args);\
					\});\
				\}],\
				\
				\cf2 // a function cannot be sent via the OSC, so 'put' only works using\
\cf3 				\cf2 // inter-VM-messaging on the same machine. \
\cf3 				[\cf5 \\put\cf3 , \{ \cf4 arg\cf3  node, id, function; \cf4 ObjectTable\cf3 .put(id, function.value); \}],\
				\
				[\cf5 \\remove\cf3 , \{ \cf4 arg\cf3  node, id; \cf4 ObjectTable\cf3 .remove(id); \}]\
			]],\
			[\cf5 \\sound\cf3 , [\
				[\cf5 \\play\cf3 , \{ \cf4 arg\cf3  node, pathName, duration, loop=\cf4 false\cf3 ;\
					\cf4 SoundFile\cf3 .play(pathName, duration: duration, loop: loop);\
				\}]\
			]],\
			[\cf5 \\mixer\cf3 , [\
				[\cf5 \\stop\cf3 , \{ \cf4 arg\cf3  node, channel;\
					\cf4 MixerPlayer\cf3 .forInput(channel, \{ \cf4 arg\cf3  input; \
						input.killBox.value = 0;\
						input.killBox.doAction; \
					\});\
				\}],\
				[\cf5 \\copy\cf3 , \{ \cf4 arg\cf3  node, channel;\
					\cf4 MixerPlayer\cf3 .forInput(channel, \{ \cf4 arg\cf3  input; \
						input.copyBtn.doAction; \
					\});\
				\}],\
				[\cf5 \\volume\cf3 , \{ \cf4 arg\cf3  node, channel, volume = 0.0;\
					\cf4 MixerPlayer\cf3 .forInput(channel, \{ \cf4 arg\cf3  input; \
						input.volumeSlider.value = volume; \
					\});\
				\}],\
				[\cf5 \\close\cf3 , \{ \cf4 arg\cf3  node;\
					\cf4 MixerPlayer\cf3 .mixer.notNil.if(\{\
						\cf4 MixerPlayer\cf3 .mixer.close; \
					\});\
				\}]\
			]],\
			[\cf5 \\ping\cf3 , \{ \cf4 arg\cf3  node; node.port.reply(\cf6 "/sc/postln"\cf3 , \cf6 "I'm listening"\cf3 ) \}], \
			[\cf5 \\run\cf3 , \{ \cf4 thisProcess\cf3 .run \}], \
			[\cf5 \\stop\cf3 , \{ \cf4 Synth\cf3 .stop \}],\
			[\cf5 \\post\cf3 , \{ \cf4 arg\cf3  node ... args; args.do(\{ \cf4 arg\cf3  item; item.post; \cf6 " "\cf3 .post; \}); \cf6 "\\n"\cf3 .post; \}],\
			[\cf5 \\postln\cf3 , \{ \cf4 arg\cf3  node ... args; args.do(\{ \cf4 arg\cf3  item; item.postln; \});\}]\
		]);\
	\}\
	\
	\cf2 // PRIVATE\
\cf3 	call \{ \cf4 arg\cf3  rcvPort ... args;\
		port = rcvPort;\
		value = function.valueArray(\cf4 this\cf3 , args);\
		port = \cf4 nil\cf3 ;\
	\}\
\}\
\
\cf2 ////////////////////////////\
\cf3 \
\cf2 // UDP support\
\cf3 \
\cf4 OSCPort\cf3  \{\
	\cf4 classvar\cf3  <openPorts; \cf2 // keep track of all open ports\
\cf3 	\cf4 var\cf3  <portID, udpPort, <>oscNode;\
	\
	*closeAll \{\
		if (openPorts.notNil, \{\
			openPorts.copy.do(\{ \cf4 arg\cf3  port; port.close; \});\
		\});\
	\}\
	close \{\
		openPorts.remove(\cf4 this\cf3 );\
		\cf4 this\cf3 .prClose;\
	\}\
\
	\cf2 // PRIVATE\
\cf3 	prClose \{\
		\cf4 _OSCPort_Close\cf3 \
		^\cf4 this\cf3 .primitiveFailed;\
	\}\
	addOpenPort \{\
		if (openPorts.isNil, \{ openPorts = \cf4 Array\cf3 .new(4); \});\
		openPorts = openPorts.add(\cf4 this\cf3 );\
	\}\
\}\
\
\cf4 OSCInPort\cf3  : \cf4 OSCPort\cf3  \{\
	\cf2 // UDP port for incoming OSC messages\
\cf3 	\
	*new \{ \cf4 arg\cf3  portID, oscNode;\
		^\cf4 super\cf3 .new.init(portID, oscNode)\
	\}\
	reply \{ \cf4 arg\cf3  oscAddress ... args;\
		\cf4 _OSCInPort_Reply\cf3 \
		^\cf4 this\cf3 .primitiveFailed;\
	\}\
	\
	\cf2 // PRIVATE\
\cf3 	init \{ \cf4 arg\cf3  argPortID, argOSCNode;\
		portID = argPortID;\
		oscNode = argOSCNode;\
		\cf4 this\cf3 .addOpenPort;\
		\cf4 this\cf3 .prInit\
	\}\
	prInit \{\
		\cf4 _OSCInPort_Init\cf3 \
		^\cf4 this\cf3 .primitiveFailed;\
	\}\
\}\
\
\
\cf4 OSCOutPort\cf3  : \cf4 OSCPort\cf3  \{\
	\cf2 // UDP port for outgoing OSC messages\
\cf3 	\cf4 var\cf3  <hostname;\
	\
	*new \{ \cf4 arg\cf3  portID, hostname, oscNode;\
		^\cf4 super\cf3 .new.init(portID, hostname, oscNode)\
	\}\
	\
	send \{ \cf4 arg\cf3  oscAddress ... args;\
		\cf4 _OSCOutPort_Send\cf3 \
		^\cf4 this\cf3 .primitiveFailed;\
	\}\
	sendNoTags \{ \cf4 arg\cf3  oscAddress ... args;\
		\cf4 _OSCOutPort_SendNoTags\cf3 \
		^\cf4 this\cf3 .primitiveFailed;\
	\}\
	sendRaw \{ \cf4 arg\cf3  rawArray;\
		\cf4 _OSCOutPort_SendRaw\cf3 \
		^\cf4 this\cf3 .primitiveFailed;\
	\}\
	\
	sendArray \{ \cf4 arg\cf3  array;\
		\cf4 this\cf3 .performList(\cf5 \\send\cf3 , array);\
	\}\
	\
	\cf2 // PRIVATE\
\cf3 	init \{ \cf4 arg\cf3  argPortID, argHostname, argOSCNode;\
		portID = argPortID;\
		hostname = argHostname;\
		oscNode = argOSCNode;\
		\cf4 this\cf3 .addOpenPort;\
		\cf4 this\cf3 .prInit\
	\}\
	prInit \{\
		\cf4 _OSCOutPort_Init\cf3 \
		^\cf4 this\cf3 .primitiveFailed;\
	\}\
\}\
\
}
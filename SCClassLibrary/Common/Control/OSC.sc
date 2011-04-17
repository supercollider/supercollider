// this is probably not working anymore. see NetAddr.sc instead for the newer way.

/*
// Open Sound Control support

OSCNode {
	classvar <>sc; // this contains the "/sc" default behaviour for all input ports.

	// a not-very-secure password for access to the compiler
	// setting password to a Symbol opens SC to potential hacking
	classvar <>password;

	var <name, <>children, <>function, <>value, <port;

	*new { arg name, function, children, value;
		^super.newCopyArgs(name, children, function ? { arg node, x; x }, value);
	}
	*tree { arg children, argName, argValue;
		children = children.collect({ arg subtree;
			var name, thing, value;
			#name, thing, value = subtree;
			if (thing.isArray, {
				this.tree(thing, name, value);
			},{
				OSCNode(name, thing, nil, value);
			});
		});
		^OSCNode(argName, nil, children, argValue);
	}
	add { arg child;
		if (children.isNil, { children = Array.new(4) });
		children = children.add(child);
	}
	remove { arg child;
		if (children.notNil, { children.take(child); });
	}
	empty {
		children = nil;
	}
	name_ { arg theName; name = theName.asSymbol; }

	// these are for send and reply on the same machine.
	send { arg oscAddress ... args;
		_OSCNode_Send
		^this.primitiveFailed;
	}
	reply { arg oscAddress ... args;
		_OSCNode_Send
		^this.primitiveFailed;
	}

	*initClass {
		// Build the default behaviour for all input ports.
		// You might want to comment some or all of this out for installations
		// so you can't be hacked..
		sc = this.tree([
			[\interpreter, [
				// Warning: the 'run' method makes SC vulnerable executing any
				// code sent to it over the net. If you don't want to worry about it
				// then never set the password and you'll be safe.
				[\run, { arg node, msgPassword, string;
					if (password.isKindOf(Symbol) and: { password === msgPassword },
					{
						string.interpret; // a powerful tool for both Good and Evil...
					})
				}],
				[\set, { arg node, varName, value; // set one of the variables a-z in the interpreter
					thisProcess.interpreter.perform(varName.asSetter, value);
				}],
				[\clearAll, { thisProcess.interpreter.clearAll; }],
				[\executeFile, { arg node, pathname; thisProcess.interpreter.executeFile(pathname); }]
			]],

			[\library, [
				[\choose, { arg node, names; Library.prLibMenuChoose(names); }],
				[\perform, { arg node, names, selector ... args;
					var obj;
					obj = Library.at(names);
					if (obj.notNil, {
						obj.performList(selector, args);
					});
				}]
			]],

			[\obj, [
				[\perform, { arg node, id, selector ... args;
					var obj;
					obj = ObjectTable.at(id);
					if (obj.notNil, {
						obj.performList(selector, args);
					});
				}],

				// a function cannot be sent via the OSC, so 'put' only works using
				// inter-VM-messaging on the same machine.
				[\put, { arg node, id, function; ObjectTable.put(id, function.value); }],

				[\remove, { arg node, id; ObjectTable.remove(id); }]
			]],
			[\sound, [
				[\play, { arg node, pathName, duration, loop=false;
					SoundFile.play(pathName, duration: duration, loop: loop);
				}]
			]],
			[\mixer, [
				[\stop, { arg node, channel;
					MixerPlayer.forInput(channel, { arg input;
						input.killBox.value = 0;
						input.killBox.doAction;
					});
				}],
				[\copy, { arg node, channel;
					MixerPlayer.forInput(channel, { arg input;
						input.copyBtn.doAction;
					});
				}],
				[\volume, { arg node, channel, volume = 0.0;
					MixerPlayer.forInput(channel, { arg input;
						input.volumeSlider.value = volume;
					});
				}],
				[\close, { arg node;
					MixerPlayer.mixer.notNil.if({
						MixerPlayer.mixer.close;
					});
				}]
			]],
			[\ping, { arg node; node.port.reply("/sc/postln", "I'm listening") }],
			[\run, { thisProcess.run }],
			[\stop, { Synth.stop }],
			[\post, { arg node ... args; args.do({ arg item; item.post; " ".post; }); "\n".post; }],
			[\postln, { arg node ... args; args.do({ arg item; item.postln; });}]
		]);
	}

	// PRIVATE
	call { arg rcvPort ... args;
		port = rcvPort;
		value = function.valueArray(this, args);
		port = nil;
	}
}

////////////////////////////

// UDP support

OSCPort {
	classvar <openPorts; // keep track of all open ports
	var <portID, udpPort, <>oscNode;

	*closeAll {
		if (openPorts.notNil, {
			openPorts.copy.do({ arg port; port.close; });
		});
	}
	close {
		openPorts.remove(this);
		this.prClose;
	}

	// PRIVATE
	prClose {
		_OSCPort_Close
		^this.primitiveFailed;
	}
	addOpenPort {
		if (openPorts.isNil, { openPorts = Array.new(4); });
		openPorts = openPorts.add(this);
	}
}

OSCInPort : OSCPort {
	// UDP port for incoming OSC messages

	*new { arg portID, oscNode;
		^super.new.init(portID, oscNode)
	}
	reply { arg oscAddress ... args;
		_OSCInPort_Reply
		^this.primitiveFailed;
	}

	// PRIVATE
	init { arg argPortID, argOSCNode;
		portID = argPortID;
		oscNode = argOSCNode;
		this.addOpenPort;
		this.prInit
	}
	prInit {
		_OSCInPort_Init
		^this.primitiveFailed;
	}
}


OSCOutPort : OSCPort {
	// UDP port for outgoing OSC messages
	var <hostname;

	*new { arg portID, hostname, oscNode;
		^super.new.init(portID, hostname, oscNode)
	}

	send { arg oscAddress ... args;
		_OSCOutPort_Send
		^this.primitiveFailed;
	}
	sendNoTags { arg oscAddress ... args;
		_OSCOutPort_SendNoTags
		^this.primitiveFailed;
	}
	sendRaw { arg rawArray;
		_OSCOutPort_SendRaw
		^this.primitiveFailed;
	}

	sendArray { arg array;
		this.performList(\send, array);
	}

	// PRIVATE
	init { arg argPortID, argHostname, argOSCNode;
		portID = argPortID;
		hostname = argHostname;
		oscNode = argOSCNode;
		this.addOpenPort;
		this.prInit
	}
	prInit {
		_OSCOutPort_Init
		^this.primitiveFailed;
	}
}
*/

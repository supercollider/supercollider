


EnvirDispatch {

	var <>envir, <>public=true;
	
	value { arg key, obj;
		if(this.sendsTo(key))
		{
			if(obj.isKindOf(Function) and: { obj.isClosed.not}) {
				(Char.bullet ++ " only closed functions can be sent").postln;
				^this
			};
			
			this.dispatch(key, obj) 
		}
	}
	sendsTo { ^public }
	
	dispatch { ^this.subclassResponsibility(thisMethod) }
}




Public : EnvirDispatch {

	var <>sendingKeys, <>listeningKeys;
	var <>addresses, <>channel, <>nickname;
	var <>action, <>logSelf=true, <logAll=false, <>basicSafety=true;
	classvar <listeningSpaces, <resp;
	
	
	
	join { arg channelName, nick=\anybody; 
		channel = channelName;
		nickname = nick;
		if(listeningSpaces.isNil or: { listeningSpaces.includes(this).not }) {
			listeningSpaces = listeningSpaces.add(this)
		};
		("\nwelcome to #" ++ channel ++ "."+"\nplease respect the privacy of others.\n\n").postln;
	}
	
	leave { 
		channel = nil;
		listeningSpaces.remove(this);
	}
	
	
	
	lurk { listeningKeys = \all; sendingKeys = nil; }
	boss { listeningKeys = nil; sendingKeys = \all; }
	merge {   listeningKeys = \all; sendingKeys = \all; }
	
	 
	logAll_ { arg flag; if(flag) {
						if(sendingKeys === \all) { logAll = true } 
						{ "please set sendingKeys to \\all first".postln }
					} { logAll = false }
	}
	 
	
	makeLogWindow { arg bounds, color;
	 	var d; 
	 	d = Document((envir.tryPerform(\name) ? "log").asString);
	 	d.bounds_(bounds ? Rect(10, 400, 600, 500));
	 	d.background_(color ? Color.new255(180, 160, 180));
	 	d.string_("//" + Date.getDate.asString ++ "\n\n\n");
	 	action = { arg disp, str, key, nickname;
	 		defer { 
	 			d.selectRange(d.text.size-1, 0); // deselect user
	 			str = "~" ++ key ++ " = " ++ str;
	 			if(str.last !== $;) { str = str ++ $; };
	 			d.selectedString_(
	 				"\n" ++ "//" + nickname
	 				+ "______"
	 				+ Date.getDate.hourStamp 
	 				+ "________________________________________________\n\n"
	 				++ str
	 				++ "\n\n"
	 			);
	 			d.selectRange(d.text.size-1, 0)
	 		};
	 	};
	 	d.onClose = { action = nil }
	 }
	 
	 *getListening { arg channelName, nickname;
	 	^listeningSpaces.select { |p| 
	 		p.channel === channelName
	 		and:
	 		{ p.nickname !== nickname }  // don't listen to myself
	 	};
	 }
	 
	 *startListen { arg addr; // normally nil
		resp.remove;
		resp = OSCresponderNode(addr, '/public', { arg time, resp, msg;
			var channel, nickname, key, str, dispatchers, listeningKeys;
			#nickname, channel, key, str = msg[1..4];
			dispatchers = this.getListening(channel, nickname);
			dispatchers.do { arg dispatcher;
				if(dispatcher.listensTo(key))
				 	{ dispatcher.receive(str, key, nickname, channel) }
					{
						if(dispatcher.logAll) {
							dispatcher.action.value(dispatcher, nickname, key, str) 
						} 
					}
			}
		
		});
		resp.add;

	}
	
	receive { arg str, key ... args;
			var obj;
			str = str.asString;
			if(this.avoidTheWorst(str)) { 
				obj = str.interpret;
				action.valueArray(this, str, key, args);
				[\receive, this, str, key, args].postcs;
				if(obj.notNil) {
					this.localPut(key, obj);
				};
			}
	}
	
	
	*stopListen {
		resp.remove;
		resp = nil;
	}
	
		
	// private implementation //
	
	
	
	localPut { arg key, obj;
		if(currentEnvironment === envir) { // speed optimization
				envir.localPut(key, obj);
		} {
			envir.use {
				envir.localPut(key, obj);
			}
		}
	}
	
	dispatch { arg key, obj;
		var str, b;
		str = obj.asCompileString;
		if(logSelf) { action.value(this, str, key, nickname) };
		if(str.size > 8125) { "string too large to publish".postln; ^this };
		if(channel.isNil or: { nickname.isNil }) { Error("first join a channel, please").throw };
		b = ['/public', nickname, channel, key, str];
		addresses.do { arg addr; addr.sendBundle(nil, b) };
	}
	
	// in future: use a function to allow more flexibility.
	
	sendsTo { arg key;
		^public and: { sendingKeys.notNil } and: 
			{ 
				sendingKeys === \all 
				or: 
				{ sendingKeys.includes(key) } 
			} 
	}
	listensTo { arg key;
		^public and: { listeningKeys.notNil } and: 
			{
				listeningKeys === \all 
				or:
				{ listeningKeys.includes(key) }
			}
	}
	
	avoidTheWorst { arg str;
		^(basicSafety.not) or: {
			str.find("unixCmd").isNil 
			and: { str.find("File").isNil } 
			and: { str.find("Pipe").isNil }
			and: { str.find("Public").isNil }
		}
	}
	
}


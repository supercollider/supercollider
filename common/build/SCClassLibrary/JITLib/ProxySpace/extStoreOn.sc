+ Server { 
	storeOn { arg stream;
		var codeStr = this.switch (
			thisProcess.interpreter.s, 	{ "s" }, 					Server.default, 			{ "Server.default" },
			Server.local,				{ "Server.local" },
			Server.internal,			{ "Server.internal" },
			{ "Server.fromName(" + name.asCompileString + ")" }
		);
			stream << codeStr;
	}
}

+ AbstractPlayControl {

	storeOn { | stream | 
		source.storeOn(stream) 
	}
}

+Symbol {

	isBasicOperator {
		^#['+', '-', '*', '/', '%', '==', '!=', '<', '<=', '>', '>=', '&&', '||', '@' ]
			.includes(this);
	}

}

+Object {

	// might need correction for literals.
	envirKey { | envir |
		^(envir ? currentEnvironment).findKeyForValue(this)
	}
	
	envirCompileString {
		var key = this.envirKey;
		^if(key.notNil) { "~" ++ key } { this.asCompileString };
	}
}


+NodeProxy {

	key { | envir |
		^super.envirKey(envir);
	}
	
	servStr { 
		^if (server != Server.default) { "(" ++ server.asCompileString ++")" } { "" }
	}
	
	// not ideal, but usable for now.
	storeOn { | stream | 
		var key = this.key; 
		
		if (currentEnvironment.includes(this)) {
			stream << ("~" ++ key)
		} { 
			if (key.isNil) { 
				stream << "a = NodeProxy.new" ++ this.servStr
			}
		};
	}

	playNDialog { | bounds |
		var editstring, doc;
		bounds = bounds ?? { Rect(0, 500, 320, 100) };
		editstring = this.asCompileString ++ ".playN(\n"
			++ "\touts:" + try { this.monitor.outs } ?? { (0..this.numChannels - 1) }  ++ ",\n"
			++ "\tamps:" + try { this.monitor.amps } ?? { 1 ! this.numChannels }  ++ ",\n"
			++ "\tvol:" + try { this.monitor.vol } ? 1 ++ "\n);";

				// does this run on windows? 
		doc = Document("edit outs:", editstring); 
		try { doc.bounds_(bounds) };	// swingosc safe
	}

	findInOpenDocuments { |index = 0|
		var src, str, startSel, doc;
		src = this.at(index);
		src ?? { "NodeProxy: no source at index %.\n".postf(index); ^this };

		str = src.asCompileString;
		doc = Document.allDocuments.detect { |doc|
			startSel = doc.string.find(str);
			startSel.notNil;
		};
		doc !? { doc.front.selectRange(startSel, 0); }
	}

	document { | includeSettings = true, includeMonitor = true |
		var nameStr, srcStr, str, docStr, indexStr, key; 
		var space, spaceCS;
		
		var isAnon, isSingle, isInCurrent, isOnDefault, isMultiline; 
		
		nameStr = this.asCompileString; 
		indexStr = nameStr;

		isAnon = nameStr.beginsWith("a = ");
		isSingle = this.objects.isEmpty or: { this.objects.size == 1 and: { this.objects.indices.first == 0 } };
		isInCurrent = currentEnvironment.includes(this); 
		isOnDefault = server === Server.default;

		[\isAnon, isAnon, \isSingle, isSingle, \isInCurrent, isInCurrent, \isOnDefault, isOnDefault].postln;

		space = ProxySpace.findSpace(this); 
		spaceCS = try { space.asCode } { 
			inform("// <could not find a space for proxy: %!>".format(this.asCompileString));
			""
		};
		
		docStr = String.streamContents { arg stream;
			if(isSingle) { 
				str = nameStr; 
				srcStr = if (this.source.notNil) { this.source.envirCompileString } { "" };
				
				if ( isAnon ) {			// "a = NodeProxy.new"
					if (isOnDefault.not) { str = str ++ "(" ++ this.server.asCompileString ++ ")" };
					if (srcStr.notEmpty) { str = str ++ ".source_(" ++ srcStr ++ ")" }; 
				} {	
					if (isInCurrent) { 	// ~out 
						if (srcStr.notEmpty) { str = str + "=" + srcStr }; 
						
					} { 					// Ndef('a') - put sourceString before closing paren.
						if (srcStr.postcs.notEmpty) { 
							str = str.copy.postcs.drop(-1) ++ ", " ++ srcStr ++ nameStr.last 
						}; 
					}
				};
				str.postcs;
			} {	
				// multiple sources
				if (isAnon) { 
					str = nameStr ++ ";\n";
					indexStr = "a"; 
				}; 
				
				this.objects.keysValuesDo { arg index, item;

					srcStr = item.source.envirCompileString ? "";
					isMultiline = srcStr.includes(Char.nl);
					if (isMultiline) { srcStr = "(" ++ srcStr ++ ")" };
					srcStr = indexStr ++ "[" ++ index ++ "] = " ++ srcStr ++ ";\n";
					str = str ++ srcStr;
				};
			};
			str.postcs; 
			if (str.last != $;) { stream << str << ";\n" } { stream << str << "\n" }; 
			
			
			// add settings to compile string
			if(includeSettings) {
					this.nodeMap.storeOn(stream, indexStr, true);
			};
//				// not done yet! 
//				// Monitor should know its outs, ins, amps, play or playN and 
//				// whether it was playing or not. 
//			if (includeMonitor) { 
//				if (this.monitor.notNil) { 
//					this.monitor.storeOn(stream)
//				};
//			};
		};
		
		docStr.postcs;
		isMultiline = docStr.drop(-1).includes(Char.nl); 
		if (isMultiline) { docStr = "(\n" ++ docStr ++ ");" };
		
		^Document.new(("document :" + nameStr).asString, docStr)
	}
	
}


+BinaryOpPlug {

	envirCompileString {
		var astr, bstr, opstr, str = "";
		var basic = operator.isBasicOperator;
		astr = a.envirCompileString;
		bstr = b.envirCompileString;

		if(b.isKindOf(AbstractOpPlug)) { bstr = "(%)".format(bstr) };
		opstr = if(basic.not) { ".%(" } { " % " }.format(operator);
		str = str ++ astr ++ opstr ++ bstr;
		if(basic.not) { str = str ++ ")" };
		^str
	}
}

+UnaryOpPlug {

	envirCompileString {
		^(a.envirCompileString ? "") ++  " "  ++ operator
	}
	
}



+ ProxySpace {

			// where am I globally accessible?
	asCode { 
		var key;
		if (this == thisProcess.interpreter.p) { ^"p" };
		if (this == currentEnvironment) { ^"currentEnvironment" };
		if (Ndef.all.includes(this)) { 
			key = Ndef.all.findKeyForValue(this); 
			^"Ndef.all[%]".format(key.asCompileString); 
		}; 
		if (ProxySpace.all.includes(this)) { 
			key = ProxySpace.all.findKeyForValue(this); 
			^"ProxySpace.all[%]".format(key.asCompileString); 
		}; 

		^"/***( cannot locate this proxyspace )***/"
	}
			// not working again yet - should become much simpler. 
	storeOn { | stream, keys, includeSettings = true |
		var proxies, hasGlobalClock;
		hasGlobalClock = clock.isKindOf(TempoBusClock);
		if(hasGlobalClock) { stream <<< this.asCode << ".makeTempoClock(" << clock.tempo << ");\n\n"; };
		// find keys for all parents
		if(keys.notNil) {
			proxies = IdentitySet.new;
			keys.do { arg key; var p = envir[key]; p !? { p.getFamily(proxies) } };
			keys = proxies.collect { arg item; item.key(envir) };
		} { keys = envir.keys };
//
//		if(hasGlobalClock) { keys.remove(\tempo) };
//		// add all objects to compilestring
//		keys.do { arg key;
//			var proxy, str, isMultiline;
//			proxy = envir.at(key);
//			if(proxy.objects.size == 1 and: { proxy.objects.indices.first == 0 }) {
//				str = proxy.source.envirCompileString ? "";
//				isMultiline = str.includes(Char.nl);
//				if(isMultiline){ stream << "(" << Char.nl };
//				stream <<< proxy << " = ";
//				str.printOn(stream);
//				stream << ";";
//				if(isMultiline){ stream << Char.nl << ");" << Char.nl };
//			} {
//				proxy.objects.keysValuesDo({ arg index, item;
//					var isMultiline, str;
//					str = item.source.envirCompileString ? "";
//					isMultiline = str.includes(Char.nl);
//					if(isMultiline){ stream << "(" << Char.nl };
//					stream <<< proxy << "[" << index << "] = ";
//					str.printOn(stream);
//					stream << ";";
//					if(isMultiline){ stream << Char.nl << ");" << Char.nl };
//					stream.nl;
//				});
//			};
//			stream.nl;
//		// add settings to compile string
//			if(includeSettings) {
//				proxy.nodeMap.storeOn(stream, proxy.as, true, envir);
//			};
//			stream.nl;
//		}
	}
	
	documentOutput {
		^this.document(nil, true)
	}

	document { | keys, onlyAudibleOutput = false, includeSettings = true |
		var str;
		if(onlyAudibleOutput) {
			keys = this.monitors.collect { arg item; item.key(envir) };
		};
		str = String.streamContents { arg stream; 
			stream << "// ( p = ProxySpace.new(s).push; ) \n\n";
			this.storeOn(stream, keys, includeSettings);
			this.do { arg px; if(px.monitorGroup.isPlaying) {
				stream << px.playNDialog << ".play; \n"
				}
			};
		};
		^Document.new((name ? "proxyspace").asString, str)
	}

}

+ ProxyNodeMap {

	storeOn { | stream, namestring = "", dropOut = false |
			var strippedSetArgs, storedSetNArgs, rates, proxyMapKeys, proxyMapNKeys;
			this.updateBundle;
			if(dropOut) {
				forBy(0, setArgs.size - 1, 2, { arg i;
					var item;
					item = setArgs[i];
					if(item !== 'out' and: { item !== 'i_out' })
					{
						strippedSetArgs = strippedSetArgs.add(item);
						strippedSetArgs = strippedSetArgs.add(setArgs[i+1]);
					}
				})
			} { strippedSetArgs = setArgs };
			if(strippedSetArgs.notNil) {
				stream << namestring << ".set(" <<<* strippedSetArgs << ");" << Char.nl;
			};
			if(mapArgs.notNil or: { mapnArgs.notNil }) {
				settings.keysValuesDo { arg key, setting;
					var proxy;
					if(setting.isMapped) {
						proxy = setting.value;
						if(proxy.notNil) {
							if(setting.isMultiChannel) {
								proxyMapNKeys = proxyMapNKeys.add(key);
								proxyMapNKeys = proxyMapNKeys.add(proxy);
							}{
								proxyMapKeys = proxyMapKeys.add(key);
								proxyMapKeys = proxyMapKeys.add(proxy);
							}
						};
					};
				};
				if(proxyMapKeys.notNil) {
					stream << namestring << ".map(" <<<* proxyMapKeys << ");" << Char.nl;
				};
				if(proxyMapNKeys.notNil) {
					stream << namestring << ".mapn(" <<<* proxyMapNKeys << ");" << Char.nl;
				};
			};

			if(setnArgs.notNil) {
				storedSetNArgs = Array.new;
				settings.keysValuesDo { arg key, setting;
					if(setting.isMapped.not and: setting.isMultiChannel) {
						storedSetNArgs = storedSetNArgs.add(key);
						storedSetNArgs = storedSetNArgs.add(setting.value);
					}
				};
				stream << namestring << ".setn(" <<<* storedSetNArgs << ");" << Char.nl;
			};
			settings.keysValuesDo { arg key, setting;
				if(setting.rate.notNil) { rates = rates.add(key); rates = rates.add(setting.rate) };
			};
			if(rates.notNil) {
				stream << namestring << ".setRates(" <<<* rates << ");" << Char.nl;
			}

		}

}

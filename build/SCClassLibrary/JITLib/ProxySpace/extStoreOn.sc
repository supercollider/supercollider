

+ AbstractPlayControl {
	storeOn { arg stream; source.storeOn(stream) }
}

+Symbol {
	isBasicOperator {
		^#['+', '-', '*', '/', '%', '==', '!=', '<', '<=', '>', '>=', '&&', '||', '@' ].includes(this);	
	}
	
}

+Object {
	// might need correction for literals.
	envirKey { arg envir; ^(envir ? currentEnvironment).findKeyForValue(this) } 
	envirCompileString {
		var key = this.envirKey;
		^if(key.notNil) { "~" ++ key } { this.asCompileString };
	}
}


+NodeProxy {
	key { arg envir; 
		^super.envirKey(envir);
		//don't want to add a slot yet. not optimized
	}
	
	playNDialog { |bounds|		var editstring;
		bounds = bounds ?? { Rect(0, 500, 320, 100) };
		editstring = "~" ++ this.key ++ ".playN(\n" 
			++ "\touts:" + try { this.monitor.outs } ?? { (0..this.numChannels - 1) }  ++ ",\n"
			++ "\tamps:" + try { this.monitor.amps } ?? { 1 ! this.numChannels }  ++ ",\n"
			++ "\tvol:" + try { this.monitor.vol } ? 1 ++ "\n);"; 
			
		^Document("edit outs:", editstring).bounds_(bounds);	}}


+BinaryOpPlug {
	envirCompileString {
		var astr, bstr, opstr, str = "";
		var basic = operator.isBasicOperator;
		astr = a.envirCompileString;
		bstr = b.envirCompileString;
		
		if(b.isKindOf(AbstractOpPlug)){ bstr = "(%)".format(bstr) };
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
	
	storeOn { arg stream, keys;
		var proxies, hasGlobalClock;
		hasGlobalClock = clock.isKindOf(TempoBusClock);
		if(hasGlobalClock) { stream << "p.makeTempoClock(" << clock.tempo << ");\n\n"; };
		// find keys for all parents
		if(keys.notNil) {
			proxies = IdentitySet.new;
			keys.do { arg key; var p = envir[key]; p !? { p.getFamily(proxies) } };
			keys = proxies.collect { arg item; item.key(envir) }; 
		} { keys = envir.keys };
		
		if(hasGlobalClock) { keys.remove(\tempo) };
		// add all objects to compilestring
		keys.do { arg key; 
			var proxy, str, multiline;
			proxy = envir.at(key);
			if(proxy.objects.size == 1 and: { proxy.objects.indices.first == 0 }) {
				str = proxy.source.envirCompileString ? "";
				multiline = str.includes(Char.nl);
				if(multiline){ stream << "(" << Char.nl };
				stream << "~" << key << " = ";
				str.printOn(stream);
				stream << ";";
				if(multiline){ stream << Char.nl << ");" << Char.nl };
			} {
				proxy.objects.keysValuesDo({ arg index, item;
					var multiline, str;
					str = item.source.envirCompileString ? "";
					multiline = str.includes(Char.nl);
					if(multiline){ stream << "(" << Char.nl };
					stream << "~" << key << "[" << index << "] = ";
					str.printOn(stream);
					stream << ";";
					if(multiline){ stream << Char.nl << ");" << Char.nl };
					stream.nl;
				});
			};
			stream.nl;
		// add settings to compile string
			proxy.nodeMap.storeOn(stream, "~" ++ key, true, envir);
			stream.nl;
		}
	}
	documentOutput {
		^this.document(nil, true)
	}
	document { arg keys, onlyOutput=false; // if true only audible content is documented.
		var str;
		if(onlyOutput) {
			keys = this.monitors.collect { arg item; item.key(envir) };
		};
		str = String.streamContents({ arg stream; 
			stream << "// ( p = ProxySpace.new(s).push; ) \n\n";
			this.storeOn(stream, keys);
			this.do { arg px; if(px.monitorGroup.isPlaying) { 
				stream << "~" << px.key << ".play; \n"
				}
			};
		});
		^Document.new((name ? "proxyspace").asString, str)
	}
	
}

+ ProxyNodeMap {
	storeOn { arg stream, namestring="", dropOut=false, envir;
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
			if(mapArgs.notNil or: { mapnArgs.notNil } and: { envir.notNil }) {
				settings.keysValuesDo { arg key, s; 
					var proxyKey;
					if(s.isMapped) { 
						proxyKey = s.value.key;
						if(proxyKey.notNil) {
							if(s.isMultiChannel) {
								proxyMapNKeys = proxyMapNKeys.add("'" ++ key ++ "'"); 
								proxyMapNKeys = proxyMapNKeys.add("~" ++ proxyKey);
							}{
								proxyMapKeys = proxyMapKeys.add("'" ++ key ++ "'"); 
								proxyMapKeys = proxyMapKeys.add("~" ++ proxyKey);
							}
						};
					};
				};
				if(proxyMapKeys.notNil) {
					stream << namestring << ".map(" <<* proxyMapKeys << ");" << Char.nl;
				};
				if(proxyMapNKeys.notNil) {
					stream << namestring << ".mapn(" <<* proxyMapNKeys << ");" << Char.nl;
				};
			};
	
			if(setnArgs.notNil) {
				storedSetNArgs = Array.new;
				settings.keysValuesDo { arg key, s;
					if(s.isMapped.not and: s.isMultiChannel) {
						storedSetNArgs = storedSetNArgs.add(key);
						storedSetNArgs = storedSetNArgs.add(s.value);
					}
				};
				stream << namestring << ".setn(" <<<* storedSetNArgs << ");" << Char.nl;
			};
			settings.keysValuesDo { arg key, s;
				if(s.rate.notNil) { rates = rates.add(key); rates = rates.add(s.rate) };
			};
			if(rates.notNil) {
				stream << namestring << ".setRates(" <<<* rates << ");" << Char.nl;
			}
			
		}

}
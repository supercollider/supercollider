

+ AbstractPlayControl {
	storeOn { arg stream; source.storeOn(stream) }
}

+Symbol {
	isBasicOperator {
		^#['+', '-', '*', '/', '%', '==', '!=', '<', '<=', '>', '>=', '&&', '||', '@' ].includes(this);	
	}
	
}


+NodeProxy {
	key { arg envir;
		^(envir ? currentEnvironment).findKeyForValue(this) 
		//don't want to add a slot yet. not optimized
	}
	storeOn { arg stream;
		var key;
		key = this.key;
		if(key.notNil) {Êstream << "~" << key } { super.storeOn(stream) };
	}
}

+BinaryOpPlug {

	storeOn { arg stream;
		var astr, bstr, opstr;
		astr = if(a.isKindOf(NodeProxy))
			{ "~" ++ a.key } { a.asCompileString };
		bstr = if(b.isKindOf(NodeProxy))
			{ "~" ++ b.key } { b.asCompileString };
		if(b.isKindOf(AbstractOpPlug))Ê{ bstr = "(" ++ bstr ++ ")" };
		opstr = if(operator.isBasicOperator.not) 
			{ "." ++ operator ++ "(" } { " " ++ operator ++ " " };
		stream << astr  << opstr  << bstr;
		if(operator.isBasicOperator.not) { stream << ")" };
	}
}

+UnaryOpPlug {
	storeOn { arg stream; 
		var astr;
		astr = if(a.isKindOf(NodeProxy))
			{ "~" ++ a.key } { a.asCompileString };
			stream << astr << " " << operator
	 }
}


+ ProxySpace {
	
	storeOn { arg stream, keys;
		var proxies, hasGlobalClock;
		hasGlobalClock = clock.isKindOf(TempoBusClock);
		if(hasGlobalClock) { stream << "p.makeTempoClock(" << clock.tempo << ");"; stream.nl.nl };
		// find keys for all parents
		if(keys.notNil) {
			proxies = IdentitySet.new;
			keys.do { arg key; envir[key].getFamily(proxies) };
			keys = proxies.collect { arg item; item.key(envir) }; 
		} { keys = envir.keys };
		// if(hasGlobalClock) { keys.remove(\tempo) };
		// add all objects to compilestring
		keys.do { arg key; 
			var proxy, str, multiline;
			proxy = envir.at(key);
			if(proxy.objects.size == 1) {
				str = proxy[0].source.asCompileString ? "";
				multiline = str.includes(Char.nl);
				if(multiline)Ê{ stream << "(" << Char.nl };
				stream << "~" << key << " = ";
				str.printOn(stream);
				stream << ";";
				if(multiline)Ê{ stream << Char.nl << ");" << Char.nl };
			} {
				proxy.objects.keysValuesDo({ arg index, item;
					var multiline, str;
					str = item.source.asCompileString ? "";
					multiline = str.includes(Char.nl);
					if(multiline)Ê{ stream << "(" << Char.nl };
					stream << "~" << key << "[" << index << "] = ";
					str.printOn(stream);
					stream << ";";
					if(multiline)Ê{ stream << Char.nl << ");" << Char.nl };
					stream.nl;
				});
			};
			stream.nl;
		// add settings to compile string
			proxy.nodeMap.storeOn(stream, "~" ++ key, true, envir);
		// add task to compile string
			if(proxy.task.notNil) { 
				stream.nl << "~" << key << ".task = " << 
				"Task(" <<< proxy.task.originalStream.func << ")" << ";" 
			};
			stream.nl;
		}
	}
	
	document { arg keys; // use -1 as flag so only audible content is documented.
		var str;
		if(keys == -1) {
			keys = this.monitors.collect { arg item; item.key(envir) };
		};
		str = String.streamContents({ arg stream; 
			stream << "p = ProxySpace.push(s);";
			stream.nl.nl;
			this.storeOn(stream, keys); 
		});
		^Document.new((name ? "proxyspace").asString, str)
	}
	
}

+ ProxyNodeMap {
	storeOn { arg stream, namestring="", dropOut=false, envir;
			var strippedSetArgs, rates, proxyMapKeys;
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
			if(mapArgs.notNil && envir.notNil) {
				settings.keysValuesDo { arg key, s; 
					var proxyKey;
					if(s.bus.notNil) { 
						proxyKey = s.bus.key;
						if(proxyKey.notNil) {
							proxyMapKeys = proxyMapKeys.add("'" ++ key ++ "'"); 
							proxyMapKeys = proxyMapKeys.add("~" ++ proxyKey);
						};
					};
				};
				if(proxyMapKeys.notNil) {
					stream << namestring << ".map(" <<* proxyMapKeys << ");" << Char.nl;
				};
			};
	
			if(setnArgs.notNil) {
				stream << namestring << ".setn(" <<<* setnArgs << ");" << Char.nl;
			};
			settings.keysValuesDo { arg key, s;
				if(s.rate.notNil) { rates = rates.add(key); rates = rates.add(s.rate) };
			};
			if(rates.notNil) {
				stream << namestring << ".setRates(" <<<* rates << ");" << Char.nl;
			}
			
		}

}
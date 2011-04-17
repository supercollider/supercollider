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

	playEditString { |usePlayN, dropDefaults = false, nameStr|
		var editString, outs, amps;
		nameStr = nameStr ?? { this.asCompileString };

		if (nameStr.beginsWith("a = ")) { // anon proxy
			nameStr = nameStr.keep(1);
		};
		usePlayN = usePlayN ?? { if (monitor.notNil) { monitor.usedPlayN } ? false };

		// if they are defaults, don't post them
		if (usePlayN) {
			editString = nameStr ++ this.playNString(dropDefaults)
		} {
			editString = nameStr ++ this.playString(dropDefaults)
		};
		^editString;
	}

	playString { |dropDefaults = false|
		var defOut = 0, defVol = 1, defNumCh = this.numChannels ? 2;
		var out, numCh, vol, setStr = "";

		out = try { this.monitor.out } ? defOut;
		numCh = this.numChannels ? defNumCh; 		// should be able to be different, or not?
		vol = try { this.monitor.vol } ? defVol;

		if (dropDefaults.not or: { out != defOut }) { setStr = setStr ++ "\tout:" + out };

		if (dropDefaults.not or: { numCh != defNumCh }) {
			if (setStr.size > 0) { setStr = setStr ++ ", \n" };
			setStr = setStr ++ "\tnumChannels:" + numCh;
		};

		if (dropDefaults.not or: { vol != defVol }) {
			if (setStr.size > 0) { setStr = setStr ++ ", \n" };
			setStr = setStr ++ "\tvol:" + vol ++ "\n";
		};
		if (setStr.size > 0) {
			setStr = "(\n" ++ setStr ++ "\n)";
		};
		^(".play" ++ setStr ++ ";\n");
	}

	playNString { |dropDefaults = false|
		var numCh =  this.numChannels ? 2;
		var defOuts = { |i| i } ! numCh;
		var defAmps = 1 ! numCh;
		var defIns = { |i| i + this.index } ! numCh;
		var defVol = 1;

		var outs = try { this.monitor.outs } ? defOuts;
		var amps = try { this.monitor.amps } ? defAmps;
		var ins  = try { this.monitor.ins }  ? defIns;
		var vol  = try { this.monitor.vol }  ? defVol;

		var setStr = "";

		// [\o, defOuts, outs, \a, defAmps, amps, \i, defIns, ins].postcs;

		if (dropDefaults.not or: { outs != defOuts }) {
			setStr = setStr ++ "\touts:" + outs
		};
		if (dropDefaults.not or: { amps != defAmps }) {
			if (setStr.size > 0) { setStr = setStr ++ ", \n" };
			setStr = setStr ++ "\tamps:" + amps;
		};
		if (dropDefaults.not or: { ins != defIns }) {
			if (setStr.size > 0) { setStr = setStr ++ ", \n" };
			setStr = setStr ++ "\tins:" + ins;
		};
		if (dropDefaults.not or: { vol != defVol }) {
			if (setStr.size > 0) { setStr = setStr ++ ", \n" };
			setStr = setStr ++ "\tvol:" + vol ++ "\n";
		};
		if (setStr.size > 0) {
			setStr = "(\n" ++ setStr ++ "\n)";
		};
		^(".playN" ++ setStr ++ ";\n");
	}

	playNDialog { | bounds, usePlayN |
		var doc = this.playEditString(usePlayN).newTextWindow("edit outs:");
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

	asCode { | includeSettings = true, includeMonitor = true, envir |
		var nameStr, srcStr, str, docStr, indexStr, key;
		var space, spaceCS;

		var isAnon, isSingle, isInCurrent, isOnDefault, isMultiline;

		envir = envir ? currentEnvironment;

		nameStr = envir.use { this.asCompileString };
		indexStr = nameStr;

		isAnon = nameStr.beginsWith("a = ");
		isSingle = this.objects.isEmpty or: { this.objects.size == 1 and: { this.objects.indices.first == 0 } };
		isInCurrent = envir.includes(this);
		isOnDefault = server === Server.default;

	//	[\isAnon, isAnon, \isSingle, isSingle, \isInCurrent, isInCurrent, \isOnDefault, isOnDefault].postln;

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
						if (srcStr.notEmpty) {
							str = str.copy.drop(-1) ++ ", " ++ srcStr ++ nameStr.last
						};
					}
				};
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

			stream << str << if (str.keep(-2).includes($;)) { "\n" } { ";\n" };

				// add settings to compile string
			if(includeSettings) {
					this.nodeMap.storeOn(stream, indexStr, true);
			};
				// include play settings if playing ...
				// hmmm - also keep them if not playing,
				// but inited to something non-default?
			if (this.rate == \audio and: includeMonitor) {
				if (this.monitor.notNil) {
					if (this.isMonitoring) {
						stream << this.playEditString(this.monitor.usedPlayN, true)
					}
				};
			};
		};

		isMultiline = docStr.drop(-1).includes(Char.nl);
		if (isMultiline) { docStr = "(\n" ++ docStr ++ ");\n" };

		^docStr
	}

	document { | includeSettings = true, includeMonitor = true |
		^this.asCode(includeSettings, includeMonitor).newTextWindow("document :" + this.asCompileString)
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

	storeOn { | stream, keys, includeSettings = true, includeMonitors = true |
		var proxies, hasGlobalClock;

		hasGlobalClock = clock.isKindOf(TempoBusClock);

		stream << "\n(\n"; // )
		if(hasGlobalClock) { stream <<< this.asCode << ".makeTempoClock(" << clock.tempo << ");\n\n"; };
		// find keys for all parents
		if(keys.notNil) {
			proxies = IdentitySet.new;
			keys.do { arg key; var p = envir[key]; p !? { p.getFamily(proxies) } };
			keys = proxies.collect { arg item; item.key(envir) };
		} { keys = envir.keys };

		if(hasGlobalClock) { keys.remove(\tempo) };

		// add all objects to compilestring
		keys.do { arg key;
			var proxy = envir.at(key);
			stream << proxy.asCode(includeSettings, includeMonitors, this.envir) << "\n";
		};

		stream << /*(*/ ");\n";
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
//			this.do { arg px; if(px.monitorGroup.isPlaying) {
//				stream << px.playEditString << ".play; \n"
//				}
//			};
		};
		^str.newTextWindow((name ? "proxyspace").asString)
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

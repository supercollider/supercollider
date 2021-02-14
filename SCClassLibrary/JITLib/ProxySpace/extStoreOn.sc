+ AbstractPlayControl {

	storeOn { | stream |
		source.storeOn(stream)
	}
}

+ Symbol {

	isBasicOperator {
		^#['+', '-', '*', '/', '%', '==', '!=', '<', '<=', '>', '>=', '&&', '||', '@' ].includes(this);
	}

}

+ Object {

	// might need correction for literals.
	envirKey { | envir |
		^(envir ? currentEnvironment).findKeyForValue(this)
	}

	envirCompileString {
		var key = this.envirKey;
		^if(key.notNil) { "~" ++ key } { this.asCompileString };
	}
}


+ NodeProxy {

	key { | envir |
		^super.envirKey(envir);
	}

	servStr {
		^if(server != Server.default) { "(" ++ server.asCompileString ++")" } { "" }
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
		var defFadeTime = 0.02 ! numCh;

		var outs = try { this.monitor.outs } ? defOuts;
		var amps = try { this.monitor.amps } ? defAmps;
		var ins  = try { this.monitor.ins }  ? defIns;
		var vol  = try { this.monitor.vol }  ? defVol;
		var fadeTime = try { this.monitor.fadeTime }  ? defFadeTime;

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
			setStr = setStr ++ "\tvol:" + vol;
		};
		if (dropDefaults.not or: { fadeTime != defFadeTime }) {
			if (setStr.size > 0) { setStr = setStr ++ ", \n" };
			setStr = setStr ++ "\tfadeTime:" + fadeTime;
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

	// asCode posts a proxy and its full state, so it can be recreated.
	// needed because asCompileString only posts access code to proxy or Ndef.
	asCode { | includeSettings = true, includeMonitor = true, envir |
		var nameStr, srcStr, str, docStr, accessStr = "a";
		var space, spaceCS;

		var isAnon, isSingle, isInCurrent, isOnDefault, isMultiline;

		envir = envir ? currentEnvironment;

		nameStr = envir.use { this.asCompileString };
		isAnon = nameStr.beginsWith("a = ");
		if (isAnon.not) { accessStr = nameStr };

		isSingle = this.objects.isEmpty or: { this.objects.size == 1 and: { this.objects.indices.first == 0 } };
		isInCurrent = envir.includes(this);
		isOnDefault = server === Server.default;

		space = ProxySpace.findSpace(this);
		spaceCS = try { space.asCode } {
			"// anonymous proxy posted - please change code to make it accessible:".postln
		};

		docStr = String.streamContents { |stream|
			// proxy has a single source
			if(isSingle) {
				str = nameStr;
				srcStr = if (this.source.notNil) { this.source.envirCompileString } { "" };

				if ( isAnon ) {			// "a = NodeProxy.new"
					if (isOnDefault.not) { str = str ++ "(" ++ this.server.asCompileString ++ ")" };
					if (srcStr.notEmpty) { str = str ++ ".source_(" ++ srcStr ++ ")" };
				} {
					// globally accessible, and we have a source to post
					if (srcStr.notEmpty) {
						if (this.isKindOf(Ndef)) {
							// basic Ndef('a') - put sourceString before closing paren.
							str = this.cs.drop(-1) ++ "," + srcStr ++ ")";
						} {
							// proxy must be in a pushed proxyspace:
							if (isInCurrent) {
								// basic nodeproxy: ~x = { ... }
								str = str + "=" + srcStr
							}
						}
					};
				};
			} {
				// proxy has multiple sources
				if (isAnon) {
					str = nameStr ++ ";\n";
				};

				this.objects.keysValuesDo { |index, item|

					srcStr = item.source.envirCompileString ? "";
					isMultiline = srcStr.includes(Char.nl);
					if (isMultiline) { srcStr = "(" ++ srcStr ++ ")" };
					srcStr = accessStr ++ "[" ++ index ++ "] = " ++ srcStr ++ ";\n";
					str = str ++ srcStr;
				};
			};

			stream << str << if (str.keep(-2).includes($;)) { "\n" } { ";\n" };

			// add settings to compile string
			if(includeSettings) {
				stream << this.nodeMap.asCode(accessStr, true);
			};
			// includes play settings if playing only.
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
		var nameStr = this.class.asString ++"_" ++ this.key;
		^this.asCode(includeSettings, includeMonitor)
		.newTextWindow("document-" ++ nameStr)
	}

}


+ BinaryOpPlug {

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

+ UnaryOpPlug {

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

		stream << "\n(\n";
		if(hasGlobalClock) { stream <<< this.asCode << ".makeTempoClock(" << clock.tempo << ");\n\n"; };
		// find keys for all parents
		if(keys.notNil) {
			proxies = IdentitySet.new;
			keys.do { |key| var p = envir[key]; p !? { p.getFamily(proxies) } };
			keys = proxies.collect { |item| item.key(envir) };
		} { keys = envir.keys };

		if(hasGlobalClock) { keys.remove(\tempo) };

		// add all objects to compilestring
		keys.do { |key|
			var proxy = envir.at(key);
			stream << proxy.asCode(includeSettings, includeMonitors, this.envir) << "\n";
		};

		stream << ");\n";
	}

	documentOutput {
		^this.document(nil, true)
	}

	document { | keys, onlyAudibleOutput = false, includeSettings = true |
		var str;
		if(onlyAudibleOutput) {
			keys = this.monitors.collect { |item| item.key(envir) };
		};
		str = String.streamContents { |stream|
			if (Ndef.all.includes(this)) {
				stream << ("// ( " + this.asCode + ") \n\n");
			} {
				stream << "// ( p = " << this.class.name << ".new(s).push; ) \n\n";
			};
			this.storeOn(stream, keys, includeSettings);
			//			this.do { |px| if(px.monitorGroup.isPlaying) {
			//				stream << px.playEditString << ".play; \n"
			//				}
			//			};
		};
		// ^str.newTextWindow((name ? "proxyspace").asString)
		^Document((name ? "proxyspace").asString, str);
	}

}


+ ProxyNodeMap {

	asCode { | namestring = "", dropOut = false |
		^String.streamContents({ |stream|
			var map, rate;
			if(dropOut) {
				map = this.copy;
				map.removeAt(\out);
				map.removeAt(\i_out);
			} { map = this };

			if (map[\fadeTime] == NodeProxy.defaultFadeTime) {
				map.removeAt(\fadeTime)
			};

			if(map.notEmpty) {
				// 'map' might refer to other NodeProxies
				// Before referring to them in an arg list,
				// we should be sure they are initialized
				// to the right rate and number of channels.
				// It is OK if the 'source' definition comes later in the doc string.
				map.keysValuesDo { |key, value|
					if(value.isKindOf(BusPlug)) {
						rate = value.rate;
						if(rate != \audio) { rate = \control };
						stream <<< value  // storeOn gets the ~key
						<< "." << UGen.methodSelectorForRate(rate)
						<< "(" << value.numChannels << ");\n";
					};
				};
				stream << namestring << ".set(" <<<* map.asKeyValuePairs << ");" << Char.nl;
			};
			if(rates.notNil) {
				stream << namestring << ".setRates(" <<<* rates << ");" << Char.nl;
			}
		});
	}
}

FreqScopeView {

	var <scope;
	var <scopebuf;
	var <server;
	var <active, <synth, <inBus, <dbRange, dbFactor, rate, <freqMode;
	var <bufSize;	// size of FFT
	var <>specialSynthDef, <specialSynthArgs; // Allows to override the analysis synth

	*initClass {
		StartUp.add {
			this.initSynthDefs;
		}
	}

	*new { arg parent, bounds, server;
		^super.new.initFreqScope (parent, bounds, server)
	}

	*initSynthDefs {
		// dbFactor -> 2/dbRange

		// linear
		SynthDef("system_freqScope0", { arg in=0, fftBufSize = 2048, scopebufnum=1, rate=4, dbFactor = 0.02;
			var phase = 1 - (rate * fftBufSize.reciprocal);
			var signal, chain, result, phasor, numSamples, mul, add;
			var fftbufnum = LocalBuf(fftBufSize, 1);
			mul = 0.00285;
			numSamples = (BufSamples.ir(fftbufnum) - 2) * 0.5; // 1023 (bufsize=2048)
			signal = In.ar(in);
			chain = FFT(fftbufnum, signal, hop: 0.75, wintype:1);
			chain = PV_MagSmear(chain, 1);
			// -1023 to 1023, 0 to 2046, 2 to 2048 (skip first 2 elements DC and Nyquist)
			phasor = LFSaw.ar(rate/BufDur.ir(fftbufnum), phase, numSamples, numSamples + 2);
			phasor = phasor.round(2); // the evens are magnitude
			ScopeOut.ar( ((BufRd.ar(1, fftbufnum, phasor, 1, 1) * mul).ampdb * dbFactor) + 1, scopebufnum);
		}, [\kr, \ir, \ir, \ir, \kr]).add;
		SynthDef("system_freqScope0_shm", { arg in=0, fftBufSize = 2048, scopebufnum=1, rate=4, dbFactor = 0.02;
			var phase = 1 - (rate * fftBufSize.reciprocal);
			var signal, chain, result, phasor, numSamples, mul, add;
			var fftbufnum = LocalBuf(fftBufSize, 1);
			mul = 0.00285;
			numSamples = (BufSamples.ir(fftbufnum) - 2) * 0.5; // 1023 (bufsize=2048)
			signal = In.ar(in);
			chain = FFT(fftbufnum, signal, hop: 0.75, wintype:1);
			chain = PV_MagSmear(chain, 1);
			// -1023 to 1023, 0 to 2046, 2 to 2048 (skip first 2 elements DC and Nyquist)
			phasor = LFSaw.ar(rate/BufDur.ir(fftbufnum), phase, numSamples, numSamples + 2);
			phasor = phasor.round(2); // the evens are magnitude
			ScopeOut2.ar( ((BufRd.ar(1, fftbufnum, phasor, 1, 1) * mul).ampdb * dbFactor) + 1, scopebufnum, fftBufSize/rate);
		}, [\kr, \ir, \ir, \ir, \kr]).add;

		// logarithmic
		SynthDef("system_freqScope1", { arg in=0, fftBufSize = 2048, scopebufnum=1, rate=4, dbFactor = 0.02;
			var phase = 1 - (rate * fftBufSize.reciprocal);
			var signal, chain, result, phasor, halfSamples, mul, add;
			var fftbufnum = LocalBuf(fftBufSize, 1);
			mul = 0.00285;
			halfSamples = BufSamples.ir(fftbufnum) * 0.5;
			signal = In.ar(in);
			chain = FFT(fftbufnum, signal, hop: 0.75, wintype:1);
			chain = PV_MagSmear(chain, 1);
			phasor = halfSamples.pow(LFSaw.ar(rate/BufDur.ir(fftbufnum), phase, 0.5, 0.5)) * 2; // 2 to bufsize
			phasor = phasor.round(2); // the evens are magnitude
			ScopeOut.ar( ((BufRd.ar(1, fftbufnum, phasor, 1, 1) * mul).ampdb * dbFactor) + 1, scopebufnum);
		}, [\kr, \ir, \ir, \ir, \kr]).add;

		SynthDef("system_freqScope1_shm", { arg in=0, fftBufSize = 2048, scopebufnum=1, rate=4, dbFactor = 0.02;
			var phase = 1 - (rate * fftBufSize.reciprocal);
			var signal, chain, result, phasor, halfSamples, mul, add;
			var fftbufnum = LocalBuf(fftBufSize, 1);
			mul = 0.00285;
			halfSamples = BufSamples.ir(fftbufnum) * 0.5;
			signal = In.ar(in);
			chain = FFT(fftbufnum, signal, hop: 0.75, wintype:1);
			chain = PV_MagSmear(chain, 1);
			phasor = halfSamples.pow(LFSaw.ar(rate/BufDur.ir(fftbufnum), phase, 0.5, 0.5)) * 2; // 2 to bufsize
			phasor = phasor.round(2); // the evens are magnitude
			ScopeOut2.ar( ((BufRd.ar(1, fftbufnum, phasor, 1, 1) * mul).ampdb * dbFactor) + 1, scopebufnum, fftBufSize/rate);
		}, [\kr, \ir, \ir, \ir, \kr]).add;

		// These next two are based on the original two, but adapted by Dan Stowell
		// to calculate the frequency response between two channels
		SynthDef("system_freqScope0_magresponse", { arg in=0, fftBufSize = 2048, scopebufnum=1, rate=4, dbFactor = 0.02, in2=1;
			var phase = 1 - (rate * fftBufSize.reciprocal);
			var signal, chain, result, phasor, numSamples, mul, add;
			var signal2, chain2, divisionbuf;
			var fftbufnum = LocalBuf(fftBufSize, 1);
			mul = 0.00285;
			numSamples = (BufSamples.ir(fftbufnum) - 2) * 0.5; // 1023 (bufsize=2048)
			signal = In.ar(in);
			signal2 = In.ar(in2);
			chain = FFT(fftbufnum, signal, hop: 0.75, wintype:1);
			divisionbuf = LocalBuf(BufFrames.ir(fftbufnum));
			chain2 = FFT(divisionbuf, signal2, hop: 0.75, wintype:1);
			// Here we perform complex division to estimate the freq response
			chain = PV_Div(chain2, chain);
			chain = PV_MagSmear(chain, 1);
			// -1023 to 1023, 0 to 2046, 2 to 2048 (skip first 2 elements DC and Nyquist)
			phasor = LFSaw.ar(rate/BufDur.ir(fftbufnum), phase, numSamples, numSamples + 2);
			phasor = phasor.round(2); // the evens are magnitude
			ScopeOut.ar( ((BufRd.ar(1, divisionbuf, phasor, 1, 1) * mul).ampdb * dbFactor) + 1, scopebufnum);
		}, [\kr, \ir, \ir, \ir, \kr, \ir]).add;

		SynthDef("system_freqScope0_magresponse_shm", { arg in=0, fftBufSize = 2048, scopebufnum=1, rate=4, dbFactor = 0.02, in2=1;
			var phase = 1 - (rate * fftBufSize.reciprocal);
			var signal, chain, result, phasor, numSamples, mul, add;
			var signal2, chain2, divisionbuf;
			var fftbufnum = LocalBuf(fftBufSize, 1);
			mul = 0.00285;
			numSamples = (BufSamples.ir(fftbufnum) - 2) * 0.5; // 1023 (bufsize=2048)
			signal = In.ar(in);
			signal2 = In.ar(in2);
			chain = FFT(fftbufnum, signal, hop: 0.75, wintype:1);
			divisionbuf = LocalBuf(BufFrames.ir(fftbufnum));
			chain2 = FFT(divisionbuf, signal2, hop: 0.75, wintype:1);
			// Here we perform complex division to estimate the freq response
			chain = PV_Div(chain2, chain);
			chain = PV_MagSmear(chain, 1);
			// -1023 to 1023, 0 to 2046, 2 to 2048 (skip first 2 elements DC and Nyquist)
			phasor = LFSaw.ar(rate/BufDur.ir(fftbufnum), phase, numSamples, numSamples + 2);
			phasor = phasor.round(2); // the evens are magnitude
			ScopeOut2.ar( ((BufRd.ar(1, divisionbuf, phasor, 1, 1) * mul).ampdb * dbFactor) + 1, scopebufnum, fftBufSize/rate);
		}, [\kr, \ir, \ir, \ir, \kr, \ir]).add;

		SynthDef("system_freqScope1_magresponse", { arg in=0, fftBufSize = 2048, scopebufnum=1, rate=4, dbFactor = 0.02, in2=1;
			var phase = 1 - (rate * fftBufSize.reciprocal);
			var signal, chain, result, phasor, halfSamples, mul, add;
			var signal2, chain2, divisionbuf;
			var fftbufnum = LocalBuf(fftBufSize, 1);
			mul = 0.00285;
			halfSamples = BufSamples.ir(fftbufnum) * 0.5;
			signal = In.ar(in);
			signal2 = In.ar(in2);
			chain = FFT(fftbufnum, signal, hop: 0.75, wintype:1);
			divisionbuf = LocalBuf(BufFrames.ir(fftbufnum));
			chain2 = FFT(divisionbuf, signal2, hop: 0.75, wintype:1);
			// Here we perform complex division to estimate the freq response
			chain = PV_Div(chain2, chain);
			chain = PV_MagSmear(chain, 1);
			phasor = halfSamples.pow(LFSaw.ar(rate/BufDur.ir(fftbufnum), phase, 0.5, 0.5)) * 2; // 2 to bufsize
			phasor = phasor.round(2); // the evens are magnitude
			ScopeOut.ar( ((BufRd.ar(1, divisionbuf, phasor, 1, 1) * mul).ampdb * dbFactor) + 1, scopebufnum);
		}, [\kr, \ir, \ir, \ir, \kr, \ir]).add;

		SynthDef("system_freqScope1_magresponse_shm", { arg in=0, fftBufSize = 2048, scopebufnum=1, rate=4, dbFactor = 0.02, in2=1;
			var phase = 1 - (rate * fftBufSize.reciprocal);
			var signal, chain, result, phasor, halfSamples, mul, add;
			var signal2, chain2, divisionbuf;
			var fftbufnum = LocalBuf(fftBufSize, 1);
			mul = 0.00285;
			halfSamples = BufSamples.ir(fftbufnum) * 0.5;
			signal = In.ar(in);
			signal2 = In.ar(in2);
			chain = FFT(fftbufnum, signal, hop: 0.75, wintype:1);
			divisionbuf = LocalBuf(BufFrames.ir(fftbufnum));
			chain2 = FFT(divisionbuf, signal2, hop: 0.75, wintype:1);
			// Here we perform complex division to estimate the freq response
			chain = PV_Div(chain2, chain);
			chain = PV_MagSmear(chain, 1);
			phasor = halfSamples.pow(LFSaw.ar(rate/BufDur.ir(fftbufnum), phase, 0.5, 0.5)) * 2; // 2 to bufsize
			phasor = phasor.round(2); // the evens are magnitude
			ScopeOut2.ar( ((BufRd.ar(1, divisionbuf, phasor, 1, 1) * mul).ampdb * dbFactor) + 1, scopebufnum, fftBufSize/rate);
		}, [\kr, \ir, \ir, \ir, \kr, \ir]).add;
	}

	initFreqScope { arg parent, bounds, argServer;
		server = argServer ? Server.default;
		if (this.shmScopeAvailable) {
			scope = ScopeView.new(parent, bounds);
			scope.server = server;
			scope.fill = false;
		} {
			scope = SCScope(parent, bounds);
		};

		active = false;
		inBus = 0;
		dbRange = 96;
		dbFactor = 2/dbRange;
		rate = 4;
		freqMode = 0;
		bufSize = 2048;
		ServerQuit.add(this, server);
		^this;
	}

	allocBuffers {
		if (this.shmScopeAvailable) {
			scopebuf = ScopeBuffer.alloc(server);
			scope.bufnum = scopebuf.bufnum;
		} {
			Buffer.alloc(server, bufSize/4, 1, { |sbuf|
				scope.bufnum = sbuf.bufnum;
				scopebuf = sbuf;
			});
		};
	}

	freeBuffers {
		if (scopebuf.notNil) {
			scopebuf.free; scopebuf = nil;
		};
	}

	start {
		var defname, args;

		if (synth.notNil) { synth.free };
		if (scopebuf.isNil) { this.allocBuffers };

		defname = specialSynthDef ?? {
			"system_freqScope" ++ freqMode.asString ++ if (this.shmScopeAvailable) {"_shm"} {""}
		};
		args = [\in, inBus, \dbFactor, dbFactor, \rate, 4, \fftBufSize, bufSize,
			\scopebufnum, scopebuf.bufnum] ++ specialSynthArgs;
		synth = Synth.tail(RootNode(server), defname, args);
		if (scope.isKindOf(ScopeView)) { scope.start };
	}

	kill {
		this.active_(false);
		this.freeBuffers;
		ServerQuit.remove(this, server);
	}

	active_ { |bool|
		if (bool) {
			ServerTree.add(this, server);
			if (server.serverRunning) {
				active = bool;
				this.doOnServerTree;
				^this
			}
		} {
			ServerTree.remove(this, server);
			if (server.serverRunning and: active) {
				if (scope.isKindOf(ScopeView)) { scope.stop };
				synth.free;
				synth = nil;
			};
		};
		active = bool;
		^this
	}

	doOnServerTree {
		synth = nil;
		if (active) { this.start; }
	}

	doOnServerQuit {
		var thisScope = scope;
		defer {
			thisScope.stop;
		};
		scopebuf = synth = nil;
	}

	inBus_ { arg num;
		inBus = num;
		if(active, {
			synth.set(\in, inBus);
		});
		^this
	}

	dbRange_ { arg db;
		dbRange = db;
		dbFactor = 2/db;
		if(active, {
			synth.set(\dbFactor, dbFactor);
		});
	}

	freqMode_ { arg mode;
		freqMode = mode.asInteger.clip(0,1);
		if(active, {
			this.start;
		});
	}

	specialSynthArgs_ {|args|
		specialSynthArgs = args;
		if(args.notNil and:{active}){
			synth.set(*specialSynthArgs);
		}
	}

	special { |defname, extraArgs|
		this.specialSynthDef_(defname);
		this.specialSynthArgs_(extraArgs);
		if(active, {
			this.start;
		});
	}

	*response { |parent, bounds, bus1, bus2, freqMode = 1|
		var scope = this.new(parent, bounds, bus1.server).inBus_(bus1.index);
		var synthDefName = "system_freqScope%_magresponse%".format(freqMode, if (scope.shmScopeAvailable) {"_shm"} {""});

		^scope.special(synthDefName, [\in2, bus2])
	}

	doesNotUnderstand { arg selector ... args;
		^scope.performList(selector, args);
	}

	shmScopeAvailable {
		^server.isLocal
		// and: { server.inProcess.not }
	}
}

FreqScope {
	classvar <scopeOpen;

	var <scope, <window;

	*new { arg width=522, height=300, busNum=0, scopeColor, bgColor, server;
		var rect, scope, window, pad, font, freqLabel, freqLabelDist, dbLabel, dbLabelDist;
		var setFreqLabelVals, setDBLabelVals;
		var nyquistKHz;
		busNum = busNum.asControlInput;
		if(scopeOpen != true, { // block the stacking up of scope windows
			//make scope

			scopeColor = scopeColor ?? { Color.new255(255, 218, 000) };

			scopeOpen = true;

			server = server ? Server.default;

			rect = Rect(0, 0, width, height);
			pad = [30, 48, 14, 10]; // l,r,t,b
			font = Font.monospace(9);
			freqLabel = Array.newClear(12);
			freqLabelDist = rect.width/(freqLabel.size-1);
			dbLabel = Array.newClear(17);
			dbLabelDist = rect.height/(dbLabel.size-1);

			nyquistKHz = server.sampleRate;
			if( (nyquistKHz == 0) || nyquistKHz.isNil, {
				nyquistKHz = 22.05 // best guess?
			},{
				nyquistKHz = nyquistKHz * 0.0005;
			});

			setFreqLabelVals = { arg mode, bufsize;
				var kfreq, factor, halfSize;

				factor = 1/(freqLabel.size-1);
				halfSize = bufsize * 0.5;

				freqLabel.size.do({ arg i;
					if(mode == 1, {
						kfreq = (halfSize.pow(i * factor) - 1)/(halfSize-1) * nyquistKHz;
					},{
						kfreq = i * factor * nyquistKHz;
					});

					if(kfreq > 1.0, {
						freqLabel[i].string_( kfreq.asString.keep(4) ++ "k" )
					},{
						freqLabel[i].string_( (kfreq*1000).asInteger.asString)
					});
				});
			};

			setDBLabelVals = { arg db;
				dbLabel.size.do({ arg i;
					dbLabel[i].string = (i * db/(dbLabel.size-1)).asInteger.neg.asString;
				});
			};

			window = Window("Freq Analyzer", rect.resizeBy(pad[0] + pad[1] + 4, pad[2] + pad[3] + 4), false);

			freqLabel.size.do({ arg i;
				freqLabel[i] = StaticText(window, Rect(pad[0] - (freqLabelDist*0.5) + (i*freqLabelDist), pad[2] - 10, freqLabelDist, 10))
					.font_(font)
					.align_(\center)
				;
				StaticText(window, Rect(pad[0] + (i*freqLabelDist), pad[2], 1, rect.height))
					.string_("")
				;
			});

			dbLabel.size.do({ arg i;
				dbLabel[i] = StaticText(window, Rect(0, pad[2] + (i*dbLabelDist), pad[0], 10))
					.font_(font)
					.align_(\left)
				;
				StaticText(window, Rect(pad[0], dbLabel[i].bounds.top, rect.width, 1))
					.string_("")
				;
			});

			scope = FreqScopeView(window, rect.moveBy(pad[0], pad[2]), server);
			scope.xZoom_((scope.bufSize*0.25) / width);

			setFreqLabelVals.value(scope.freqMode, 2048);
			setDBLabelVals.value(scope.dbRange);

			Button(window, Rect(pad[0] + rect.width, pad[2], pad[1], 16))
				.states_([["stop", Color.white, Color.green(0.5)], ["start", Color.white, Color.red(0.5)]])
				.action_({ arg view;
					if(view.value == 0, {
						scope.active_(true);
					},{
						scope.active_(false);
					});
				})
				.font_(font)
				.canFocus_(false)
			;

			StaticText(window, Rect(pad[0] + rect.width, pad[2]+20, pad[1], 10))
				.string_("BusIn")
				.font_(font)
			;

			NumberBox(window, Rect(pad[0] + rect.width, pad[2]+30, pad[1], 14))
				.action_({ arg view;
					view.value_(view.value.asInteger.clip(0, server.options.numAudioBusChannels));
					scope.inBus_(view.value);
				})
				.value_(busNum)
				.font_(font)
			;

			StaticText(window, Rect(pad[0] + rect.width, pad[2]+48, pad[1], 10))
				.string_("FrqScl")
				.font_(font)
			;
			PopUpMenu(window, Rect(pad[0] + rect.width, pad[2]+58, pad[1], 16))
				.items_(["lin", "log"])
				.action_({ arg view;
					scope.freqMode_(view.value);
					setFreqLabelVals.value(scope.freqMode, 2048);
				})
				.canFocus_(false)
				.font_(font)
				.valueAction_(1)
			;

			StaticText(window, Rect(pad[0] + rect.width, pad[2]+76, pad[1], 10))
				.string_("dbCut")
				.font_(font)
			;
			PopUpMenu(window, Rect(pad[0] + rect.width, pad[2]+86, pad[1], 16))
				.items_(Array.series(12, 12, 12).collect({ arg item; item.asString }))
				.action_({ arg view;
					scope.dbRange_((view.value + 1) * 12);
					setDBLabelVals.value(scope.dbRange);
				})
				.canFocus_(false)
				.font_(font)
				.value_(7)
			;

			scope
				.inBus_(busNum)
				.active_(true)
				.style_(1)
				.waveColors_([scopeColor.alpha_(1)])
				.canFocus_(false)
			;

			if (bgColor.notNil) {
				scope.background_(bgColor)
			};

			window.onClose_({
				scope.kill;
				scopeOpen = false;
			}).front;
			^super.newCopyArgs(scope, window)
		});
	}
}

PlusFreqScope {
	classvar <server;

	var <scope;
	var <scopebuf;
	var <active, <synth, <inBus, <dbRange, dbFactor, rate, <freqMode;
	var <bufSize;	// size of FFT
	var <>specialSynthDef, <specialSynthArgs; // Allows to override the analysis synth

	*initClass {
		StartUp.add {
			server = GUI.current.stethoscope.defaultServer;

			this.initSynthDefs;
		}
	}

	*server_ {|aServer|
		if(GUI.current.stethoscope.isValidServer(aServer).not) {
			Error("PlusFreqScope: can not use server '%' with current GUI scheme (%)"
				.format(aServer.name, GUI.current.id)).throw;
		};

		server = aServer;
	}

	*new { arg parent, bounds;
		^super.new.initFreqScope (parent, bounds)
	}

	*initSynthDefs {
		// dbFactor -> 2/dbRange

		// linear
		SynthDef("freqScope0", { arg in=0, fftBufSize = 2048, scopebufnum=1, rate=4, dbFactor = 0.02;
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
		SynthDef("freqScope0_shm", { arg in=0, fftBufSize = 2048, scopebufnum=1, rate=4, dbFactor = 0.02;
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
		SynthDef("freqScope1", { arg in=0, fftBufSize = 2048, scopebufnum=1, rate=4, dbFactor = 0.02;
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

		SynthDef("freqScope1_shm", { arg in=0, fftBufSize = 2048, scopebufnum=1, rate=4, dbFactor = 0.02;
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
		SynthDef("freqScope0_magresponse", { arg in=0, fftBufSize = 2048, scopebufnum=1, rate=4, dbFactor = 0.02, in2=1;
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

		SynthDef("freqScope0_magresponse_shm", { arg in=0, fftBufSize = 2048, scopebufnum=1, rate=4, dbFactor = 0.02, in2=1;
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

		SynthDef("freqScope1_magresponse", { arg in=0, fftBufSize = 2048, scopebufnum=1, rate=4, dbFactor = 0.02, in2=1;
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

		SynthDef("freqScope1_magresponse_shm", { arg in=0, fftBufSize = 2048, scopebufnum=1, rate=4, dbFactor = 0.02, in2=1;
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

	initFreqScope { arg parent, bounds;
		if (this.shmScopeAvailable) {
			scope = \QScope2.asClass.new(parent, bounds);
			scope.server = server;
		} {
			scope = ScopeView(parent, bounds);
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

	allocBuffersAndStart {
		if (this.shmScopeAvailable) {
			scopebuf = ScopeBuffer.alloc(server);
			scope.bufnum = scopebuf.bufnum;
			this.start;
		} {
			Buffer.alloc(server, bufSize/4, 1, { |sbuf|
				scope.bufnum = sbuf.bufnum;
				scopebuf = sbuf;
				this.start;
			});
		}
	}

	freeBuffers {
		if (scopebuf.notNil) {
			scopebuf.free; scopebuf = nil;
		};
	}

	start {
		var defname = specialSynthDef ?? {"freqScope" ++ freqMode.asString ++ if (this.shmScopeAvailable) {"_shm"} {""}};
		var args = [\in, inBus, \dbFactor, dbFactor, \rate, 4, \fftBufSize, bufSize,
			\scopebufnum, scopebuf.bufnum] ++ specialSynthArgs;
		synth = Synth.tail(RootNode(server), defname, args);
		if (scope.class.name === \QScope2) { scope.start };
	}

	kill {
		this.active_(false);
		this.freeBuffers;
		ServerQuit.remove(this, server);
	}

	active_ { arg activate;
		if (activate) {
			ServerTree.add(this, server);
			if (server.serverRunning) {
				active=activate;
				this.doOnServerTree;
				^this
			}
		} {
			ServerTree.remove(this, server);
			if (server.serverRunning and: active) {
				if (scope.class.name === \QScope2) { scope.stop };
				synth.free;
			};
		};
		active=activate;
		^this
	}

	doOnServerTree {
		if (active) {
			if (scopebuf.isNil) {
				this.allocBuffersAndStart;
			} {
				this.start;
			}
		}
	}

	doOnServerQuit {
		scope.stop;
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
			synth.free;
			this.start;
		});
	}

	specialSynthArgs_ {|args|
		specialSynthArgs = args;
		if(args.notNil and:{active}){
			synth.set(*specialSynthArgs);
		}
	}

	special { |defname, extraargs|
		this.specialSynthDef_(defname);
		this.specialSynthArgs_(extraargs);
		if(active, {
			synth.free;
			this.start;
		});
	}

	*response{ |parent, bounds, bus1, bus2, freqMode=1|
		var scope = this.new(parent, bounds).inBus_(bus1.index);
		var synthDefName = "freqScope%_magresponse%".format(freqMode, if (scope.shmScopeAvailable) {"_shm"} {""});

		^scope.special(synthDefName, [\in2, bus2])
	}

	doesNotUnderstand { arg selector ... args;
		^scope.performList(selector, args);
	}

	shmScopeAvailable {
		if (GUI.id != \qt) { ^false };
		^(server.isLocal and: (server.inProcess.not))
	}
}

PlusFreqScopeWindow {
	classvar <scopeOpen;

	var <scope, <window;

	*new { arg width=522, height=300, busNum=0, scopeColor, bgColor;
		var rect, scope, window, pad, font, freqLabel, freqLabelDist, dbLabel, dbLabelDist;
		var setFreqLabelVals, setDBLabelVals;
		var nyquistKHz;
		if(scopeOpen != true, { // block the stacking up of scope windows
			//make scope

			scopeOpen = true;

			if(scopeColor.isNil, { scopeColor = Color.green });
			if(bgColor.isNil, { bgColor = Color.green(0.1) });

			rect = Rect(0, 0, width, height);
			pad = [30, 48, 14, 10]; // l,r,t,b
			font = Font.monospace(9);
			freqLabel = Array.newClear(12);
			freqLabelDist = rect.width/(freqLabel.size-1);
			dbLabel = Array.newClear(17);
			dbLabelDist = rect.height/(dbLabel.size-1);

			nyquistKHz = PlusFreqScope.server.sampleRate;
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
					.background_(scopeColor.alpha_(0.25))
				;
			});

			dbLabel.size.do({ arg i;
				dbLabel[i] = StaticText(window, Rect(0, pad[2] + (i*dbLabelDist), pad[0], 10))
					.font_(font)
					.align_(\left)
				;
				StaticText(window, Rect(pad[0], dbLabel[i].bounds.top, rect.width, 1))
					.string_("")
					.background_(scopeColor.alpha_(0.25))
				;
			});

			scope = PlusFreqScope(window, rect.moveBy(pad[0], pad[2]));
			scope.xZoom_((scope.bufSize*0.25) / width);

			setFreqLabelVals.value(scope.freqMode, 2048);
			setDBLabelVals.value(scope.dbRange);

			Button(window, Rect(pad[0] + rect.width, pad[2], pad[1], 16))
				.states_([["Power", Color.white, Color.green(0.5)], ["Power", Color.white, Color.red(0.5)]])
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
					view.value_(view.value.asInteger.clip(0, Server.internal.options.numAudioBusChannels));
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
				.background_(bgColor)
				.style_(1)
				.waveColors_([scopeColor.alpha_(1)])
				.canFocus_(false)
			;

			window.onClose_({
				scope.kill;
				scopeOpen = false;
			}).front;
			^super.newCopyArgs(scope, window)
		});
	}

	*server {
		^PlusFreqScope.server
	}
	*server_ {|aServer|
		^PlusFreqScope.server_(aServer)
	}
}

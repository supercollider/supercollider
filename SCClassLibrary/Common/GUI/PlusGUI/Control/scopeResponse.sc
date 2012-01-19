/*
Server.default = s = Server.internal.boot;

{|in| MoogFF.ar(in, freq: LFCub.kr(0.2).exprange(10, 10000))}.scopeResponse
{|in| MoogFF.ar(in)}.scopeResponse

LPF.scopeResponse
HPF.scopeResponse
MoogFF.scopeResponse
BLowPass.scopeResponse
BBandPass.scopeResponse
BLowShelf.scopeResponse // by default BLowShelf doesn't mangle much
Resonz.scopeResponse
BRF.scopeResponse
Integrator.scopeResponse
Formlet.scopeResponse
Median.scopeResponse // nonlinear, and therefore interesting
Slew.scopeResponse

*/

+ Function {
	scopeResponse{ |server, freqMode=1, label="Empirical Frequency response", mute = false|

		var bus1, bus2, synth, win, fs;

		if (server.isNil) {
			server = GUI.freqScopeView.server;
		} {
			if (server != GUI.freqScopeView.server) {
				"Function-scopeReponse: resetting GUI.freqScopeView.server".warn;
				GUI.freqScopeView.server = server;
			};
		};

		// Create two private busses
		bus1 = Bus.audio(server, 1);
		bus2 = Bus.audio(server, 1);

		// Create the SCFreqScope.response using the same simple window as in the helpfile
		// Also, onClose must free the synth and the busses

		win = GUI.window.new(label, Rect(100, 100, 511, 300));
		fs = GUI.freqScopeView.response(win, win.view.bounds, bus1, bus2, freqMode);

		win.onClose_ {
			fs.kill;
			synth.release;
		};

		win.front;
		fs.active_(true);

		// Create a synth using this function and the busses
		synth = { |gate = 1|
			var noise = PinkNoise.ar;
			var filtered = this.value(noise);
			var env = EnvGen.kr(Env.asr(0.1, 1, 0.1, \sine), gate, 0.1, doneAction: 2);
			if (not(mute)) {
				Out.ar(0, (filtered * env) ! 2);   // filter only
			};
			Out.ar(bus1, noise);
			Out.ar(bus2, filtered);
		}.play(server.defaultGroup);
		synth.register;
		synth.onFree {
			{
				[bus1, bus2].do(_.free);
				fs.active_(false);
				win.close;
			}.defer;
		}

		^fs
	}
}


+ Filter {
	*scopeResponse { |server, freqMode=1, label, args|
		var argNames = this.class.findRespondingMethodFor(\ar).argNames;
		var hasFreqInput = argNames.includes(\freq);

		^if(hasFreqInput){
			{|in| this.ar(in: in, freq:MouseX.kr(10, SampleRate.ir / 4, 1)) * Line.ar(0,1,0.1) }
				.scopeResponse(server, freqMode,
					label ?? {"%: empirical frequency response (move mouse to change freq)".format(this.asString)}
					)
		}{ // no freq input
			{|in| this.ar(in: in) * Line.ar(0,1,0.1) }
				.scopeResponse(server, freqMode,
					label ?? {"%: empirical frequency response".format(this.asString)}
					)
		}
	}
}

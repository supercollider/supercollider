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
	scopeResponse{ |server, freqMode=1, label="Empirical Frequency response"| 
		
		var bus1, bus2, synth, win, fs;
		server = server ?? {GUI.stethoscope.defaultServer};
		
		// Create two private busses
		bus1 = Bus.audio(server, 1).postln;
		bus2 = Bus.audio(server, 1).postln;
		
		// Create the SCFreqScope.response using the same simple window as in the helpfile
		// Also, onClose must free the synth and the busses
		
		win = GUI.window.new(label, Rect(100, 100, 511, 300));
		fs = GUI.freqScopeView.response(win, win.view.bounds, bus1, bus2, freqMode);
		win.onClose_{
			fs.kill; 
			server.bind{[synth, bus1, bus2].do(_.free)};
		};
		win.front;
		fs.active_(true);
		
		Task{
			1.5.wait;
			server.sync;
			// Create a synth using this function and the busses
			synth = {
			    var son1, son2;
			    son1 = PinkNoise.ar;
			    son2 = this.value(son1);
			    Out.ar(bus1, son1);
			    Out.ar(bus2, son2);
			    Out.ar(0, [son1, son2] * 0.1); // audible
			}.play(Node.basicNew(server, fs.node), addAction: \addBefore);
		}.play;
		
		^fs
	}
}


+ Filter {
	*scopeResponse { |server, freqMode=1, label, args|
		var argNames = this.class.findRespondingMethodFor(\ar).argNames;
		var hasFreqInput = argNames.includes(\freq);
		
		^if(hasFreqInput){
			{|in| this.ar(in: in, freq:MouseX.kr(10, 10000, 1)) * Line.ar(0,1,0.1) }
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

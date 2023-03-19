
/*
Adding harmonics to a voice input


One easy way to add harmonics is to use the [PitchShift] UGen. See its helpfile for more details.

An alternative approach is to use the [Pitch] UGen to track the pitch contour, and use that in synthesis. This allows you to generate sounds matching the pitch of the input but with any kind of texture.

*/


// (Dan Stowell) (public domain 2006)

// USE HEADPHONES to prevent feedback.

(

x = {

	var son, freq, hasFreq, amp, out, harm;
	son = SoundIn.ar(0); // get first channel of sound input
	# freq, hasFreq = Pitch.kr(son); // Pitch of input signal
	amp = Amplitude.ar(son); // Amplitude of input signal
	harm = SinOsc.ar(freq * 2, 0, amp * hasFreq);
	Pan2.ar(harm + son)

}.play;

)


x.free; // Use this to stop the synth


/*
The above example adds only one harmonic. You can extend this to create multiple harmonics quite easily, for example by using an array of pitch ratios rather than a single ratio (and therefore creating an array of sine oscillators); or you could change the texture by using a more unusual oscillator (e.g. [Saw] or [Gendy1] or [HenonN]).


If you want a large number of controllable overtones then a bank of separate sine oscillators will become quite computationally intensive, so a good alternative is to use the general wavetable oscillator [Osc] instead of SinOsc. The wavetable (a buffer) can be filled with a wave representing the harmonic profile you're interested in, and only one oscillator is needed to read it back.


Let's implement this, and add a GUI for controlling the contents of the wavetable:
*/



// (Dan Stowell, kernel) (gpl 2006)

// first make and send the Synthdef so it's on the server


(

SynthDef(\oscPlayer, {|bufnum,balance=0.5,smooth=0.1|
	var input,freq,hasFreq,amp,mix,wet;
	input = SoundIn.ar(0); // get first channel of sound input
	#freq,hasFreq = Pitch.kr(input); // pitch of input signal
	amp = Amplitude.ar(input); // amplitude of input signal
	wet = Osc.ar(bufnum,Lag.kr(freq,smooth), 0, amp);
	mix = (input * (1-balance)) + (wet * balance);
	Out.ar(0, Pan2.ar(mix));
}).add;

)



// Now to instantiate the synth and build the GUI.

// USE HEADPHONES to prevent feedback.

(

var win, buf, m, harmonics, maxHarmonics = 60, theSynth;


win = Window("input harmonica",Rect(200,200,420,160)).front;
harmonics = Array.fill(maxHarmonics,0);
harmonics.put(0,1);			// init array so first partial is alive
buf = Buffer.alloc(s, 4096, 1);	// create buffer
buf.sine1(harmonics);			// fill buffer
theSynth = Synth(\oscPlayer, [\bufnum, buf.bufnum], s);

m = MultiSliderView(win, Rect(5, 5, 350, 100))

.value_(harmonics)
.isFilled_(true)
.valueThumbSize_(3.0)
.indexThumbSize_(3.0)
.gap_(2)
.elasticMode_(1)
.action_({|v| buf.sine1(v.value)}); // fill buffer again

Button(win,Rect(5,110,70,20))

.states_([["CLEAR",Color.green,Color.black]])
.action_({var temp;
	temp = Array.fill(maxHarmonics,0);
	buf.sine1(temp);
	m.value_(temp);
});



Button(win,Rect(80,110,65,20))

.states_([["RAND",Color.green,Color.black]])
.action_({|v| var temp;
	temp = m.value.scramble;
	buf.sine1(temp);
	m.value_(temp);
});



Button(win,Rect(150,110,20,20))

.states_([["<<",Color.green,Color.black]])
.action_({|v| var temp;
	temp = m.value.rotate(-1);
	buf.sine1(temp);
	m.value_(temp);
});



Button(win,Rect(175,110,20,20))

.states_([[">>",Color.green,Color.black]])
.action_({|v| var temp;
	temp = m.value.rotate(1);
	buf.sine1(temp);
	m.value_(temp);
});



Slider(win, Rect(365,5,20,100))
.value_(0.1)
.action_({|v| theSynth.set(\smooth,[0,1,\lin,0.001].asSpec.unmap(v.value))});


Slider(win, Rect(390,5,20,100))
.value_(0.5)
.action_({|v| theSynth.set(\balance,v.value)});

)

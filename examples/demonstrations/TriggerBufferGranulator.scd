//Sam Pluta - 2006
//this Synth uses the SoundIn as a trigger to step through and granulate the loaded buffer

s.boot;
b = Buffer.read(s, "sounds/a11wlk01.wav");  //use your own buffer here - mine is probably not on your computer

(
SynthDef("TriggerBufferGranular", {arg out=0, bufnum=0, in=0, thresh = 0.004, ampScaler = 1, duration =
0.2, pan=0,centerPosition=0.1, interp=1, incrementBy = 200;

	var trig, amp, gatedAmp, triggerBool, outamp, bufferFrames, bufRateScale;
	var bufPointer, trigs, env, envGens, outArray, grNum;

	grNum = 4; // the number of granulators in the synthdef
	bufferFrames = BufFrames.kr(bufnum);
	bufRateScale = BufRateScale.kr(bufnum);
	amp = Amplitude.kr (SoundIn.ar(in));
	triggerBool = (SoundIn.ar(in) >= thresh);
	gatedAmp = Latch.kr(amp, triggerBool);
	outamp = (gatedAmp * ampScaler);

	trig = Trig.kr(triggerBool, duration/4);  //makes a trigger if triggerBool goes over the threshold
	env = Env([0,1,0],[duration/2,duration/2],'sine');

	bufPointer = PulseCount.kr(trig);  //a counter that counts the number of triggers triggered

	//each trigger below triggers once for every four times trig (above) triggers (if a trig trig could trig trig)

	trigs = Array.fill(grNum, {|i| PulseDivider.kr(trig, 4, i )});
	envGens = Array.fill(grNum, {|i| EnvGen.kr(env, trigs[i] ) });

	//trigs correspond to and trigger a PlayBuf below - the pointer slowly moves through the file
	outArray = Array.fill(grNum, {|i| PlayBuf.ar(1, bufnum, bufRateScale, trigs[i], (bufPointer*incrementBy)%bufferFrames)*envGens[i]});

	Out.ar(out, Pan2.ar(Mix(outArray)));

}).play(s,[\out, 0, \bufnum, b.bufnum]);
)



// (thor magnusson) (2007)

// a 19th century steam train leaving the station

(
{
	var steam, pan, time, e2, filter, we, whistle;
	time = 24;

	e2 = Env.new([-0.8, 0.8 ], [ time+2],'sine');
	we = Env.new([0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 0], [2, 0, 0.2, 0, 0.2, 0, 0.8, 0, 4, 0, 3]);

	steam = WhiteNoise.ar(LFSaw.ar(XLine.ar(1, 7, time)))+PinkNoise.ar(LFSaw.ar(XLine.ar(1, 7, time)));
	filter = BPF.ar(steam, 600, 1+EnvGen.ar(Env.sine(time,9)));
	whistle = Mix.ar(GVerb.ar(Klank.ar(`[[800, 600, 1200, 990], nil, [1, 1, 1, 1]], WhiteNoise.ar(0.005)) * EnvGen.ar(we, timeScale:time/10.2 ), 100, 0.3, 0.1));

	pan = Pan2.ar(filter+whistle, EnvGen.ar(e2, doneAction:2));
	pan

}.play
)

// (julian rohrhuber) (2007)

// julian took the train into the 21st century:

(
{
	var hiss, time, e2, ee2, filter, we, whistle, train;
	time = 24;

	e2 = Env.new([0, 1, 0], [ time / 2, time / 2]);
	we = Env.new([0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 0], [0.2, 0, 0.2, 0, 0.2, 0, 0.8, 0, 2, 0, 3]);
	ee2 = EnvGen.ar(e2, doneAction:2);
	hiss = HPF.ar(WhiteNoise.ar(10 ! 2), ee2.linexp(0, 1, 10000, 1000), 0.03);

	whistle = Mix.ar(GVerb.ar(Klank.ar(`[[1, 1.25, 1.2, 1.5].midiratio * 50, nil, [1, 1, 1, 1] * 0.3], Saw.ar(67, 0.004)) * EnvGen.ar(we, timeScale:time/10.2 ), 100, 0.3, 0.1));

	train = hiss + whistle;
	train = BPF.ar(train, 600, 1 + EnvGen.ar(Env.sine(time,9)));
	train = DelayL.ar(train, 0.01, 1 - ee2 + [0, 0.01]);
	Pan2.ar(train, ee2.linlin(0, 1, -1, 1) * [1, 0.99], ee2.linexp(0, 1, 0.01, 1));
}.play
)

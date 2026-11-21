// InfiniteMelody by_otophilia

(
s = Server.default;

Routine.run {var c; c = Condition.new; s.bootSync(c);

(
	SynthDef('infOrgan',{
		arg outBus=0, pitch=50, amp=1.0;
		var out;
		pitch = pitch + LFNoise1.kr(1000, 0.02);
		pitch = pitch.midicps;
		amp = amp * AmpComp.kr(pitch, 60);
		amp = EnvGen.kr(Env.linen(0.04, 0.10, 0.06, 1, -2),1,amp, doneAction:2);
		out = LFTri.ar(pitch, 0, amp);
		out = Pan2.ar(out, LFNoise2.kr(4, 0.5));
		Out.ar(outBus, out);
	}).send(s);
	SynthDef('reverb',{
		arg inBus=0, outBus=0;
		var out, rev;
		out = In.ar(inBus, 2);
		rev = out;
		3.do{rev = AllpassN.ar(rev, 0.050, [rrand(0.020,0.050), rrand(0.020,0.050)], 0.3)};
		3.do{rev = AllpassN.ar(rev, 0.050, [rrand(0.020,0.050), rrand(0.020,0.050)], 2)};
		out = LPF.ar(rev, 5000, 0.5, out);
		out = Limiter.ar(out, 1, 0.02);
		ReplaceOut.ar(outBus, out);
	}).send(s);
); s.sync(c);

(
	~seq = Array.fill(12, {|i| (i*5)+
		[0, -1, 0, -5, -3, -1, 0, 2, 4, 2, 4, 0, 2, 4, 5, 2, 4, 7, 12, 11, 9, 7, 5, 4, 2, 4, 5, 2, 4, 0, 2, 4]
		%12}).flat;
	~seq2 = Array.fill(12, {|i| (i*5)%12+
		[-8, -5, -8, -8, -7, -10, -8, -5, -0, -5, 0, -5, -1, 0, 2, -5, 0, 2, 4, 7, 5, 0, -3, 0, -1, -0, 2, -5, 0, -5, -1, 0]
		}).flat;
);

(
	~clock = TempoClock(120/60);
	~i = 32;

	~msg = List.new;
	~group = Group.new;

	~reverb = Synth('reverb', nil, ~group, \addAfter);
	~clock.schedAbs(~clock.beats.floor + 1, {
		~msg = List.new;

		12.do{|i|
			var pitch, amp, syn;
			pitch = ~seq.wrapAt(~i) + (i*12);
			syn = Synth.basicNew('infOrgan');
			amp = 0.2;
			if(pitch<40){amp=amp*(pitch-10/(40-10)).min(1).max(0).squared};
			if(pitch>120){amp=amp*(1.0-(pitch-120/(140-120))).min(1).max(0).squared};
			~msg.add( syn.newMsg(~group, ['pitch', pitch, 'amp', amp, 'outBus', 0]) );
		};

		6.do{|i|
			var pitch, amp, syn;
			pitch = ~seq2.wrapAt(~i) + (i*12) + 48;
			syn = Synth.basicNew('infOrgan');
			amp = 0.1;
			if(pitch<60){amp=amp*(pitch-48/(60-48)).min(1).max(0).squared};
			if(pitch>110){amp=amp*(1.0-(pitch-110/(140-110))).min(1).max(0).squared};
			~msg.add( syn.newMsg(~group, ['pitch', pitch, 'amp', amp, 'outBus', 0]));
		};

		~msg !? {s.listSendBundle(s.latency, ~msg)};
		~i = ~i+1;
		(1/4)
	});
);

};

);
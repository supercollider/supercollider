SCSA_Demo : SCWindow {

	classvar
		<width = 300,
		<height = 200,
		buttonWidth = 100,
		buttonHeight = 30;

	var button, synth, server;

	*new {
		|name, bounds, server|
		^super.new(name, bounds).init(server);
	}

	init {
		|serverArg|
		server = serverArg;

		SynthDef(\demoSynth, {
			|gate = 1.0, out = 0|
			var env;
			env = EnvGen.kr(Env.adsr(0, 0, 1, 0), gate: gate, doneAction: 2);
			Out.ar(out, [SinOsc.ar(1000), SinOsc.ar(1500)] * env * 0.125);
		}).send(server);

		button = SCButton(this, Rect(
			(this.bounds.width - buttonWidth) * 0.5,
			(this.bounds.height - buttonHeight) * 0.5,
			buttonWidth,
			buttonHeight
		));
		button.states = [
			["Hit me!", Color.black, Color.gray],
			["Make it stop!", Color.black, Color.red]
		];
		button.action = {
			|theButton|
			theButton.value.switch(
				1, { synth = Synth.new(\demoSynth, target: server) },
				0, { synth.set(\gate, 0.0) }
			)
		};
	}

}
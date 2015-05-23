	// basic gui for a proxy, with monitor or not.

MonitorGui : JITGui {

	var <ampSl, <playBut, <setOutBox, <fadeBox;
	classvar <>lastOutBus;

	*initClass {
		Class.initClassTree(Spec);
		Class.initClassTree(Server);
		Spec.add(\ampx4, [0, 4, \amp]);
		Spec.add(\fadePx, [0, 100, \amp, 0, 0.02]);
		lastOutBus = Server.default.options.numAudioBusChannels;
	}
		// options can be: \level, \name, \fade
	*new { |object, parent, bounds, makeSkip=true, options = #[]|
		^super.newCopyArgs(object, 0, parent, bounds).init(makeSkip, options)
	}

	setDefaults { |options|
		var minWidth = 0;
		config = (
			level: options.includes(\level),
			name: options.includes(\name),
			fade: options.includes(\fade)
		);

	if (parent.notNil) { skin = skin.copy.put(\margin, 0@0) };

		// a lot more negotiations, based on which options should be there
		defPos = 10@260;

		minWidth = 200;

		if (config.level) { minWidth = minWidth + 60 };
		if (config.name) { minWidth = minWidth + 60 };
		if (config.fade) { minWidth = minWidth + 60 };
		minSize = minWidth @ (skin.buttonHeight + (skin.margin.y * 2));
	///	"MonitorGui-minSize: %\n".postf(minSize);
	}

	makeViews { |options|

		var fullWid = zone.bounds.width;
		var height = zone.bounds.height - (skin.margin.y * 2);

		var levelWid = if (config.level, 60, 0);
		var nameWid = if (config.name, 60, 0);
		var fadeWid = if (config.fade, 60, 0);
		var playWid = 40, outWid = 30;
		var sliderWidth = fullWid - (levelWid + playWid + outWid + nameWid + fadeWid) - 4;

		zone.decorator.margin_(0@0);
		zone.visible_(false);


		this.makeVol(sliderWidth + levelWid, height);
		this.makePlayOut(playWid, outWid, height);

		if (config.name) { this.makeNameView(nameWid, height) };
		if (config.fade) { this.makeFade(fadeWid, height) };
	}

	accepts { |obj|
		^(obj.isNil or: { obj.isKindOf(NodeProxy) })
	}

	makeNameView { |width, height|
		nameView = DragSource(zone, Rect(0,0, width, height))
		.font_(font).align_(0).resize_(3)
	}

	makeFade { |width = 60, height = 18|
		fadeBox = EZNumber(zone, width@height, \fade, \fadePx,
				{ |num| try { object.fadeTime_(num.value) } },
				try { object.fadeTime } ? 0.02,
				labelWidth: 28,
				numberWidth: width - 28
		);

		fadeBox.labelView.font_(font).background_(skin.foreground);
		fadeBox.numberView.font_(font).background_(skin.foreground);
		fadeBox.view.resize_(3);
	}

	makeVol { |width, height|
		var showLev = config.level.binaryValue;

		ampSl = EZSlider(zone, (width @ height), "", \amp, // for now, leave "vol" out
			{ arg sl; if(object.notNil) { object.vol_(sl.value) } },
			0, false,
			labelWidth: 0,
			//labelWidth: showLev * 20, // for now, leave "vol" out
			numberWidth: showLev * 40,
			gap: 1@1
		);

		ampSl.labelView.font_(font).align_(0);
		ampSl.view.resize_(2);
	}

	makePlayOut { |playWid, outWid, height|

		playBut = Button(zone, Rect(0,0, playWid, height))

		.font_(font).resize_(3)
		.states_([
			[ \play, skin.fontColor, skin.offColor],
			[ \stop, skin.fontColor, skin.onColor ]
		])
		.action_({ arg btn, modif;
			var func;
			var alt = modif.notNil and: { modif.isAlt };
			var shift = modif.notNil and: { modif.isShift };
			if(object.notNil) {
				[ 	{ 	if(shift) { object.playNDialog(usePlayN:true) } { if (alt) { object.end } { object.stop } }; },
					{ 	if(shift) { object.playNDialog(usePlayN:true) } { if (alt) { object.vol_(0) } }; // play silently
						object.play;
					}
				].at(btn.value).value;
				btn.value_((try { object.monitor.isPlaying } ? false).binaryValue);
			} {
				"MonitorGui - no proxy to play!".warn;
				btn.value_(0);
			}
		});

		setOutBox = EZNumber(zone, outWid@height, "", [0, lastOutBus, \lin, 1],
			{ |box, mod|
				if (object.notNil) {
					if (object.monitor.isNil) {
						"MonitorGui - monitor is nil, cannot set outs yet.".postln;
						box.numberView.string = "-"
					} {
						object.monitor.out_(box.value.asInteger);
					};
				};
			}, 0, labelWidth: 1, unitWidth: 1);	// 1 is workaround for EZNumber resize bug
		setOutBox.view.resize_(3);
		setOutBox.numberView
		.background_(skin.background)
		.stringColor_(skin.fontColor)
		.font_(font).align_(\center);
	}


	getState {
		var isAudio, newState;
		var monitor, outs, amps, newHasSeriesOut;
		var plays = 0, playsSpread = false;

		newState = (
			object: nil,
			name: \_none_,
			isAudio: false,
			monPlaying: 	0,
			vol: 		1,
			playsSpread: 	false,
			out: 		0,
			monFade:		0.02
		);

		if (object.isNil) { ^newState };

		newState.putPairs([
			\object, object,
			\name, object.key,
			\isAudio, object.rate == \audio
		]);

		monitor = object.monitor;
		if (monitor.isNil) {
			^newState
		};


		newState.putPairs([
			\monPlaying,	monitor.isPlaying.binaryValue,
			\vol, 		monitor.vol,
			\out, 		monitor.out ? 0,
			\playsSpread,	monitor.hasSeriesOuts.not,
			\monFade,		monitor.fadeTime
		]);

		^newState;
	}

	checkUpdate {
		var newState = this.getState;

//			// don't know why early exit suppresses changes in play state. fix later...
//		if (newState == prevState) { ^this };


		if (newState[\object] != prevState[\object]) {
			zone.visible_(newState[\object].notNil);
		};

		if (newState[\isAudio] != prevState[\isAudio]) {
			zone.enabled_(newState[\isAudio]);
		};

		if (nameView.notNil) {
			if (newState[\name] != prevState[\name]) {
				nameView.object_(newState[\object])
					.string_(newState[\name].asString);
			};
		};

		if (newState[\vol] != prevState[\vol]) {
			ampSl.value_(newState[\vol]);
		};
		if (newState[\monPlaying] != prevState[\monPlaying]) {
			playBut.value_(newState[\monPlaying]);
		};
		if (newState[\out] != prevState[\out]) {
			setOutBox.value_(newState[\out]);
		};
		if (fadeBox.notNil) {
			if (newState[\monFade] != prevState[\monFade]) {
				fadeBox.value_(newState[\monFade] ? 0.02);
			}
		};

		prevState = newState;
	}
}

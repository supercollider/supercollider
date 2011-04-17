	// basic gui for a nodeproxy, with monitor or not.

NdefGui : JITGui {
	classvar <buttonSizes;

	var <typeView, <fadeBox;
	var <monitorGui, <pauseBut, <sendBut, <edBut, <wakeBut;
	var <paramGui;

	var <config, <buttonFuncs;

	*initClass {
		Class.initClassTree(Spec);
		Spec.add(\ampx4, [0, 4, \amp]);
		Spec.add(\fadePx, [0, 100, \amp, 0, 0.02]);

		buttonSizes = (
			name: 70, type: 30, CLR: 30, reset: 40, scope: 40, doc: 30, end: 30, fade: 70,
			monitor: 200, monitorM: 250, monitorL: 300, playN: 20, pausR: 40, sendR: 40,
			ed: 20, rip: 20, poll: 35, wake: 50
		);
	}

	proxy_ { |proxy| ^this.object_(proxy) }
	proxy { ^this.object }
	edits { ^paramGui.widgets }

	editKeys {
		if (paramGui.isNil) { ^[] };
		^paramGui.editKeys;
	}

	highlightParams { |parOffset, num|
		var onCol = Color(1, 0.5, 0.5), offCol = Color.clear, col;
		{ paramGui.widgets.do { |widget, i|
			if (widget.notNil) {
				col = if (i >= parOffset and: (i < (parOffset + num).max(0)), onCol, offCol);
				widget.labelView.background_(col.green_([0.5, 0.7].wrapAt(i - parOffset div: 2)));
			}
		} }.defer;
	}

	*new { |object, numItems = (4), parent, bounds, makeSkip=true, options|
		options = options ?? { this.big };
		^super.newCopyArgs(object, numItems, parent, bounds).init(makeSkip, options)
	}

	*big {
			// two lines - for big editor
		^[\name, \type, \CLR, \reset, \scope, \doc, \end, \fade, \poll,
			\monitorL, \playN, \pausR, \sendR  ]
	}

	*full {
			// two lines - for big editor
		^[\name, \type, \CLR, \reset, \scope, \doc, \end, \fade, \poll, \rip,
			\monitorL, \playN, \pausR, \sendR, ]
	}

	*audio {
			// one line, for ProxyMixer, ar
		^[\monitorM, \playN, \name, \pausR, \sendR, \ed]
	}

	*control {
			// one short line - for ProxyMixer, kr
		^[\name, \pausR, \sendR, \poll, \ed]
	}

	setDefaults { |options|
		var width = options.sum( buttonSizes[_] );
		var xpositions;
		var numLines = 1;
		if (width > 500) { // two lines, break them up near the middle
			numLines = 2;
			xpositions = options.collect( NdefGui.buttonSizes[_] ).integrate;
			width = width * 0.5 + (xpositions.absdif(width * 0.5).minItem);
		};

		if (numItems > 0) { width = width.max(320) }; // min width of NdefParamGui
		defPos = 300@20;
		minSize = width @ (numItems * skin.buttonHeight + (skin.buttonHeight * numLines));

	//	"NdefGui - width: % minSize: %\n".postf(width, minSize);

		if (parent.notNil) { skin = skin.copy.margin = 0@0; };

		this.makeButFuncs(options, skin.buttonHeight);

	}



	makeButFuncs { |options, height|
		buttonFuncs = (
			name: 	{ this.makeNameView(buttonSizes[\name], height) },
			type: 	{ this.makeTypeView(buttonSizes[\type], height) },
			CLR: 	{ this.makeClrBut(buttonSizes[\CLR], height) },
			reset: 	{ this.makeResetBut(buttonSizes[\reset], height) },
			scope: 	{ this.makeScopeBut(buttonSizes[\scope], height) },
			doc: 	{ this.makeDocBut(buttonSizes[\doc], height) },
			end: 	{ this.makeEndBut(buttonSizes[\end], height) },
			fade: 	{ this.makeFadeBox(buttonSizes[\fade], height) },
			monitor: 	{ this.makeMonitor(buttonSizes[\monitor], height, options) },
			monitorM:{ this.makeMonitor(buttonSizes[\monitorM], height, options) },
			monitorL:{ this.makeMonitor(buttonSizes[\monitorL], height, options) },
			pausR: 	{ this.makePauseBut(buttonSizes[\pausR], height) },
			sendR: 	{ this.makeSendBut(buttonSizes[\sendR], height) },
			rip: 	{ this.makeRipBut(buttonSizes[\rip], height) },
			ed: 		{ this.makeEdBut(buttonSizes[\ed], height) },
			poll: 	{ this.makePollBut(buttonSizes[\poll], height) },
			wake: 	{ this.makeWakeBut(buttonSizes[\wake], height) }
		)
	}

	accepts { |obj|
		^(obj.isNil or: { obj.isKindOf(NodeProxy) })
	}

	makeViews { |options|
		var lineBreakIndex, hasName, hasMonitor, resizer, butLines;
	//	"NdefGui - zone.bounds: % zone.decorator.margin: %\n".postf(zone.bounds, zone.decorator.margin);

		options.do { |option| buttonFuncs[option].value; };


			// a clumsy way to figure out how to set resizes for all children.
		lineBreakIndex = zone.children.detectIndex { |a, i|
			var b = zone.children[i + 1];
			b.notNil and: { b.bounds.left < a.bounds.left }
		};

		butLines = if (lineBreakIndex.isNil) {
			[zone.children]
		} {
			[zone.children.keep(lineBreakIndex + 1), zone.children.drop(lineBreakIndex + 1)]
		};

		butLines.do { |butLine|
			var resizer = 1;
			var resizeIndex;
			var monIndex = if (monitorGui.notNil) { butLine.detectIndex(_ == monitorGui.zone) };
			var nameIndex = if (nameView.notNil) { butLine.detectIndex(_ == nameView) };
			if (monIndex.notNil) {
				resizeIndex = monIndex;
			} {
				if (nameIndex.notNil) { resizeIndex = nameIndex; }
			};
			// [\monIndex, monIndex, \nameIndex, nameIndex, \resizeIndex, resizeIndex].postcs;

			if (resizeIndex.notNil) {
				butLine.do { |element, i|
					if (i == resizeIndex) { resizer = 2 };
					if (i > resizeIndex) { resizer = 3 };
					// [i, element, resizer].postln;
					element.resize_(resizer);
				};
			};
		};

		if (numItems > 0) {
			zone.decorator.nextLine.shift(0, 2);
			zone.bounds;
			paramGui = NdefParamGui(object, numItems, zone, Rect(0,0, zone.bounds.width - (skin.margin.x * 2), 0));
		}
	}

	makeNameView { |nameWid, height|
		try { // QT temp fix
			nameView = DragBoth(zone, Rect(0,0, nameWid, height))
			.font_(font).align_(0)
			.receiveDragHandler_({
				var drag = View.currentDrag;
				if (drag.isKindOf(String)) { drag = drag.interpret };
				this.object_(drag);
			});
		} {
			nameView = TextView(zone, Rect(0,0, nameWid, height))
		    .font_(font)
		}
	}

	makeTypeView { |width, height|
		typeView = StaticText(zone, width@height).string_("-").align_(0)
			.font_(font).align_(0);
	}

	makeClrBut { |width, height|
		Button(zone, width@height).font_(font)
			.states_([[\CLR, skin.fontColor, Color.clear]])
			.action_({ arg btn, mod;
				if (mod.isAlt) { object.clear } {
					"Safety - use alt-click to clear object.".postln;
				}
			})
	}

	makeWakeBut { |width, height|
		wakeBut = Button(zone, width@height).font_(font)
			.states_([[\WAKE, skin.fontColor, Color.clear],
				[\WAKE, skin.fontColor, skin.onColor]])
			.action_({ object.resume.wakeUp; wakeBut.value_(1); })
	}

	makeResetBut { |width, height|
		Button(zone, width@height).font_(font)
			.states_([[\reset, skin.fontColor, Color.clear]])
			.action_({ |view, mod|
				object !? {
					if (mod.notNil and: { mod.isAlt }) {
						this.object.resetNodeMap;
					} {
						this.object.cleanNodeMap;
					};
					this.checkUpdate;
				}
			})
	}

	makeScopeBut { |width, height|
		Button(zone, width@height).font_(font)
			.states_([[\scope, skin.fontColor, Color.clear]])
			.action_({ object !? { object.scope } })
	}

	makeDocBut { |width, height|
		Button(zone, width@height).font_(font)
			.states_([[\doc, skin.fontColor, Color.clear]])
			.action_({ |but, mod|
				if (object.notNil) {
				if (mod.isAlt) {
					try { ProxySpace.findSpace(object).document(object.key) };
				} {
					object.document;
				} }
			})
	}

	makeEndBut { |width, height|
		Button(zone, width@height).font_(font)
			.states_([[\end, skin.fontColor, Color.clear]])
			.action_({ object !? {  object.end } })
	}

	makeFadeBox { |width = 60, height = 18|
		fadeBox = EZNumber(zone, width@height, \fade, \fadePx,
				{ |num| try { object.fadeTime_(num.value) } },
				try { object.fadeTime } ? 0.02,
				labelWidth: 28,
				numberWidth: width - 28
		);

		fadeBox.labelView.font_(font).background_(Color.clear);
		fadeBox.numberView.font_(font).background_(Color.clear);
	}

	makeMonitor { |width, height, npOptions|
		var monOptions = npOptions.sect([\level, \playN]);
		monitorGui = MonitorGui(object, zone, width@height, false, monOptions);
	}

	makePauseBut { |width, height|
		pauseBut = Button(zone, width@height).font_(font)
			.states_([
				["paus", skin.fontColor, skin.onColor],
				["rsum", skin.fontColor, skin.offColor]
			])
			.action_({ arg btn; object !? {
				[ { object.resume; }, { object.pause; }  ].at(btn.value).value;
			} });
	}

	makeSendBut { |width, height|
		sendBut = Button(zone, width@height).font_(font)
			.states_([
				["send", skin.fontColor, skin.offColor],
				["send", skin.fontColor, skin.onColor]
			])
			.action_({ arg btn, mod;
				var alt = mod.notNil and: { mod.isAlt };
				if(object.notNil and: (btn.value == 0)) {
					if (alt) { object.rebuild } { object.send }
				};
				btn.value_(1 - btn.value)
			})
	}

	makeEdBut { |width, height|
		edBut = Button(zone, width@height).font_(font)
			.states_([['ed', skin.fontColor, Color.clear], ['ed', skin.fontColor, skin.onColor]])
	}

	makeRipBut { |width, height|
		Button(zone, width@height).font_(font)
			.states_([['^', skin.fontColor, Color.clear]])
			.action_({ this.class.new(object, numItems) })
	}

	makePollBut { |width, height|
		Button(zone, width@height).font_(font)
			.states_([[\poll, skin.fontColor, Color.clear]])
			.action_({  object !? {
				object.bus.getn(action: { |arr| (object.asCompileString + "poll:" + arr).postln })
			} })
	}

	getState {
		var newState = (
			object: object,
			name: \_none_,
			type: "-",
			isPlaying: object.isPlaying,
			isPaused: false,
			canSend: false,
			fadeTime: 0.02
		);

		if (object.isNil) { ^newState };

		newState.putPairs([
			\name, object.key,
			\type, object.typeStr,
			\isPaused, object.paused,
			\canSend, object.sources.notNil,
			\fadeTime, object.fadeTime,
			\isPlaying, object.isPlaying
		]);

		^newState
	}

	checkUpdate {
		var newState = this.getState;

		if (monitorGui.notNil) { monitorGui.checkUpdate };
		if (paramGui.notNil) { paramGui.checkUpdate };

		// update common stuff first
		if (newState[\object] != prevState[\object]) {
			if (monitorGui.notNil) { monitorGui.object_(object) };
			if (paramGui.notNil) { paramGui.object_(object) };
		};

		if (newState[\name] != prevState[\name]) {
			this.name_(newState[\name])
		};

		if (typeView.notNil) {
			if (newState[\type] != prevState[\type]) {
				typeView.string_(newState[\type])
			}
		};

		if (fadeBox.notNil) {
			if (newState[\fadeTime] != prevState[\fadeTime]) {
				fadeBox.value_(newState[\fadeTime])
			}
		};

		if (pauseBut.notNil) {
			if (newState[\isPaused] != prevState[\isPaused]) {
				pauseBut.value_(newState[\isPaused].binaryValue)
			}
		};

		if (sendBut.notNil) {
			if (newState[\canSend] != prevState[\canSend]) {
				sendBut.value_(newState[\canSend].binaryValue)
			}
		};
		if (wakeBut.notNil) {
			if (newState[\isPlaying] != prevState[\isPlaying]) {
				wakeBut.value_(newState[\isPlaying].binaryValue)
			}
		};

		prevState = newState;
	}

		// support overwriting the param names shown, e.g. for ProxyChain
	addReplaceKey { |replaced, replacer, spec|
		if (paramGui.notNil) { paramGui.addReplaceKey(replaced, replacer, spec) }
	}

	removeReplaceKey { |replaced|
		if (paramGui.notNil) { paramGui.removeReplaceKey(replaced) }
	}
}

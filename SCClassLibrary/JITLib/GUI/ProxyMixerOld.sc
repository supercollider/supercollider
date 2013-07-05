// idea by adc, mods by /f0, jrh, mc, ...

ProxyMixerOld {
		// proxymixer need not be built into a supplied window,
		// so just call the parent window.
	var <window, <zone, <proxyspace, <>nProxies;
	var <activeNodes;
	var <title;

	var <editZoneBtn, <pxMons, <editBtnsAr, <editBtnsKr, <buttonLinesKr;
	var <compArZone, <compKrZone;

	var origBounds, fullBounds, editZoneOpen=false, <selectMethod=\existingProxies;
	var <skippy, <editor, <recorder;
	var <skin, <font;
	var <prevKrNames, <prevArNames;
	var <scrollyAr, <keysRotationAr=0;
	var <scrollyKr, <keysRotationKr=0;

	*initClass {
		Class.initClassTree(GUI);
		GUI.skins.put(\jitSmall, (
				fontSpecs: 	["Helvetica", 10],
				fontColor: 	Color.black,
				background: 	Color(0.8, 0.85, 0.7, 0.5),
				foreground:	Color.grey(0.95),
				onColor:		Color(0.5, 1, 0.5),
				offColor:		Color.clear,
				gap:			0 @ 0,
				margin: 		2@2,
				buttonHeight:	16
			)
		);
	}

	w {
		warn("" ++ this.class ++ ":w is deprecated. use .window instead.")
		^window
	}

	*new { arg proxyspace, nProxies = 16, title, where;
		^super.new.init(proxyspace, nProxies,
			title ?? { format("proxyspace: %", proxyspace.tryPerform(\name) ? "") }, where
		);
	}

	init { arg space, nPxs = 16, argTitle, where;

		var winHeight, height, color, bounds;
		skin = GUI.skins[\jitSmall];
		font = GUI.font.new(*skin.fontSpecs);

		title = argTitle;
		bounds = ((where ? (10@250)).asArray ++ [0,0]).asRect;

		proxyspace = space ? currentEnvironment;
		if(proxyspace.isKindOf(ProxySpace).not) { Error("no proxy space present!").throw };
		nProxies = nPxs;

		height = nProxies + 1 * (skin.buttonHeight) + 16;

		// global layout:

		origBounds = bounds.resizeTo(612, height + 10);
		fullBounds = origBounds.resizeBy(345, 0);
		this.makeWindow;

		zone = CompositeView(window, fullBounds.moveTo(0,0));
		zone.decorator =  FlowLayout(zone.bounds); //.gap_(6@6);

		compArZone = CompositeView.new(zone, Rect(0, 0, 400, height))
			.background_(skin.foreground);
		compKrZone = CompositeView.new(zone, Rect(0, 0, 200, height))
			.background_(skin.foreground);

		compArZone.decorator = FlowLayout(compArZone.bounds).gap_(skin.gap);
		compKrZone.decorator = FlowLayout(compKrZone.bounds).gap_(skin.gap);

		this.makeArZone;
		this.makeKrZone;
		this.makeEditZone;

		window.front;

		this.makeSkipJack;
		window.onClose_({ this.stopUpdate });
		this.runUpdate;

	}

	runUpdate { editor.runUpdate; skippy.start; }
	stopUpdate { editor.stopUpdate; skippy.stop }

	makeWindow {
		window = Window(this.title, origBounds);
		window.view.background = skin.background;

	}
	makeSkipJack {
		skippy = SkipJack({ this.updateZones }, 0.2, { window.isClosed }, title);
	}

	makeEditZone {
		editor = NodeProxyEditor(nil, nProxies - 1, zone);
	}

	openEditZone { arg flag;
			if(flag == 1 or: {editZoneOpen.not}) {
				if(editZoneOpen.not) {
					origBounds = window.bounds;
					window.bounds = window.bounds.setExtent(fullBounds.width, fullBounds.height);
					editZoneOpen = true;
					editZoneBtn.value = 1;
					this.update;
				}
			}{
				window.bounds = window.bounds.setExtent(origBounds.width, origBounds.height);
				editZoneOpen = false;
				editZoneBtn.value = 0;
			};
	}


	makeArZoneHead {

		GUI.popUpMenu.new(compArZone, Rect(10, 10, 110, skin.buttonHeight+2))
				.items_([\existingProxies, \activeProxies, \playingProxies])
				.action_({ arg view; selectMethod = view.items[view.value] })
				.font_(font);

		GUI.button.new(compArZone, Rect(10, 10, 50, skin.buttonHeight+2))
				.states_(
					[["reduce", skin.fontcolor, Color.clear]]				)
				.action_({ proxyspace.reduce }).font_(font);
		GUI.button.new(compArZone, Rect(10, 10, 30, skin.buttonHeight+2))
				.states_(
					[["doc", skin.fontcolor, Color.clear]]				)
				.action_({ proxyspace.document }).font_(font);
		GUI.button.new(compArZone, Rect(10, 10, 30, skin.buttonHeight+2))
				.states_(
					[["docc", skin.fontcolor, Color.clear]]				)
				.action_({
					proxyspace.document(this.selectedKeys)
				}).font_(font);
		/*
		GUI.button.new.new(compArZone, Rect(10, 10, 20, skin.buttonHeight+2)).states_([
				["#",  skin.fontcolor, Color.clear ]
			])
			.action_({ editor.pxKey !? { proxyspace.storeOn(Post, [editor.pxKey]) } })
			.font_(font);
		*/
		editZoneBtn = GUI.button.new(compArZone, Rect(10, 10, 60, skin.buttonHeight+2))
				.font_(font)
				.states_(
					[	["openEdit", skin.fontcolor, Color.clear],
						["closeEdit", skin.fontcolor, Color.clear]
					]				)
				.action_({ |b| this.openEditZone(b.value) });

		GUI.button.new(compArZone, Rect(10, 10, 20, skin.buttonHeight+2))
				.font_(font)
				.states_(
					[	["R", Color.red, Color.clear]					])
				.action_({  RecordProxyMixer(this, origBounds.resizeTo(472, 100)) });

	}

	makeArZone {
		var arLayout = compArZone.decorator;
		this.makeArZoneHead;

		arLayout.nextLine;
		arLayout.shift(0,4);

		#pxMons, editBtnsAr = Array.fill(nProxies, { arg i;
			var pxmon, edbut;
			pxmon = ProxyMonitorGui(nil, compArZone, 360@skin.buttonHeight, makeWatcher: false);			pxmon.zone.visible_(false);

		//	arLayout.shift(4);

			edbut = GUI.button.new(compArZone, Rect(0,0, 16, 16))
				.font_(font)
				.visible_(false)
				.states_([
					["ed", Color.black, Color.grey(0.75)],
					["ed", Color.black, Color.white]])
				.action_({ arg btn, mod;
					if (mod.notNil and: { mod.isAlt }) {
						NodeProxyEditor(pxmon.proxy);					} {
						editor.proxy_(pxmon.proxy);
						this.openEditZone(1);
						editBtnsAr.do { |b| b.value_(0) };
						editBtnsKr.do { |b| b.value_(0) };
						btn.value_(1);
					};
				});
			arLayout.nextLine;
			[pxmon, edbut]
		}).flop;

		scrollyAr = EZScroller.new(window,
			Rect(compArZone.bounds.right - 16, 30, 12, nProxies * skin.buttonHeight),
			nProxies, nProxies,
			{ |sc| keysRotationAr = sc.value.asInteger.max(0); }
		).value_(0).visible_(true);

	}

	makeKrZone {
		var layout = compKrZone.decorator;
		var emergencySynth;

		GUI.staticText.new(compKrZone, Rect(0, 0, 180, 18))
			.font_(font).align_(\center)
			.string_("kr proxies");

		layout.nextLine;
		layout.shift(0,4);

		/*
		GUI.button.new(compKrZone, Rect(0,0,20, skin.buttonHeight))
		.font_(Font("Helvetica-Bold", 12))
		.states_([["+", Color.red, Color.white]])
		.action_({
			if(emergencySynth.isPlaying.not) {
				emergencySynth = Synth.tail(0, "proxymixer_emergency");
				NodeWatcher.register(emergencySynth);
			} {
				emergencySynth.release
			}
		});
		*/
		layout.nextLine;
		layout.nextLine;

		buttonLinesKr = Array.fill(nProxies, { arg i;
			var nameBtn, pausBtn, sendBtn, editBtn, pollBtn;

			nameBtn = GUI.dragSource.new(compKrZone, Rect(0,0,72, skin.buttonHeight))
			.font_(font)
			.align_(\center);
			pausBtn = GUI.button.new(compKrZone, Rect(0,0, 30, skin.buttonHeight))
				.font_(font).states_([["paus", Color.black, skin.onColor],
				["rsum", Color.black, Color.clear]])
				.action_({ |btn|
					var editName = nameBtn.string.asSymbol;
					var px = proxyspace.envir[editName];
					if (px.notNil) {
						if (px.paused) { px.resume } { px.pause };
					};
				});
			sendBtn = GUI.button.new(compKrZone, Rect(0,0, 30, skin.buttonHeight))
				.font_(font).states_([["send", Color.black, skin.onColor]])
				.action_({ |btn|
					var editName = nameBtn.string.asSymbol;
					var px = proxyspace.envir[editName];
					if (px.notNil) { px.send };
				});

			pollBtn = GUI.button.new(compKrZone, Rect(0,0, 24, skin.buttonHeight))
				.font_(font).states_([["poll", Color.black, skin.foreground]])
				.action_({
					var editName = nameBtn.string.asSymbol;
					var px = proxyspace.envir[editName];
					if (px.notNil) {
						px.bus.getn(px.numChannels, { |arr| [editName, arr.unbubble].postln });
					};
				});
			editBtn = GUI.button.new(compKrZone, Rect(0,0,20, skin.buttonHeight)).states_([
				["ed", skin.fontcolor, Color.new(0.7,0.7,0.7,1)],
				["ed", skin.fontcolor, Color.white]])
				.action_({ arg btn;
					var editName = nameBtn.string.asSymbol;
					editor.proxy_(proxyspace.envir[editName]);
					this.update;
					this.openEditZone(1);
					editBtnsAr.do { |b| b.value_(0) };
					editBtnsKr.do { |b| b.value_(0) };
					btn.value_(1);
				}).font_(font);


			[ nameBtn, pausBtn, sendBtn, editBtn, pollBtn].do({ arg v; v.visible_(false) });
		});
		editBtnsKr = buttonLinesKr.collect(_[1]);
		layout.nextLine;

		scrollyKr = EZScroller.new(window,
			Rect(compKrZone.bounds.right - 16, 30, 12, nProxies * skin.buttonHeight),
			nProxies, nProxies,
			{ |sc| keysRotationKr = sc.value.asInteger.max(0); }
		).value_(0).visible_(true);

	}

	updateZones {
		this.updateArSlots;
		this.updateKrSlots;
	}

	highlightSlots { |parOffset, num|
		var onCol = Color(1, 0.5, 0.5);
		var offCol = Color.clear;
		{ pxMons.do { |moni, i|
			var col = if (i >= parOffset and: (i < (parOffset + num).max(0)), onCol, offCol);
			moni.nameView.background_(col.green_([0.5, 0.7].wrapAt(i - parOffset div: 2)));
		} }.defer;
	}

	updateKrSlots {
		var krProxyNames;
		krProxyNames = proxyspace.krProxyNames;

		if (krProxyNames.size > nProxies) {

			scrollyKr.visible_(true)
				.numItems_(krProxyNames.size)
				.value_(keysRotationKr ? (krProxyNames.size - nProxies));

			krProxyNames = krProxyNames.drop(keysRotationKr).keep(nProxies);
		} {
			scrollyKr.visible_(false);
		};

		if (krProxyNames != prevKrNames) {
			nProxies.do { arg i;
				var butLine, nameSink, key, px, showLine;
				key = krProxyNames[i];
				px = proxyspace.envir[ key ];

				butLine = buttonLinesKr[i];
				nameSink = butLine[0];
				nameSink.object_(px).string_(key ? "");

				showLine = key.notNil and: px.notNil;
				butLine.do(_.visible_(showLine));
			}
		};
		prevKrNames = krProxyNames;
	}

	existingProxies { ^proxyspace.arProxyNames }
	activeProxies { ^proxyspace.arProxyNames({ |px, key| px.isPlaying }) }
	playingProxies { ^proxyspace.arProxyNames({ |px, key| px.monitor.isPlaying }) }

	selectedKeys { ^this.perform(selectMethod) }

	updateArSlots {
		var arPxNames, pxKey, px, pxMon;
		arPxNames = this.selectedKeys;

		if (arPxNames.size > nProxies) {
			scrollyAr.visible_(true)
				.numItems_(arPxNames.size)
				.value_(keysRotationAr ? (arPxNames.size - nProxies));

			arPxNames = arPxNames.drop(keysRotationAr).keep(nProxies);
		} {
			scrollyAr.visible_(false);
		};

		if (arPxNames != prevArNames) {
			nProxies.do { arg i;
				pxMon = pxMons[i];
				pxKey = arPxNames[i];

				px = proxyspace.envir.at(pxKey);
				if(px.notNil) {
					pxMon.zone.visible_(true);
					editBtnsAr[i].visible_(true);
					pxMon.proxy_(px);
				}{
					pxMon.zone.visible_(false);
					editBtnsAr[i].visible_(false);
				}
			}
		};
		arPxNames.do { |name, i| pxMons[i].updateAll };

		prevArNames = arPxNames;
	}

	adjustWindowSize { arg n;
		n = n ? nProxies;
		window.bounds = if(editZoneOpen)
			{ origBounds.resizeBy(0, 330) }
			{ origBounds.copy.height_(n  * 18 + 80) };
	}
}


NdefMixerOld : ProxyMixerOld {

	*new { |server, nProxies = 16, title, bounds|
		var space;
		if (server.isKindOf(Symbol)) {
			server = Server.named.at(server);
			if (server.isNil) { Error("NdefMixer: no server named %.".format(server)).throw };
		} {
			server = server ? Ndef.defaultServer ? Server.default;
		};

		space = Ndef.dictFor(server);

		title = title ? ("Ndef:" + server.name);
		^super.new(space, nProxies, title, bounds);
	}
}

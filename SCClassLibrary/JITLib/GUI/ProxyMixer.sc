ProxyMixer : JITGui {

	var <sizes;
	var <arZone, <krZone, <editZone;
	var <arGuis, <krGuis, <editGui;
	var <arScroller, <krScroller;
	var <arKeysRotation = 0, <krKeysRotation = 0;

	var selectMethod = \existingProxies;

		// easy access methods
	atAr { |index| ^prevState[\krNames][index] }
	atKr { |index| ^prevState[\arNames][index] }
	arNames { ^prevState[\arNames] }
	krNames { ^prevState[\krNames] }
	numArs { ^prevState[\arNames].size }
	numKrs { ^prevState[\krNames].size }

		// backwards compatibility
	proxyspace { ^object }
	proxyspace_ { |obj| this.object_(obj) }

	highlight { |index, prefix|
		arGuis[index].highlightName(prefix);
	}

	unhighlight {|index|
		arGuis[index].unhighlightName;
	}

	highlightSlots { |parOffset, num, highNames=#[], clearOthers = true|
		var onCol = skin.onColor2;
		var offCol = skin.offColor;
		var highIndices = (0..num-1) + parOffset;

		{
			arGuis.do { |argui, i|
				if (highIndices.includes(i)) {
					argui.highlightName(highNames[i - parOffset] ? "");
				} {
					if (clearOthers) { argui.unhighlightName; };
				};
			};
		}.defer;
	}

	title { ^this.parent.name }


	accepts { |obj|
		^obj.isNil or: { obj.isKindOf(ProxySpace) }
	}

	*new { |obj, numItems = 16, parent, bounds, makeSkip = true, options|
		^super.new(obj, numItems, parent, bounds, makeSkip, options);
	}

	* small { |obj, numItems = 16, parent, bounds, makeSkip = true|
		^this.new(obj, numItems, parent, bounds, makeSkip, [\small]);
	}

	setDefaults { |options|
		var width = 600;
		var height = numItems * skin.buttonHeight + skin.headHeight + 25;

		skin = GUI.skins.jit;
		font = Font(*skin.fontSpecs);

		defPos = 10@260;

		if (options.notNil and: { options.includes(\small) }) {
			sizes = (
				small: (396 @ height),
				mid: (626 @ height),
				big: (800 @ height)
			);
		} {
			sizes = (
				small: (446 @ height),
				mid: (676 @ height),
				big: (1080 @ height)
			);
		};

		minSize = sizes[\big];
	}

	makeViews { |options|
		var isSmall = options.notNil and: { options.includes(\small) };
		var openEditBut;
		var arZoneWidth = if (isSmall, 444 - 50, 444);

		parent.bounds_(parent.bounds.extent_(sizes[\mid] + (8@8)));

		zone.decorator.gap_(4@4);
		zone.resize_(1).background_(skin.background);
		arZone = CompositeView(zone, Rect(0, 0, arZoneWidth, sizes[\mid].y ))
			.background_(skin.foreground);
		arZone.addFlowLayout(skin.margin, skin.gap);

		krZone = CompositeView(zone, Rect(0, 0, 225, sizes[\mid].y ))
			.background_(skin.foreground);
		krZone.addFlowLayout(skin.margin, skin.gap);

		this.makeTopLine;
		openEditBut = arZone.children[4];

		arZone.decorator.nextLine.shift(0, 10);
		this.makeArZone(isSmall);

		this.makeKrZone;
		this.setEdButs(isSmall);

		if (isSmall) {
			// put editGui in the same place as krZone
			zone.decorator.left_(krZone.bounds.left).top_(krZone.bounds.top);
				// change openEditButton action:
			openEditBut.action = { |but| this.switchSize(but.value, true) };
		};

		editZone = CompositeView(zone, Rect(0, 0, 400, sizes[\mid].y ))
			.background_(skin.foreground);
		editZone.addFlowLayout(0@0, 0@0);

		if (isSmall) { editZone.visible_(false) };

		this.makeEditZone;

	}

	makeTopLine {

		PopUpMenu(arZone, Rect(10, 10, 110, skin.headHeight))
				.items_([\existingProxies, \activeProxies, \playingProxies])
				.action_({ arg view; selectMethod = view.items[view.value] })
				.font_(font);

		Button(arZone, Rect(10, 10, 50, skin.headHeight))
				.states_(
					[["reduce", skin.fontColor, skin.offColor]]				)
				.action_({ object !? { object.reduce } }).font_(font);
		Button(arZone, Rect(10, 10, 30, skin.headHeight))
				.states_(
					[["doc", skin.fontColor, skin.offColor]]				)
				.action_({ object !? { object.document } }).font_(font);
		Button(arZone, Rect(10, 10, 45, skin.headHeight))
				.states_(
					[["docSel", skin.fontColor, skin.offColor]]				)
				.action_({
					object !? { object.document(this.selectedKeys) }
				}).font_(font);

		Button(arZone, Rect(10, 10, 60, skin.headHeight))
				.font_(font)
				.states_([
						["openKr", skin.fontColor, skin.offColor],
						["openEdit", skin.fontColor, skin.offColor],
						["closeEdit", skin.fontColor, skin.offColor]
					])
				.value_(1)
				.action_({ |b| this.switchSize(b.value) });

		Button(arZone, Rect(10, 10, 50, skin.headHeight))
				.font_(font)
				.states_(
					[	["Record", skin.fontColor, skin.onColor2]					])
				.action_({ RecordProxyMixer(this, parent.bounds.resizeTo(472, 100)) });

	}

	switchSize { |index, hideZones = false|
		parent.bounds_(parent.bounds.extent_(sizes[[\small, \mid, \big][index]] + (6@10)));
		if (hideZones) {
			index.asInteger.switch(
				0, { krZone.visible_(false); editZone.visible_(false) },
				1, { krZone.visible_(true);  editZone.visible_(false) },
				2, { krZone.visible_(false); editZone.visible_(true)  }
			);
		};
	}

	setEdButs { |isSmall = false|
		(arGuis ++ krGuis).do { |pxgui|
			pxgui.edBut.states_([
					["ed", skin.fontColor, skin.background],
					["ed", skin.fontColor, skin.onColor]])

				.action_({ arg btn, mod;
					if (mod.notNil and: { mod.isAlt }) {
						NdefGui(pxgui.object);
					} {
						this.switchSize(2, isSmall);
						editGui.object_(pxgui.object);
						arGuis.do { |gui| gui.edBut.value_(0) };
						krGuis.do { |gui| gui.edBut.value_(0) };
						btn.value_(1);
					};
				});
		};
	}

	makeArZone { |isSmall = false|
		var ndefOptions = if (isSmall) { NdefGui.audioSm } { NdefGui.audio };
		var arLayout = arZone.decorator;

		var dim = ((arZone.bounds.width - 20)@skin.buttonHeight);

		arLayout.nextLine;
		arLayout.shift(0,4);

		arGuis = numItems.collect {
			NdefGui(nil, 0, arZone, dim, makeSkip: false, options: ndefOptions)
		};

		arLayout.top_(40).left_(arZone.bounds.width - 15);
		arScroller = EZScroller.new(arZone,
			Rect(0, 0, 12, numItems * skin.buttonHeight),
			numItems, numItems,
			{ |sc| arKeysRotation = sc.value.asInteger.max(0); this.checkUpdate }
		).value_(0).visible_(true);
	}

	makeKrZone {
		var krLayout = krZone.decorator;

		StaticText.new(krZone, Rect(0, 0, 180, 24))
			.font_(font).align_(\center)
			.string_("Control Proxies");

		krLayout.nextLine;
		krLayout.shift(0,4);
		krZone.decorator.nextLine.shift(0, 10);

		krGuis = numItems.collect {
			NdefGui(nil, 0, krZone, 180@18, makeSkip: false, options: NdefGui.control)
		};


		krLayout.top_(40).left_(krZone.bounds.width - 15);

		krScroller = EZScroller.new(krZone,
			Rect(0, 0, 12, numItems * skin.buttonHeight),
			numItems, numItems,
			{ |sc| krKeysRotation = sc.value.asInteger.max(0); this.checkUpdate }
		).value_(0).visible_(true);

	}

	makeEditZone {
		editGui = NdefGui(nil, numItems, editZone, makeSkip: false, options: NdefGui.big);
	}

	existingProxies { ^object !? { object.arProxyNames } ? [] }
	activeProxies { ^object !? { object.arProxyNames({ |px, key| px.isPlaying }) } ? [] }
	playingProxies { ^object !? { object.arProxyNames({ |px, key| px.monitor.isPlaying }) } ? [] }

	selectedKeys { ^this.perform(selectMethod) }

	getState {
		var arNames, krNames;

		var newState = (
			object: object,
			name: '-',
			arNames: [],
			krNames: [],
			editedName: nil,
			arOverflow: 0,
			krOverflow: 0
		);
		if (object.notNil) {
			arNames = this.selectedKeys.asArray;
			krNames = object.krProxyNames.asArray;

			newState.putPairs([
				\name, object.asCode,
				\arNames, arNames,
				\krNames, krNames,
				\editedName, editGui.object !? { editGui.object.key },
				\arOverflow, (arNames.size - numItems).max(0),
				\krOverflow, (krNames.size - numItems).max(0)
			]);
		};
		^newState;
	}

	checkUpdate {
		var newState = this.getState;

		var arNames, prevArNames, fullSize;
		var krNames, prevKrNames;
		var editName, prevEditName;

		if (object.isNil) {
			arGuis.do { |g| g.object_(nil); g.zone.visible_(false) };
			krGuis.do { |g| g.object_(nil); g.zone.visible_(false) };
			[arScroller, krScroller].do(_.visible_(false));
			this.name_(newState[\name]);
			prevState = newState;
		//	editGui.object_(nil);
			^this
		};

		if (newState[\name] != prevState[\name]) {
			this.name_(newState[\name])
		};

		arNames = newState[\arNames];
		prevArNames = prevState[\arNames];
		fullSize = arNames.size;

		if (newState[\arOverflow] > 0) {
			arNames = arNames.drop(arKeysRotation).keep(numItems);
			newState[\arNames] = arNames;
		} {
			arKeysRotation = 0;
		};

		arKeysRotation = min(arKeysRotation, newState[\arOverflow]);
		arScroller.numItems_(fullSize).value_(arKeysRotation).visible_(newState[\arOverflow] > 0);


		if (arNames != prevArNames) {
			arGuis.do { |argui, i|
				var newName = arNames[i];
				var newPx = object.envir[newName];
				argui.object_(newPx);
				argui.zone.visible_(newPx.notNil);
			};
		};
		arGuis.do { |gui|
			var pxIsEdited = gui.object.notNil and: { gui.object == editGui.object };
			gui.checkUpdate;
			if(gui.hasName.not) { gui.name = this.proxyspace.findKeyForValue(gui.object) };
			gui.edBut.value_(pxIsEdited.binaryValue);
		};


		krNames = newState[\krNames];
		prevKrNames = prevState[\krNames];
		fullSize = krNames.size;

		if (newState[\krOverflow] > 0) {
			krNames = krNames.drop(krKeysRotation).keep(numItems);
			newState[\krNames] = krNames;

		} {
			krKeysRotation = 0;
		};
		krKeysRotation = min(krKeysRotation, newState[\krOverflow]);
		krScroller.numItems_(fullSize)
			.value_(krKeysRotation).visible_(newState[\krOverflow] > 0);

		if (krNames != prevKrNames) {
			krGuis.do { |krgui, i|
				var newName = krNames[i];
				var newPx = object.envir[newName];
				krgui.object_(newPx);
				krgui.zone.visible_(newPx.notNil);
			};
		};

		krGuis.do { |gui|
			var pxIsEdited = gui.object.notNil and: { gui.object == editGui.object };
			gui.checkUpdate;
			if(gui.hasName.not) { gui.name = this.proxyspace.findKeyForValue(gui.object) };
			gui.edBut.value_(pxIsEdited.binaryValue);
		};

		editGui.checkUpdate;

		prevState = newState;
	}
}

NdefMixer : ProxyMixer {
	object_ { |obj|
		obj = Server.named.at(obj) ? obj;
		if (obj.isKindOf(Server)) {
				super.object_(Ndef.dictFor(obj));
		}
	}
}

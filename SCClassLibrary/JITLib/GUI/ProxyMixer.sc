ProxyMixer : JITGui {

	var <sizes;
	var <arZone, <krZone, <editZone;
	var <arGuis, <krGuis, <editGui;
	var <arScroller, <krScroller;
	var 	<arKeysRotation = 0, <krKeysRotation = 0;

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
	editor { ^editGui }
	pxMons { ^arGuis }		// should work in some cases

	highlightSlots { |parOffset, num|
		var onCol = Color(1, 0.5, 0.5);
		var offCol = Color.clear;
		{ arGuis.do { |argui, i|
			var col = if (i >= parOffset and: (i < (parOffset + num).max(0)), onCol, offCol);
			argui.nameView.background_(col.green_([0.5, 0.7].wrapAt(i - parOffset div: 2)));
		} }.defer;
	}

	title { ^this.parent.name }


	accepts { |obj|
		^obj.isNil or: { obj.isKindOf(ProxySpace) }
	}

	*new { |obj, numItems = 16, parent, bounds, makeSkip = true, options|
		^super.new(obj, numItems, parent, bounds, makeSkip, options);
	}

	setDefaults {
		var width = 600;
		var height = numItems * skin.buttonHeight + skin.headHeight + 20;

		skin = GUI.skins.jit;
		font = Font(*skin.fontSpecs);

		defPos = 10@260;

		sizes = (
			small: (446 @ height),
			mid: (676 @ height),
			big: (1090 @ height)
		);
		minSize = sizes[\big];
	}

	makeViews {
		parent.bounds_(parent.bounds.extent_(sizes[\mid] + (8@8)));

		zone.decorator.gap_(6@6);
		zone.resize_(1).background_(Color.grey(0.7));
		arZone = CompositeView(zone, Rect(0, 0, 444, sizes[\mid].y ))
			.background_(skin.foreground);
		arZone.addFlowLayout(skin.margin, skin.gap);

		krZone = CompositeView(zone, Rect(0, 0, 225, sizes[\mid].y ))
			.background_(skin.foreground);
		krZone.addFlowLayout(skin.margin, skin.gap);

		editZone = CompositeView(zone, Rect(0, 0, 400, sizes[\mid].y ))
			.background_(skin.foreground);
		editZone.addFlowLayout(skin.margin, skin.gap);

		this.makeTopLine;
		arZone.decorator.nextLine.shift(0, 10);
		this.makeArZone;

		this.makeKrZone;
		this.setEdButs;

		this.makeEditZone;

		this.switchSize(1); // show kr proxies, but not editor
	}

	makeTopLine {

		PopUpMenu(arZone, Rect(10, 10, 110, skin.headHeight))
				.items_([\existingProxies, \activeProxies, \playingProxies])
				.action_({ arg view; selectMethod = view.items[view.value] })
				.font_(font);

		Button(arZone, Rect(10, 10, 50, skin.headHeight))
				.states_(
					[["reduce", skin.fontcolor, Color.clear]]				)
				.action_({ object !? { object.reduce } }).font_(font);
		Button(arZone, Rect(10, 10, 30, skin.headHeight))
				.states_(
					[["doc", skin.fontcolor, Color.clear]]				)
				.action_({ object !? { object.document } }).font_(font);
		Button(arZone, Rect(10, 10, 45, skin.headHeight))
				.states_(
					[["docSel", skin.fontcolor, Color.clear]]				)
				.action_({
					object !? { object.document(this.selectedKeys) }
				}).font_(font);

		Button(arZone, Rect(10, 10, 60, skin.headHeight))
				.font_(font)
				.states_([
						["openKr", skin.fontcolor, Color.clear],
						["openEdit", skin.fontcolor, Color.clear],
						["closeEdit", skin.fontcolor, Color.clear]
					])
				.value_(1)
				.action_({ |b| this.switchSize(b.value) });

		Button(arZone, Rect(10, 10, 50, skin.headHeight))
				.font_(font)
				.states_(
					[	["Record", Color.red, Color.clear]					])
				.action_({ RecordProxyMixer(this, parent.bounds.resizeTo(472, 100)) });

	}

	switchSize { |index|
		parent.bounds_(parent.bounds.extent_(sizes[[\small, \mid, \big][index]] + (8@12)));
	}

	setEdButs {
		(arGuis ++ krGuis).do { |pxgui|
			pxgui.edBut.states_([
					["ed", Color.black, Color.grey(0.75)],
					["ed", Color.black, Color.white]])

				.action_({ arg btn, mod;
					if (mod.notNil and: { mod.isAlt }) {
						NdefGui(pxgui.object);
					} {
						this.switchSize(2);
						editGui.object_(pxgui.object);
						arGuis.do { |gui| gui.edBut.value_(0) };
						krGuis.do { |gui| gui.edBut.value_(0) };
						btn.value_(1);
					};
				});
		};
	}

	makeArZone {
		var arLayout = arZone.decorator;

		var dim = ((arZone.bounds.width - 20)@skin.buttonHeight);

		arLayout.nextLine;
		arLayout.shift(0,4);

		arGuis = numItems.collect { NdefGui(nil, 0, arZone, dim, makeSkip: false, options: NdefGui.audio) };

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

ProxyMonitorGui {
	classvar <>lastOutBus = 99;

	var <proxy, <usesPlayN = false, <usesName, <usesPausSend;
	var <win, <zone, <flow;
	var <ampSl, <playBut, <nameView, <setOutBox, <playNDialogBut, <pauseBut, <sendBut;
	var <skipjack, <oldState = #[];

	*new { |proxy, w, bounds, showLevel=false, showPlayN=true, showName=true, showPauseSend = true,
		makeWatcher=true, skin|

		^super.new.init(w, bounds, showLevel, showPlayN, showName, showPauseSend, makeWatcher, skin)
			.proxy_(proxy);
	}
	proxy_ { |inproxy|
		if (proxy.isNil or: proxy.isKindOf(NodeProxy)) {
			proxy = inproxy;
			this.updateAll;
		} {
			warn("ProxyMonitorGui: % is not a nodeproxy.".format(inproxy))
		};
	}

	init { |w, bounds, showLevel, showPlayN, showName, showPauseSend, makeWatcher, skin|
		var font;
		var nameWid = 60;
		var playWid = 34;
		var outWid = 20;
		var playNWid = 20;
		var pausSendWid = playWid * 2;
		var defaultBounds = 400@20;
		var height;
		var widthSum, sliderWidth, winBounds, viewBounds;

		skin = skin ? GUI.skins[\jitSmall];
		font = GUI.font.new(*skin.fontSpecs);

		usesPlayN = showPlayN;
		usesName = showName;
		usesPausSend = showPauseSend;
		bounds = bounds ? defaultBounds;

		if (w.notNil) {
			win = w;
			viewBounds = bounds ? defaultBounds;
		} {
			viewBounds = bounds;
			if (bounds.isKindOf(Point)) {
				winBounds = Rect(80,400,0,0).setExtent(bounds.x, bounds.y);
			} {
				winBounds = bounds;
			};
		//	[\winBounds, winBounds, \viewBounds, 	viewBounds].postln;
			win = Window(this.class.name.asString, winBounds, false).front;
		};


		zone = GUI.compositeView.new(win, viewBounds);
		zone.background_(skin.foreground);
		flow = FlowLayout(zone.bounds, 0@0, 0@0);
		zone.decorator = flow;
		zone.resize_(2);

		if (viewBounds.isKindOf(Rect)) { viewBounds = viewBounds.extent };

	//	[\bounds, bounds, \winBounds, winBounds, \viewBounds, viewBounds].postln;
		widthSum =
			(showName.binaryValue * nameWid)
			+ playWid + outWid
			+ (showPlayN.binaryValue * playNWid)
			+ (showPauseSend.binaryValue * pausSendWid);
		sliderWidth = viewBounds.x - widthSum;

		height = viewBounds.y;

		ampSl = EZSlider(zone, (sliderWidth @ height), \vol, \db,
			{ arg slid;
				if(proxy.notNil) {
					proxy.vol_(slid.value.dbamp);
				}
			}, 0, false,
			labelWidth: showLevel.binaryValue * 20,
			numberWidth: showLevel.binaryValue * 40);
		ampSl.labelView.font_(font).align_(0);
		ampSl.view.resize_(2);


			// should have four states: ...

		playBut = Button(zone, Rect(0,0,playWid, height))
			.font_(font).resize_(3)
			.states_([
				[ if (usesPlayN, \playN, \play), skin.fontColor, skin.offColor],
				[ \stop, skin.fontColor, skin.onColor ]
			]);

		playBut.action_({ arg btn, modif;
			if(proxy.notNil) {
				[ 	{ 	if (modif.isAlt) { proxy.end } { proxy.stop }; },
					{ 	if (modif.isAlt) { proxy.vol_(0) };
						if (usesPlayN) { proxy.playN } { proxy.play }
					}
				].at(btn.value).value
			}
		});

		setOutBox = EZNumber(zone, outWid@height, nil, [0, lastOutBus, \lin, 1],
			{ |box, mod|
				if (proxy.notNil) {
					if (proxy.monitor.isNil) {
						"ProxyMonitorGui - cant set outs yet.".postln
					} {
						proxy.monitor.out_(box.value.asInteger);
					};
				};
			}, 0);

		setOutBox.view.resize_(3);
		setOutBox.numberView.font_(font).align_(0);

		if (usesPlayN) {
			playNDialogBut = GUI.button.new(zone, Rect(0,0, playNWid, height))
				.font_(font).resize_(3)
				.states_([
					["-=", skin.fontColor, skin.offColor],
					["-<", skin.fontColor, skin.onColor]
				])
				.action_({ |box, mod|
					if (proxy.notNil) { proxy.playNDialog };
					box.value_(1 - box.value);
				});
		};

		if (usesName) {
			nameView = DragBoth(zone, Rect(0,0, nameWid, height));
			nameView.font_(font).align_(0).resize_(3)
				.setBoth_(false)
				.receiveDragHandler = { this.proxy_(View.implClass.currentDrag) };
		};

		if (usesPausSend) {
			pauseBut = GUI.button.new(zone, Rect(0,0,34,height))
				.font_(font).resize_(3)
				.states_([
					["paus", skin.fontColor, skin.onColor],
					["rsum", skin.fontColor, skin.offColor]
				])
				.action_({ arg btn;
				if(proxy.notNil, {
						[ 	{ proxy.resume; }, { proxy.pause; }  ].at(btn.value).value;
					})
				});

			sendBut = Button(zone, Rect(0,0,34,height))
				.font_(font).resize_(3)
				.states_([
					["send", skin.fontColor, skin.offColor],
					["send", skin.fontColor, skin.onColor]
				])
				.action_({ arg btn, mod;
					if(proxy.notNil and: (btn.value == 0)) {
						if (mod.isAlt) { proxy.rebuild } { proxy.send }
					};
					btn.value_(1 - btn.value)
				})
		};

		if (makeWatcher) { this.makeWatcher };
	}

	makeWatcher {
		skipjack.stop;
		skipjack = SkipJack({ this.updateAll },
			0.5, 			{ win.isClosed },			("ProxyMon" + try { proxy.key }).asSymbol		);		skipjack.start;	}		updateAll {		var monitor, outs, amps, newHasSeriesOut;

		var currState;
		var currVol=0, pxname='<no proxy>', isAudio=false, plays=0, playsSpread=false, pauses=0, canSend=0;
		var type = "-";

		if (win.isClosed) {skipjack.stop; ^this };

		if (proxy.notNil) {

			pxname = proxy.key ? 'anon';
			type = proxy.typeStr;
			canSend = proxy.objects.notEmpty.binaryValue;
			pauses = proxy.paused.binaryValue;

			isAudio = proxy.rate == \audio;
			monitor = proxy.monitor;
			plays = monitor.isPlaying.binaryValue;

			if (monitor.notNil, {
				currVol = proxy.monitor.vol;
				playsSpread = proxy.monitor.hasSeriesOuts.not;
				outs = monitor.outs;
			});
		};

		currState = [currVol, pxname, isAudio, plays, outs, playsSpread, pauses, canSend];

		if (currState != oldState) {
		//	"ProxyMonitorGui - updating.".postln;
			if (currVol.notNil) { ampSl.value_(currVol.ampdb) };
			if (usesName) { nameView.object_(proxy).string_(pxname) };

			playBut.value_(plays);
			if (usesPausSend) {
				pauseBut.value_(pauses);
				sendBut.value_(canSend);
			};

			if (isAudio != oldState[2]) {
				[ampSl, playBut, setOutBox, playNDialogBut].reject(_.isNil).do(_.enabled_(isAudio));
			};
				// dont update if typing into numberbox - should be tested with SwingOSC!
			if (setOutBox.numberView.hasFocus.not) {
				setOutBox.value_(try { outs[0] } ? 0);
				if (usesPlayN) {
					playNDialogBut.value_(playsSpread.binaryValue)
				};
			}
		};
		oldState = currState;
	}
	clear { proxy = nil }
}

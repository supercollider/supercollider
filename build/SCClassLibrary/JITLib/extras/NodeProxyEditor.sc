NodeProxyEditor { 		classvar <>menuHeight=18;	var 	<proxy;
	var <skin, <font;

	var <>w, <zone, <nameView, <typeChanView, <monitor,
		<>nSliders, <edits, <sinks, <scrolly, skipjack;
	var buttonFuncs, pauseBut, sendBut;
	
	var 	<currentSettings, 	<prevSettings, <editKeys, <>ignoreKeys=#[];	var <>replaceKeys;		// a dict for slider names to be replaced
	var 	<tooManyKeys = false, <keysRotation = 0; 	

	*initClass { 
		StartUp.add({ Spec.add(\amp4, [0, 4, \amp]) }); 
	}
		*new { arg proxy, nSliders=16, win, comp, 		extras=[\CLR, \reset, \scope, \doc, \end, \fade], 
		monitor=true, sinks=true, morph=false; 
				^super.new.nSliders_(nSliders)
			.init(win, comp, extras, monitor, sinks, morph)			.proxy_(proxy);	}	proxy_ { arg px; 
		var typeStr;
		if (px.isNil) { 
			proxy = nil; 
			nameView.object_(px).string_('-');
			typeChanView.string_('-'); 
			this.fullUpdate;
		} { 			if (px.isKindOf(NodeProxy)) { 				proxy = px;				nameView.object_(px).string_(px.key ? 'anon proxy');				typeStr = if(proxy.rate === 'audio') { "ar" } { 
						if(proxy.rate === 'control', { "kr" }, { "ir" })
						} +  proxy.numChannels;
				typeChanView.string_(typeStr);
				if (monitor.notNil) { monitor.proxy_(proxy) };				this.fullUpdate;			} 
		}	}
	
	name { ^nameView.string.asSymbol }
	name_ { |key| nameView.string_(key.asString); }
	
		// backwards compatibility
	pxKey { ^this.name }
	pxKey_ { |key| ^this.name_(key) }
	clear {		proxy = nil; 		{ nameView.object_(nil).string_("-"); }.defer;		this.fullUpdate;	}
			init { arg win, comp, extras, monitor, sinks, morph;
						var bounds;						skin = GUI.skin; 				font = GUI.font.new(*GUI.skin.fontSpecs);				bounds = Rect(0, 0, 340, nSliders + 2 * skin.buttonHeight + 16);				win = win ?? { 
			// "NodeProxyEditor: making internal win.".postln; 
			Window(this.class.name, bounds.resizeBy(4, 4)) 
		};		w = win; 			zone = comp ?? { 
			// "NodeProxyEditor: making internal zone.".postln;  
			CompositeView(w, bounds); 
		};		zone.decorator = zone.decorator ??  { FlowLayout(zone.bounds).gap_(0@0) };		w.view.background = skin.background;		zone.background = skin.foreground;		w.front;		
		replaceKeys = replaceKeys ?? { () };
		
		this.makeButtonFuncs; 
				this.makeTopLine(extras);		if (monitor, { this.makePxMon; zone.decorator.nextLine.shift(0, 4); });		if (morph, { this.makeMorph; zone.decorator.nextLine.shift(0, 4); });		this.makeSinksSliders(sinks);
				this.makeSkipJack; 	}
		makePxMon { 		monitor = ProxyMonitorGui(proxy, zone, 
			zone.bounds.width - 4 @ (skin.buttonHeight), 
			showName: false, makeWatcher: false);	}	makeMorph { 
		inform("NodeProxyEditor: preset/morph not finished yet.");//		zone.decorator.shift(0, 2);//		StaticText(zone, Rect(0,0, 330, 1)).background_(Color.gray(0.2));//		zone.decorator.shift(0, 2);//		PxPreset(proxy, zone);	}		makeTopLine { |extras|
		nameView = DragBoth(zone, 90@menuHeight)			.font_(font).align_(\center)			.background_(Color.white);
		nameView.setBoth_(false)
			.receiveDragHandler = { this.proxy_(View.implClass.currentDrag) };
			
		typeChanView = StaticText(zone, 30@menuHeight).string_("ar 88").align_(0).font_(font);
		
		extras.do { |butkey| 
			buttonFuncs[butkey].value;	
		};
		
		zone.decorator.nextLine.shift(0, 4);	}
		makeSinksSliders { |bigSinks|		var sinkWidth, lay; 
				sinkWidth = if (bigSinks, 40, 0); 	// invisibly small		#edits, sinks = Array.fill(nSliders, { arg i;			var ez, sink;			zone.decorator.nextLine; 			sink = DragBoth(zone, Rect(0,0, sinkWidth, skin.buttonHeight))				.string_("-").align_(\center).visible_(false);							sink.action_({ arg sink; var px;				if (sink.object.notNil) { 					px = currentEnvironment[sink.object.asSymbol];					if(px.isKindOf(NodeProxy)) {						proxy.map(editKeys[i], px);						this.fullUpdate;					}				}			}); 			ez = EZSlider(zone, (330 - sinkWidth)@(skin.buttonHeight), "", nil.asSpec, 				labelWidth: 60, numberWidth: 42, unitWidth: 20);
			ez.visible_(false);			ez.labelView.font_(font).align_(\center);
			
			ez.sliderView.keyDownAction = { |view, char, mod, unicode| 
				if (unicode == 127) { // delete key
					try { proxy.unset(ez.labelView.string.asSymbol) }
				}
			};
			[ez, sink]		}).flop;

		lay = zone.decorator; 
		lay.left_(lay.bounds.right - 20).top_(lay.bounds.top + 48);
		
		scrolly = EZScroller(zone, 
			Rect(0, 0, 14, nSliders * skin.buttonHeight), 
			nSliders, nSliders, 
			{ |sc| keysRotation = sc.value.asInteger; }
		).visible_(false);
		[\scrolly, scrolly.slider.bounds];
	}

	makeButtonFuncs { 
		buttonFuncs = (
			CLR: { Button(zone, 30@20).font_(font)
					.states_([[\CLR, skin.fontColor, Color.clear]])
					.action_({ arg btn, mod; 
						if ([524576, 24].includes(mod) ) { 
							proxy.clear 
						} { 
							"use alt-click to clear proxy.".postln;
						} 
					}) 
			},
				
			reset: { Button(zone, 30@20).font_(font)
					.states_([[\reset, skin.fontColor, Color.clear]])
					.action_({ proxy !? { proxy.nodeMap = ProxyNodeMap.new; this.fullUpdate; } }) 
			},

			pausR: { pauseBut = Button(zone, 30@20).font_(font)
					.states_([
							["paus", skin.fontColor, skin.onColor], 
							["rsum", skin.fontColor, skin.offColor]
						])
					.action_({ arg btn; proxy !? {
								[ { proxy.resume; }, { proxy.pause; }  ].at(btn.value).value;
							} });
				},

			sendR: 	{ sendBut = Button(zone, 30@20).font_(font)
						.states_([ 
							["send", skin.fontColor, skin.offColor], 
							["send", skin.fontColor, skin.onColor] 
						])
						.action_({ arg btn, mod; 
									//	mod.postln;
							if(proxy.notNil and: (btn.value == 0)) { 
								// alt-click osx, swingosc
								if ([524576, 24].includes(mod) ) { proxy.rebuild } { proxy.send }
							};
							btn.value_(1 - btn.value)
						})
			},

			scope: 	{ Button(zone, 36@20).font_(font)
						.states_([[\scope, skin.fontColor, Color.clear]])
						.action_({ proxy !? { proxy.scope } }) 
			},
			
			doc: 	{ Button(zone, 30@20).font_(font)
						.states_([[\doc, skin.fontColor, Color.clear]])
						.action_({ 
							if(proxy.notNil and: currentEnvironment.isKindOf(ProxySpace)) { 
								currentEnvironment.document(proxy.key)
									.title_("<" + proxy.key.asString + ">") 
							} {
								"can't currently document a proxy outside a proxy space.".inform;
							}
						}) 
			}, 

			end: 	{ Button(zone, 24@20).font_(font)
					.states_([[\end, skin.fontColor, Color.clear]])
					.action_({ proxy !? {  proxy.end } }) 
			},
			
			fade: 	{ var nb = EZNumber(zone, 60@20, \fade, [0.001, 100, \exp], 
								{ |num| proxy.fadeTime_(num.value) }, 
								try { proxy.fadeTime } ? 0.02, 
								labelWidth: 24, 
								numberWidth: 32);
						nb.labelView.font_(font).background_(Color.clear);
						nb.numberView.font_(font).background_(Color.clear);
			},
				
				// extras:

			rip: 	{ Button(zone, 15@20).font_(font)
						.states_([['^', skin.fontColor, Color.clear]])
						.action_({ this.class.new(proxy, nSliders) }) 
			},


			wake: 	{ Button(zone, 30@20).font_(font)
						.states_([[\wake, skin.fontColor, Color.clear]])
						.action_({  proxy !? { proxy.wakeUp } }) 
			},
				
			send: 	{ Button(zone, 30@20).font_(font)
						.states_([[\send, skin.fontColor, Color.clear]])
						.action_({  proxy !? { proxy.send } }) 
			},
				
			rebuild: 	{ Button(zone, 30@20).font_(font)
						.states_([[\rbld, skin.fontColor, Color.clear]])
						.action_({  proxy !? { proxy.rebuild } }) 
			}
				
//			poll: 	{ Button(zone, 30@20).font_(font)
//				.states_([[\poll, skin.fontColor, Color.clear]])
//				.action_({  proxy !? { proxy.poll } }) },
//
//						// show a little amp view?
//			amp: 	{ Button(zone, 30@20).font_(font)
//				.states_([[\amp, skin.fontColor, Color.clear]])
//				.action_({ "// show a little amp view?".postln }) }
				
		);
	}
		makeSkipJack {				skipjack = SkipJack(			{ this.checkUpdate }, 			0.2, 			{ w.isClosed }, 			this.class.name		); 		// w.onClose_({ skipjack.stop; });		this.runUpdate;	}	getCurrentKeysValues {  		if (proxy.isNil, {^[] });		currentSettings = proxy.getKeysValues(except: ignoreKeys);		editKeys = currentSettings.collect({ |list| list.first.asSymbol }); 	}	
	checkTooMany { 
		var oversize = (editKeys.size - nSliders).max(0);
		tooManyKeys = oversize > 0;
		keysRotation = keysRotation.clip(0, oversize);
		
		if (tooManyKeys) { 
		//	"tooManyKeys...".postln;

			scrolly.visible_(true);
			scrolly.numItems_(editKeys.size);
			editKeys = editKeys.drop(keysRotation).keep(nSliders);
			currentSettings = currentSettings.drop(keysRotation).keep(nSliders);
		} {	
		//	"plenty of space.".postln;
			scrolly.numItems_(editKeys.size);
			scrolly.visible_(false);
		};
		scrolly.value_( keysRotation);
	}
		checkUpdate { 		var oldKeys; 		oldKeys = editKeys; 
				this.getCurrentKeysValues; 		
		this.checkTooMany;
		
		// [\oldKeys, oldKeys, \editKeys, editKeys].printAll;
		
		if (monitor.notNil) { monitor.updateAll }; 

			if (pauseBut.notNil) { 
				pauseBut.value_((proxy.notNil and: { proxy.paused }).binaryValue) 
			};
			if (sendBut.notNil) { 
				sendBut.value_((proxy.notNil and: { proxy.objects.notEmpty }).binaryValue) 
			};
				if ( (editKeys != oldKeys), { this.updateAllEdits }, {  this.updateVals });	}		fullUpdate { 		this.getCurrentKeysValues; 
		this.checkTooMany;		this.updateAllEdits;	}		updateVals {		var sl, val, mapKey;
		
	//	"updateVals : ".postln;
		if (currentSettings == prevSettings) {
		 //	"no change.".postln;
			^this;
		};
		
	//	"values have changed - updating edits.".postln; 
		
		editKeys.do { arg key, i;			sl = edits[i];			// editKeys and currentSettings are in sync.			val = currentSettings[i][1].unbubble; 
			if (val != try { prevSettings[i][1] }) { 
					// disable for arrayed controls
				sl.enabled_(val.size <= 1);		 				// when in doubt, use this:				//	val = (currentSettings.detect { |set| set[0] == key } ? [])[1];				if(sl.numberView.hasFocus.not) {					if (val.isKindOf(SimpleNumber), { 												sl.value_(val.value);						sl.labelView.string_(this.replaceName(key));						sinks[i].string_("-");					}, { 						if (val.isKindOf(BusPlug), { 							mapKey = currentEnvironment.findKeyForValue(val) ? "";							sinks[i].object_(mapKey).string_(mapKey);							sl.labelView.string = "->" + key;						});					});				};
			};		};
		prevSettings = currentSettings; 	}	replaceName { |key| 
		var replaced;
		if (replaceKeys.isNil) { ^key };
		replaced = replaceKeys[key.asSymbol];
		if (replaced.isNil) { ^key };
	//	"NPXE: replacing % with %.".format(key.asCompileString, replaced.asCompileString).postln;
		^replaced;
		 
	}	updateAllEdits {
	//	var keyPressed = false;
		if (proxy.isNil) { 
			[sinks, edits].do(_.do(_.visible_(false)));
			^this;
		};
		
		edits.do { arg sl, i;
			var key, val, mappx, spec, slider, number, sink, mapKey;
			var keyString, labelKey, isWet, isMix; 
			
			key = editKeys[i];
			sink = sinks[i];
			
			if(key.notNil) { 
				// editKeys and currentSettings are in sync.
				val = currentSettings[i][1];	
				
				labelKey = key;
				keyString = key.asString;
				spec = key.asSpec;

				isWet = keyString.beginsWith("wet"); // a filtered slot
				isMix = keyString.beginsWith("mix");	// an additive source
					
				if (isWet or: isMix) { 
					if (isWet) { spec = \amp.asSpec };
					if (isMix) { spec = \amp4.asSpec };
					sl.sliderView.background_(Color.green(1.0, 0.5)).refresh;
					sl.labelView.background_(Color.green(1.0, 0.5)).refresh;
				} { 
					sl.sliderView.background_(skin.foreground).refresh;
					sl.labelView.background_(skin.foreground).refresh;
				};
				sl.visible_(true);
				
				sl.labelView.string = this.replaceName(key);
				sl.sliderView.enabled = spec.notNil;
				sl.sliderView.visible = spec.notNil;
				sl.numberView.enabled = true;
				sl.numberView.visible = true;
				sl.labelView.visible = true;
				sink.visible = true;						
	//			sl.sliderView.keyDownAction = { keyPressed = true }; // doesn't work yet.
	//			sl.sliderView.keyUpAction = {  proxy.xset(key, sl.value); keyPressed = false };

				sl.action_({ arg nu; proxy.set(key, nu.value) });
				
				if(spec.notNil) { sl.controlSpec = spec } {
					sl.controlSpec = ControlSpec(-1e8, 1e8);
				};
				
				mappx = val.value ? 0; 
				if(mappx.isNumber) {
					sl.value_(mappx ? 0);
					sink.object_(nil).string_("-");
				} {	
						// assume mappx is a proxy:
					if (val.isKindOf(BusPlug), { 
						mapKey = currentEnvironment.findKeyForValue(mappx) ? "???";
						sink.object_(mapKey).string_(mapKey);
						sl.labelView.string = "->" + key;
					});				
				}
			}  {		
				// "// hide unused edits".postln;
				sink.object_(nil).string_("-").visible_(false);
				sl.labelView.background_(skin.foreground).string_("");
				sl.sliderView.background_(skin.foreground);
				sl.visible_(false);
			};
		};
		// this.adjustWindowSize;
	}		runUpdate {  skipjack.start }	stopUpdate { skipjack.stop }}RecordProxyMixer {	var <>proxymixer, bounds;	var <recorder, <recType=\mix;	var <>recHeaderFormat, <>recSampleFormat, <preparedForRecording=false;	var skipjack, <display;		*new { arg proxymixer, bounds; 		^super.newCopyArgs(proxymixer, bounds).initDefaults.makeWindow	}	initDefaults {		recHeaderFormat = recHeaderFormat ?? { this.server.recHeaderFormat };		recSampleFormat = recSampleFormat ?? { this.server.recSampleFormat };	}		prepareForRecord {		var numChannels=0, path, proxies, func;		proxies = this.selectedKeys.collect { arg key; this.at(key) };		if(proxies.isEmpty) { "NodeProxyEditor: no proxies.".postln; ^nil };						if(recType == \multichannel)  {			proxies.do {  |el| numChannels = numChannels + el.numChannels };			func = { proxies.collect(_.ar).flat }; // no vol for now!		} {			proxies.do {  |el| numChannels = max(numChannels, el.numChannels) };			func = { var sum=0;				proxies.do { |el| sum = sum +.s el.ar };				sum 			}		};				path = thisProcess.platform.recordingsDir +/+ "SC_PX_" ++ numChannels ++ "_" ++ Date.localtime.stamp ++ "." ++ recHeaderFormat;		recorder = RecNodeProxy.audio(this.server, numChannels);		recorder.source = func;		recorder.open(path, recHeaderFormat, recSampleFormat);		preparedForRecording = true;		^numChannels	}		font { ^GUI.font.new(*GUI.skin.fontSpecs) }	skin { ^GUI.skin }		server { ^proxymixer.proxyspace.server }		removeRecorder {		recorder !? {			recorder.close;			recorder.clear;			recorder = nil;		};		preparedForRecording = false;	}		record { arg paused=false;		if(recorder.notNil) {recorder.record(paused) };	}		pauseRecorder { recorder !? {recorder.pause} }	unpauseRecorder { 		recorder !? { if(recorder.isRecording) { recorder.unpause } { "not recording".postln } } 	}		selectedKeys {  ^proxymixer.selectedKeys }	at { arg key; ^proxymixer.proxyspace.envir.at(key) }	selectedKeysValues {		^this.selectedKeys.collect { |key| 					var proxy = this.at(key);					[ key, proxy.numChannels ]		}.flat;	}		makeWindow {		var rw, recbut, pbut, numChannels, recTypeChoice;		var font = this.font;				rw = Window("recording:" + proxymixer.title, bounds);		rw.view.decorator = FlowLayout(rw.view.bounds.insetBy(20, 20));		rw.onClose = { this.removeRecorder; this.stopUpdate; };		rw.view.background = this.skin.background;						recbut = Button(rw, Rect(0, 0, 80, 20)).states_([			["prepare rec", this.skin.fontcolor, Color.clear],			["record >", Color.red, Color.gray(0.1)],			["stop []", this.skin.fontcolor, Color.red]		]).action_({|b|			var list;			switch(b.value,				1, { 					numChannels = this.prepareForRecord;					if(numChannels.isNil) { b.value = 0; pbut.value = 0 } 				},				2, { this.record(pbut.value == 1) },				0, { this.removeRecorder  }			);			if(b.value == 1 and: { numChannels.notNil }) {				list = this.selectedKeysValues;				this.displayString = format("recording % channels: %", numChannels, list.join(" "));			};					}).font_(font);				pbut = Button(rw, Rect(0, 0, 80, 20)).states_([			["pause", this.skin.fontcolor, Color.clear],			[">", Color.red, Color.gray(0.1)]		]).action_({|b|			if(b.value == 1) {  this.pauseRecorder } { this.unpauseRecorder }				}).font_(font);				recTypeChoice = PopUpMenu(rw, Rect(0, 0, 110, 20))				.items_([ \mix, \multichannel ])				.action_({ arg view; 					recType = view.items[view.value]; 					if(recbut.value != 0) { recbut.valueAction = 0 } 				})				.font_(font);		recTypeChoice.value = 1;		Button(rw, Rect(0, 0, 60, 20))				.states_([["cancel", this.skin.fontcolor, Color.clear]])				.action_({ if(recbut.value != 0) { recbut.valueAction = 0 } })				.font_(font);			rw.view.decorator.nextLine;		display = StaticText(rw, Rect(30, 40, 300, 20)).font_(font);		rw.front;		this.makeSkipJack;		this.runUpdate;				^rw	}	displayString_ { arg str;		display.string = str;	}		updateZones {		if(preparedForRecording.not) {			this.displayString = format("proxies: %", this.selectedKeysValues.join(" "));		}	}	title {  ^"recorder for" + proxymixer.title }	runUpdate { skipjack.start; }	stopUpdate { skipjack.stop }	makeSkipJack {				// stoptest should check window.		skipjack = SkipJack({ this.updateZones }, 0.5, name: this.title);	}}
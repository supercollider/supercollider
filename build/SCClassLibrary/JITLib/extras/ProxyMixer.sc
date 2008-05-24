// idea by adc, mods by /f0, jrh, mc, ...ProxyMixer {		var <w, <proxyspace, <>nProxies;	var <activeNodes;	var <title;		var <editZoneBtn, <pxMons, <editBtnsAr, <editBtnsKr, <buttonLinesKr;	var <compArZone, <compKrZone, <compEditZone;		var origBounds, fullBounds, editZoneOpen=false, <selectMethod=\existingProxies;	var <skippy, <editor, <recorder;	var <skin, <font; 
	var <prevKrNames, <prevArNames;	
	var <scrollyAr, <keysRotationAr=0; 
	var <scrollyKr, <keysRotationKr=0; 
		*new { arg proxyspace, nProxies = 16, title, bounds;		^super.new.init(proxyspace, nProxies, 			title ?? { format("proxyspace: %", proxyspace.tryPerform(\name) ? "") }, bounds		);	}		*initClass {				// currently stereo only..		SynthDef("proxymixer_emergency", { arg out, gate=1.0, minDb = -160;			var amp;			amp = SinOsc.kr(Rand(1, 2)).range(-20, Line.kr(-20, -160)).dbamp;			amp = amp * EnvGen.kr(					Env.new(#[0,1,0],#[0.01,1],'sine',1),					gate, doneAction:2				);			ReplaceOut.ar(out,				In.ar(out * 0.0, 2)			);					}).writeDefFile;	}		init { arg space, nPxs = 16, argTitle, argBounds; 				var winHeight, globalLayout, height, color;		skin = GUI.skin; 
		font = GUI.font.new(*skin.fontSpecs);
				title = argTitle;		argBounds = argBounds ?? { Rect(10, 250, 0, 0) };						proxyspace = space ? currentEnvironment;		if(proxyspace.isKindOf(ProxySpace).not) { Error("no proxy space present!").throw };		nProxies = nPxs;				height = nProxies + 1 * (skin.buttonHeight) + 16;				// global layout: 			//	origBounds = (Rect(10, 250, 472, height + 10));		origBounds = argBounds.resizeTo(562, height + 10);		this.makeWindow;		fullBounds = w.bounds.resizeBy(375, 0);						globalLayout = GUI.compositeView.new(w, fullBounds.moveTo(0,0));		globalLayout.decorator =  FlowLayout(globalLayout.bounds); //.gap_(6@6);				compArZone = GUI.compositeView.new(globalLayout, Rect(0, 0, 350, height))
			.background_(skin.foreground);		compKrZone = GUI.compositeView.new(globalLayout, Rect(0, 0, 200, height))
			.background_(skin.foreground);				compEditZone = GUI.compositeView.new(globalLayout, Rect(0, 0, 370,height))
			.background_(skin.foreground);			compArZone.decorator = FlowLayout(compArZone.bounds).gap_(skin.gap);		compKrZone.decorator = FlowLayout(compKrZone.bounds).gap_(skin.gap);		compEditZone.decorator = FlowLayout(compEditZone.bounds).gap_(skin.gap);				this.makeArZone; 		this.makeKrZone; 		this.makeEditZone; 				w.front; 		this.makeSkipJack;		w.onClose_({ this.stopUpdate }); 		this.runUpdate;			}			runUpdate { editor.runUpdate; skippy.start; }	stopUpdate { editor.stopUpdate; skippy.stop }			makeWindow {		w = GUI.window.new(this.title, origBounds);		w.view.background = skin.background;			}	makeSkipJack { 		skippy = SkipJack({ this.updateZones }, 0.2, { w.isClosed }, title);	}		makeEditZone {		editor = NodeProxyEditor(nil, nil, nProxies - 1, w, compEditZone);	}	openEditZone { arg flag;			if(flag == 1 or: {editZoneOpen.not}) {				if(editZoneOpen.not) { 					origBounds = w.bounds;					w.bounds = w.bounds.setExtent(fullBounds.width, fullBounds.height);					editZoneOpen = true;					editZoneBtn.value = 1;					this.update;				}			}{				w.bounds = w.bounds.setExtent(origBounds.width, origBounds.height);				editZoneOpen = false; 				editZoneBtn.value = 0; 			};	}	makeArZoneHead { 			GUI.popUpMenu.new(compArZone,Rect(10, 10, 110, skin.buttonHeight+2))				.items_([\existingProxies, \activeProxies, \playingProxies])				.action_({ arg view; selectMethod = view.items[view.value] })				.font_(font);						GUI.button.new(compArZone, Rect(10, 10, 50, skin.buttonHeight+2))				.states_(					[["reduce", skin.fontcolor, Color.clear]]				)				.action_({ proxyspace.reduce }).font_(font);		GUI.button.new(compArZone, Rect(10, 10, 30, skin.buttonHeight+2))				.states_(					[["doc", skin.fontcolor, Color.clear]]				)				.action_({ proxyspace.document }).font_(font);		GUI.button.new(compArZone, Rect(10, 10, 30, skin.buttonHeight+2))				.states_(					[["docc", skin.fontcolor, Color.clear]]				)				.action_({ 					proxyspace.document(this.selectedKeys)				}).font_(font);		/*		GUI.button.new.new(compArZone, Rect(10, 10, 20, skin.buttonHeight+2)).states_([				["#",  skin.fontcolor, Color.clear ]			])			.action_({ editor.pxKey !? { proxyspace.storeOn(Post, [editor.pxKey]) } })			.font_(font);		*/		editZoneBtn = GUI.button.new(compArZone, Rect(10, 10, 60, skin.buttonHeight+2))				.font_(font)				.states_(					[	["openEdit", skin.fontcolor, Color.clear], 						["closeEdit", skin.fontcolor, Color.clear]					]				)				.action_({ |b| this.openEditZone(b.value) });				GUI.button.new(compArZone, Rect(10, 10, 20, skin.buttonHeight+2))				.font_(font)				.states_(					[	["R", Color.red, Color.clear]					])				.action_({  RecordProxyMixer(this, origBounds.resizeTo(472, 100)) });			}		makeArZone { 		var arLayout = compArZone.decorator;		this.makeArZoneHead;				arLayout.nextLine;		arLayout.shift(0,4);				#pxMons, editBtnsAr = Array.fill(nProxies, { arg i; 			var pxmon, edbut;			pxmon = ProxyMonitorGui(nil, compArZone, skin.buttonHeight, true, false);			pxmon.zone.visible_(false);		//	arLayout.shift(4); 						edbut = GUI.button.new(compArZone, Rect(0,0,16,16))				.font_(font)				.visible_(false)				.states_([					["ed", Color.black, Color.grey(0.75)],					["ed", Color.black, Color.white]])				.action_({ arg btn;						editor.proxy_(pxmon.proxy).pxKey_(pxmon.proxy.key);					this.openEditZone(1);					editBtnsAr.do { |b| b.value_(0) };					editBtnsKr.do { |b| b.value_(0) };					btn.value_(1);				});			arLayout.nextLine;			[pxmon, edbut]		}).flop;	
		
		scrollyAr = EZScroller.new(w, 
			Rect(compArZone.decorator.bounds.right - 16, 30, 12, nProxies * skin.buttonHeight), 
			nProxies, nProxies, 
			{ |sc| keysRotationAr = sc.value.asInteger.max(0); }
		).value_(0).visible_(true);

	}		makeKrZone { 		var layout = compKrZone.decorator;
		var emergencySynth;

		GUI.staticText.new(compKrZone, Rect(0, 0, 180, 18))
			.font_(font).align_(\center)
			.string_("kr proxies");
			
		layout.nextLine;
		layout.shift(0,4);
		
		/*		GUI.button.new(compKrZone, Rect(0,0,20, skin.buttonHeight))		.font_(Font("Helvetica-Bold", 12))		.states_([["+", Color.red, Color.white]])		.action_({			if(emergencySynth.isPlaying.not) {				emergencySynth = Synth.tail(0, "proxymixer_emergency");				NodeWatcher.register(emergencySynth);			} {				emergencySynth.release			}		});		*/		layout.nextLine;		layout.nextLine;				buttonLinesKr = Array.fill(nProxies, { arg i;			var nameBtn, pausBtn, sendBtn, editBtn, pollBtn;						nameBtn = GUI.dragSource.new(compKrZone, Rect(0,0,68, skin.buttonHeight))			.font_(font)			.align_(\center);			
			pausBtn = GUI.button.new(compKrZone, Rect(0,0, 30, skin.buttonHeight))
				.font_(font).states_([["paus", Color.black, skin.foreground], 
				["rsum", Color.black]])
				.action_({ |btn|
					var editName = nameBtn.string.asSymbol;
					var px = proxyspace.envir[editName];
					if (px.notNil) { 
						if (px.paused) { px.resume } { px.pause };
					};
				});
			sendBtn = GUI.button.new(compKrZone, Rect(0,0, 30, skin.buttonHeight))
				.font_(font).states_([["send", Color.black, skin.foreground]])
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
					editor.pxKey_(editName).proxy_(proxyspace.envir[editName]);
					this.update;
					this.openEditZone(1);
					editBtnsAr.do { |b| b.value_(0) };
					editBtnsKr.do { |b| b.value_(0) };
					btn.value_(1);
				}).font_(font);

						[ nameBtn, pausBtn, sendBtn, editBtn, pollBtn].do({ arg v; v.visible_(false) });		});
		editBtnsKr = buttonLinesKr.collect(_[1]);		layout.nextLine;		

		scrollyKr = EZScroller.new(w, 
			Rect(compKrZone.decorator.bounds.right - 16, 30, 12, nProxies * skin.buttonHeight), 
			nProxies, nProxies, 
			{ |sc| keysRotationKr = sc.value.asInteger.max(0); }
		).value_(0).visible_(true);

	}	updateZones {		this.updateArSlots;			this.updateKrSlots; 	}		updateKrSlots { 		var krProxyNames;		krProxyNames = proxyspace.krProxyNames;

		if (krProxyNames.size > nProxies) { 

			scrollyKr.visible_(true)
				.numItems_(krProxyNames.size)
				.value_(keysRotationKr ? (krProxyNames.size - nProxies)); 
				
			krProxyNames = krProxyNames.drop(keysRotationKr).keep(nProxies);
		} {
			scrollyKr.visible_(false);
		}; 

		if (krProxyNames != prevKrNames) { 			nProxies.do { arg i;				var butLine, nameSink, key, px, showLine;				key = krProxyNames[i];				px = proxyspace.envir[ key ];
				
				butLine = buttonLinesKr[i];
				nameSink = butLine[0];				nameSink.string_(key ? "").object_(key);
								showLine = key.notNil and: px.notNil;					butLine.do(_.visible_(showLine));			}
		};
		prevKrNames = krProxyNames;	}	existingProxies { ^proxyspace.arProxyNames }	activeProxies { ^proxyspace.arProxyNames({ |px, key| px.isPlaying }) }	playingProxies { ^proxyspace.arProxyNames({ |px, key| px.monitor.isPlaying }) }	selectedKeys { ^this.perform(selectMethod) }		updateArSlots {		var arPxNames, pxKey, px, pxMon;		arPxNames = this.selectedKeys;				if (arPxNames.size > nProxies) { 
			scrollyAr.visible_(true)
				.numItems_(arPxNames.size)
				.value_(keysRotationAr ? (arPxNames.size - nProxies)); 
							arPxNames = arPxNames.drop(keysRotationAr).keep(nProxies);		} {
			scrollyAr.visible_(false);
		}; 
				if (arPxNames != prevArNames) {	 			nProxies.do { arg i;				pxMon = pxMons[i]; 				pxKey = arPxNames[i]; 								px = proxyspace.envir.at(pxKey);				if(px.notNil) { 					pxMon.zone.visible_(true);					editBtnsAr[i].visible_(true);					pxMon.proxy_(px);				}{					pxMon.zone.visible_(false);					editBtnsAr[i].visible_(false);					}			} 
		}; 
		arPxNames.do { |name, i| pxMons[i].updateAll };
		
		prevArNames = arPxNames;	}			adjustWindowSize { arg n;		n = n ? nProxies;		w.bounds = if(editZoneOpen) 			{ origBounds.resizeBy(0, 330) } 			{ origBounds.copy.height_(n  * 18 + 80) };	}}

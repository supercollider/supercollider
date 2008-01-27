ProxyMonitorGui { 		var <>proxy, <win, <zone, <flow, <skipjack, <usesPlayN; 	var <ampSl, <ampSpec, <playBut, <nameBut, <setOutBox, <playNDialogBut, <pauseBut, <sendBut; 	var <monHasSeriesOut = true; 	var <oldState = #[]; 
		*new { |px, w, height=16, usePlayN=true, makeWatcher=true| 		^super.new.proxy_(px).init(w, height, usePlayN, makeWatcher); 	}	init { |w, height, usePlayN, makeWatcher| 
		var skin = GUI.skin;		var font = GUI.font.new(*GUI.skin.fontSpecs);
				usesPlayN = usePlayN; 		ampSpec = ControlSpec(0, 1, 'amp', 0, 0.25);				win = w ?? { GUI.window.new("pxmon", Rect(0, 0, 330, 50)) };		zone = GUI.compositeView.new(win, Rect(0, 0, 310, height)); 		zone.background_(skin.foreground);		flow = FlowLayout(zone.bounds, 0@0, skin.gap);		zone.decorator = flow; 				ampSl = GUI.slider.new(zone, Rect(0,0,110, height))			.action_({ arg slid; 
				if(proxy.notNil) { proxy.vol_(ampSpec.map(slid.value)); } 
			});				nameBut = GUI.dragSource.new(zone, Rect(0,0,60,height))			.font_(font).align_(0)
			;					nameBut.object_(try { proxy.key });					playBut = GUI.button.new(zone, Rect(0,0,34,height))			.font_(font)			.states_([				["play", skin.fontColor, skin.offColor], 				["stop", skin.fontColor, skin.onColor ]			])			.action_({ arg btn, modif; 			// 524576 is alt, 262401 is ctl, 8388864 is fn
			// swingosc: 16 is normal  24 is alt, ctl is off, and fn is 16 as well			if(proxy.notNil) {				[ 	{ 	if ([524576, 24].includes(modif)) { proxy.end } { proxy.stop } }, 					{ 	proxy.playN; } 				].at(btn.value).value			}		});				setOutBox = GUI.numberBox.new(zone, Rect(0,0,16,height))			.font_(font).align_(0)			.action_({ |box, mod| 				proxy.monitor.out_(box.value.max(0).asInteger);			}); 		if (usesPlayN) { 			playNDialogBut = GUI.button.new(zone, Rect(0,0,20,height))
				.font_(font)				.states_([					["-=", skin.fontColor, skin.offColor],
					["-<", skin.fontColor, skin.onColor]				])
				.action_({ |box, mod|					if (proxy.notNil) { proxy.playNDialog };
					box.value_(1 - box.value)				});		}; 				pauseBut = GUI.button.new(zone, Rect(0,0,34,height))
			.font_(font)			.states_([				["paus", skin.fontColor, skin.onColor], 				["rsum", skin.fontColor, skin.offColor]			])
			.action_({ arg btn; 			if(proxy.notNil, {					[ 	{ proxy.resume; }, { proxy.pause; }  ].at(btn.value).value;				})			});				sendBut = GUI.button.new(zone, Rect(0,0,34,height))
			.font_(font)			.states_([ 				["send", skin.fontColor, skin.offColor], 				["send", skin.fontColor, skin.onColor] 			])			.action_({ arg btn, mod; 
				mod.postln;				if(proxy.notNil and: (btn.value == 0)) { 
					// alt-click osx, swingosc					if ([524576, 24].includes(mod) ) { proxy.rebuild } { proxy.send }				};
				btn.value_(1 - btn.value)			})
			;			if (win.isKindOf(GUI.window)) { win.front };		if (makeWatcher) { this.makeWatcher };	}	makeWatcher { 		skipjack.stop;		skipjack = SkipJack({ this.updateAll }, 			0.5, 			{ win.isClosed },			"PxMon" + try { proxy.key }		);		skipjack.start;	}		updateAll { 		var monitor, outs, amps, newHasSeriesOut;
		 		var currState;
		var currVol=0, pxname="", isAudio=false, plays=0, playsSpread=false, pauses=0, canSend=0; 
				if (win.isClosed) {skipjack.stop; ^this };
				if (proxy.notNil) { 
					pxname = proxy.key;	
			canSend = proxy.objects.notEmpty.binaryValue;			pauses = proxy.paused.binaryValue;
			
			isAudio = proxy.rate == \audio;			monitor = proxy.monitor; 
			plays = monitor.isPlaying.binaryValue;
			
			if (monitor.notNil, { 				currVol = ampSpec.unmap(proxy.monitor.vol);
				playsSpread = proxy.monitor.hasSeriesOuts.not;				outs = monitor.outs; 			}); 
		};
		
		currState = [currVol, pxname, isAudio, plays, outs, playsSpread, pauses, canSend];
		
		if (currState != oldState) { 
		//	"updating".postln; 
		
			ampSl.value_(currVol);
			nameBut.object_(pxname);
			pauseBut.value_(pauses);
			playBut.value_(plays);
			sendBut.value_(canSend);

			if (isAudio != oldState[2]) { 
				ampSl.enabled_(isAudio);
				playBut.enabled_(isAudio);
				setOutBox.enabled_(isAudio);
				playNDialogBut.enabled_(isAudio);
			}; 
			
			if (setOutBox.hasFocus.not) {	 
				setOutBox.value_(try { outs[0] } ? 0);
				if (usesPlayN) { 
					playNDialogBut.value_(playsSpread.binaryValue)
				};
			}
		};
		oldState = currState;	}	clear { proxy = nil }}
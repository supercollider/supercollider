/*

s.boot;

a = Volume(s, 0, s.options.numOutputBusChannels);
a.play;

z = {SinOsc.ar(440, 0, 0.1)}.play

s.volume.volume_(-9);
s.volume.volume;
a.mute;
a.unmute;
a.volume_(0);

a.gui;

a.free;

z.free;

*/

Volume {

	var startBus, numChans, <min, <max, server, persist, <amp, <>window,Ê<volume, spec;Ê
	var <lag, sdname, gui, <isPlaying, <muteamp, <muted = false, cpFun, isMuted=false;

	
	*new {Ê arg server, startBus = 0, numChans = 2, min = -90, max = 6, persist = false;
		^super.newCopyArgs(startBus, numChans, min, max, server, persist).initVolume;
	}

	initVolume {
		var cond;
		server = server ?? {Server.default};
		volume = 0;
		isPlaying = false;
		gui = false;
//		spec = [min, max, \db].asSpec;
	}

	sendDef {
		SynthDef(sdname = (\volume_amp_control++numChans).asSymbol, { arg amp = 1, lag = 0.1;
			ReplaceOut.ar( startBus, In.ar(startBus, numChans) * Lag.kr(amp, lag) );
		}).send(server);
	}

	play {arg isMuted = false;
		var cond;
		cond = Condition.new;
		server.serverRunning.if({
			Routine.run({
				this.sendDef;
				server.sync(cond);
				isPlaying = true;
				cpFun.isNil.if({
					CmdPeriod.add(cpFun = {
						persist.if({
							this.play(muted);
							}, {
							this.free;
							});
						});
					});
				amp = Synth(sdname, [\amp, volume.dbamp, \lag, lag], 
					target: 1, addAction: \addAfter);
				isMuted.if({this.mute});
				})
			}, {
			"Volume only works on a running Server. Please boot".warn;
			})	
		}
		
	free {
		amp.free;
		isPlaying = false;
		CmdPeriod.remove(cpFun);
		cpFun = nil;
		}
	
	mute{
		this.isPlaying.if({		
			this.prmute;
			}, {
			this.playVolume(true)
			});	
	}
	
	unmute{
		isMuted = true;
		this.prunmute;
		(this.muteamp == 0.0).if({
			this.free;
			});	
	}	

	
	prmute {
		isMuted = false;
		muteamp = volume;
		amp.set([\amp, -inf.dbamp]);
		muted = true;
		this.changed(\mute, true);
		}
		
	prunmute {
	
		muted = false;
		amp.set([\amp, muteamp.dbamp]);
		this.changed(\mute, false);
		
		}
	
	// sets volume back to 1 - removes the synth
	reset {
		this.free;
		}
		
	// cleaner with MVC - in db
	volume_ { arg aVolume;
		volume = aVolume;
		(volume == 0.0).if({
			(this.isPlaying and: {this.muted.not}).if({
				this.free; 
				})
			}, {
			server.serverRunning.if({
				this.playVolume(isMuted);
				})
			});		
		muted.if({muteamp = volume});
		(isPlaying && muted.not).if({amp.set([\amp, volume.dbamp])}) ;
		this.changed(\amp, volume);
	}
	
	playVolume {arg muted = false;
		(this.isPlaying.not and: {
			(volume != 0.0) or: {muted}
			}).if({
				this.play(muted);
				})
	}
			
	lag_ {arg aLagTime;
		lag = aLagTime;
		amp.set([\lag, lag]);
		}
		
	
	gui {arg window, bounds;
			this.debug(\gui);

		^VolumeGui(this, window, bounds)	
	}
	
	close {
		window.close;
		}

}

VolumeGui{
	var <>model;
	var window, spec, slider, box, simpleController;
	
	*new{|model, win, bounds|	
		^super.new.model_(model).init(win, bounds)
	}
	
	init{|win, bounds|
		spec = [model.min, model.max, \db].asSpec;
		bounds = bounds ?? {Rect(100, 100, 80, 330)};
		window = win ?? {GUI.window.new("Volume", bounds).front};
		box = GUI.numberBox.new(window, Rect(10, 10, 60, 30))
			.value_(model.volume) ;
		slider = GUI.slider.new(window, Rect(10, 40, 60, 280))
			.value_(spec.unmap(model.volume)) ;
		slider.action_({ arg item ;Ê
			model.volume_(spec.map(item.value));
		}) ;
		box.action_({ arg item ;
			model.volume_(item.value) ;
		}) ;
		window.onClose_({
			simpleController.remove;
		});
		simpleController = SimpleController(model)
				.put(\amp, {|changer, what, volume|
					this.debug(volume);
					box.value_(volume.round(0.01)) ;
					slider.value_(spec.unmap(volume)) ;
				})
	
	}
	
	

}


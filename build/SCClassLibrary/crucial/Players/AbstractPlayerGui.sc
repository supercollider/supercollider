
AbstractPlayerGui : ObjectGui { 
	
	gui { arg lay;
		var layout;
		if(lay.isNil,{ ^this.topGui });
		layout=this.guify(lay);
		layout.flow({ arg layout;
			view = layout;
			this.writeName(layout);
			this.guiBody(layout);
		}).background_(Color.blue(0.5,0.1));
		this.enableKeyDowns;
	}
	topGui { arg layout;
		layout = this.guify(layout);
		// top level controls
		this.synthConsole(layout);
		this.saveConsole(layout);
		layout.startRow;
		layout.flow({ arg layout;
			view = layout;
			this.writeName(layout,true);
			this.guiBody(layout);
		}).background_(Color.blue(0.5,0.1));
		this.enableKeyDowns;
		layout.resizeToFit.front;
		view.focus;
	}
	
//	guify { arg layout,title,width,height;
//		layout = layout ?? {
//			FlowView(nil,nil ); //title ?? {model.asString.copyRange(0,50)},width,height);
//		};
//		layout.background = 
//		layout.removeOnClose(this);
//		^layout
//	}

	smallGui { arg layout;
		layout=this.guify(layout);
		Tile(model,layout); // writes name
	}
	writeName { arg layout,big=false; 
			//color it based on whether it has a .path 
							// (was loaded from disk)
		if(model.path.notNil,{
			ActionButton(layout,"edit",{
				model.path.openTextFile;
			});
		});
		if(big,{
			InspectorLink.big(model,layout);
		},{
			InspectorLink.new(model,layout);
		})		
	}
//	keyDowns {
//		view.keyDownAction = this.keyDownResponder;
//	}
	keyDownResponder {
		var k;
		k = KeyCodeResponder.new;
		k.registerKeycode(0,49,{
			if(model.isPlaying,{
				model.stop
			},{
				model.play
			})
		});
		^k
	}
	saveConsole { arg layout;
		^SaveConsole(model,model.path,layout).save
			.saveAs({arg path;
				model.path = path;
			})
			.printPath
			.print;
	}
	
	synthConsole {arg layout;
		var s;
		Server.local.gui(layout);
		s = //SynthConsole(model,layout).play.registerPlayKey.record.pauseableRecord.write({ model.timeDuration }).scope.stop.formats.tempo;
		SynthConsole(model,layout).play.record.stop.formats.tempo;
		//NotificationCenter.register(s,\didRecordOrWrite,model,{ NotificationCenter.notify(model,\didRecordOrWrite) });
	}
	
	durationString { // time
		var dur,div;
		dur = model.timeDuration;
		if(dur.notNil,{
			^(dur / 60.0).asInteger.asString ++ ":" ++ (dur % 60.0).round(0.01).asString
		},{
			^"inf"
		});
	}
	durationGui { arg layout;
		var durb;
		var durv;
		// make switchable between beats and secs
		
		durv= CXLabel(layout, "dur:_____");

		layout.removeOnClose(
			Updater(model,{  arg changed,changer;// any change to model at all
				durv.label_("dur:" + this.durationString);
			}).update;
		);
		
		layout.removeOnClose(
			Updater(Tempo,{ // any change to Tempo
				durv.label_("dur:" + this.durationString);	
			}).update;
		);
	}

}


KrPlayerGui : AbstractPlayerGui {}



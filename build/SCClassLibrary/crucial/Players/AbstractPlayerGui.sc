
AbstractPlayerGui : ObjectGui { 

	topGui { arg layout;
		layout = this.guify(layout);
		// top level controls
		this.synthConsole(layout);
		this.saveConsole(layout);
		layout.startRow;
		this.gui(layout);
	}
	
	gui { arg layout;
		layout=this.guify(layout);
		this.writeName(layout);
		this.guiBody(layout);
	}

	smallGui { arg layout;
		layout=this.guify(layout);
		Tile(model,layout); // writes name
	}
	writeName { arg layout; //color it based on whether it has a .path 
							// (was loaded from disk)
		if(model.path.notNil,{
			ActionButton(layout,"edit",{
				model.path.openTextFile;
			});
		});
		ActionButton(layout,model.asString,{ 	
			model.inspect;
		})
		.backColor_( Color( 0.7921568627451, 1, 0.63137254901961 ) )
		.labelColor_(Color( 0.121568627450, 0.101960784313, 0.521568627450 ))
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
		CXLabel(layout,"dur:");
		// make switchable between beats and secs
		
		durv= CXLabel(layout, "_______");

		Updater(model,{  arg changed,changer;// any change to model at all
			durv.label_(this.durationString);	
		}).update.removeOnClose(layout);
		
		Updater(Tempo,{ // any change to Tempo
			durv.label_(this.durationString);	
		}).update.removeOnClose(layout);
	}

}


KrPlayerGui : AbstractPlayerGui {}



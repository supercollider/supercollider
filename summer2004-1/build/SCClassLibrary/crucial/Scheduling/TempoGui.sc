
TempoGui : ObjectGui {

	var tempoG;
	
	writeName {}
	guiBody { arg layout;
		tempoG=NumberEditor(model.bpm,[1.0,666.0])
			.action_({arg t; model.bpm_(t)});
			
		tempoG.gui(layout,nil,true);
	}
	
	update {
		tempoG.value_(model.bpm);
	}

	*setTempoKeys {
		/*
			dependancies:
				KeyCodeResponder
			TODO make keys preferences (including off)
		*/
		KeyCodeResponder.global.normal($\\->{Tempo.tempo_(Tempo.tempo + 0.166667)}, //10bpm
					$p->{Tempo.tempo_(Tempo.tempo - 0.166667)},
					$[->{Tempo.tempo_(Tempo.tempo - 0.00416666)},//0.25 bpm
					$]->{Tempo.tempo_(Tempo.tempo + 0.00416666)}//,
					);
					
		KeyCodeResponder.global.option($[->{Tempo.tempo_(Tempo.tempo - 0.0166667)},//1 bpm
					$]->{Tempo.tempo_(Tempo.tempo + 0.0166667)},//,
					$\\->{
						Tempo.tempo_(Tempo.tempo * 2.0);
					},
					$p->{
						Tempo.tempo_(Tempo.tempo * 0.5);
					});
	}
}


/**	
TempoClockGui : ObjectGui {

	var tempoG;
	
	writeName {}
	guiBody { arg layout;
		tempoG=NumberEditor(model.tempo * 60.0,[1.0,666.0])
			.action_({arg t; model.tempo_(t / 60.0).changed(this) });
			
		tempoG.gui(layout,nil,true);
	}
	
	update { arg changed,changer;
		if(changer !== this,{
			tempoG.value_(model.tempo * 60.0);
		})
	}

	*setTempoKeys {
		/*
			dependancies:
				KeyCodeResponder
				Synth qsched modification
			TODO make keys preferences (including off)
		*/
		/*
		
		KeyCodeResponder.normal($\\->{Tempo.tempo_(Tempo.tempo + 0.166667)}, //10bpm
					$p->{Tempo.tempo_(Tempo.tempo - 0.166667)},
					$[->{Tempo.tempo_(Tempo.tempo - 0.00416666)},//0.25 bpm
					$]->{Tempo.tempo_(Tempo.tempo + 0.00416666)}//,
					);
					
		KeyCodeResponder.option($[->{Tempo.tempo_(Tempo.tempo - 0.0166667)},//1 bpm
					$]->{Tempo.tempo_(Tempo.tempo + 0.0166667)},//,
					// half or double the tempo qsched on the next beat
					$\\->{
						if(thisSynth.notNil,{
							thisSynth.qsched(2.0,{ Tempo.tempo_(Tempo.tempo * 2.0) })
						},{
							 Tempo.tempo_(Tempo.tempo * 2.0);
						})		
					},
					$p->{
						if(thisSynth.notNil,{
							thisSynth.qsched(2.0,{ Tempo.tempo_(Tempo.tempo * 0.5) })
						},{
							 Tempo.tempo_(Tempo.tempo * 0.5);
						})		
					});
		*/
	}

}

**/




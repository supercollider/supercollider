
TempoGui : ObjectGui {

	var tempoG;
	
	writeName {}
	guiBody { arg layout;
		tempoG = NumberEditor(model.bpm,[1.0,666.0])
			.action_({arg t; model.bpm_(t)});
			
		tempoG.gui(layout,nil,true);
	}
	
	update {
		tempoG.value_(model.bpm).changed;
	}

	*setTempoKeys {
		KeyCodeResponder.global.normal(
					42->{Tempo.tempo_(Tempo.tempo + 0.166667)},  //  \ + 10bpm
					35->{Tempo.tempo_(Tempo.tempo - 0.166667)},  //  p - 10bpm
					30->{Tempo.tempo_(Tempo.tempo - 0.00416666)},// [ - 0.25 bpm
					33->{Tempo.tempo_(Tempo.tempo + 0.00416666)} //  ] + 0.25 bpm
				);
					
		// double or halve the tempo
		KeyCodeResponder.global.option(
					42->{
						Tempo.tempo_(Tempo.tempo * 2.0);
					},
					35->{
						Tempo.tempo_(Tempo.tempo * 0.5);
					});
	}
}

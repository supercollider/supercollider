
TempoGui : ObjectGui {

	var tempoG,gnome;
	
	writeName {}
	guiBody { arg layout;
		var gn;
		tempoG = NumberEditor(model.bpm,[1.0,666.0])
			.action_({arg t; model.bpm_(t)});
			
		tempoG.gui(layout,nil,true);
		
		gnome = Patch({ arg beat,freq,amp;
			Decay2.ar( 
				K2A.ar(beat), 0.01,0.11, 
				SinOsc.ar( freq, 0, amp )
			)
		},[
			BeatClockPlayer.new(4.0),
			StreamKrDur(
				Pseq([ 750, 500, 300, 500, 750, 500, 400, 500, 750, 500, 400, 500, 750, 500, 400, 500 ],inf), 
				1.0),
			StreamKrDur(
				Pseq([1,0.25,0.5,0.25,0.75,0.25,0.5,0.25,0.75,0.25,0.5,0.25,0.75,0.25,0.5,0.25] * 0.01,inf),
				1.0)
		]);

		gn = SCButton(layout,17@17);
		gn.states = [ ["M",Color.black,Color.white],["M",Color.white,Color.black]];
		gn.action = {
			if(gnome.isPlaying.not,{ 
				gnome.play(atTime: 1) 
			},{
				gnome.stop							
			})
		};
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

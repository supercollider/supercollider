
TempoGui : ObjectGui {

	var tempoG,gnome;

	writeName {}
	guiBody { arg layout;
		var gn,gnomeInstr,h;
		tempoG = NumberEditor(model.bpm,[1.0,666.0])
			.action_({arg t; model.bpm_(t)});

		tempoG.gui(layout,nil,true);

		gnomeInstr = Instr("TempoGui.gnomeInstr");
		if(gnomeInstr.isNil,{
			Instr("TempoGui.gnomeInstr",
				{ arg trig,freq,amp;
					Decay2.ar(
						K2A.ar(trig), 0.01,0.11,
						SinOsc.ar( freq, 0, amp )
					)
				});
		});
		gnome = Patch("TempoGui.gnomeInstr",[
			BeatClockPlayer.new(4.0),
			StreamKrDur(
				Pseq([ 750, 500, 500, 500, 750, 500, 500, 500,
					   750, 500, 500, 500, 750, 500, 500, 500 ],inf),
				1.0),
			StreamKrDur(
				Pseq([1,0.25,0.5,0.25,0.75,0.25,0.5,0.25,
					  0.75,0.25,0.5,0.25,0.75,0.25,0.5,0.25] * 0.01,inf),
				1.0)
		]);

		h = GUI.skin.buttonHeight;
		gn = GUI.button.new(layout,h@h);
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

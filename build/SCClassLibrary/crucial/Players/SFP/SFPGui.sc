
AbstractSFPGui : AbstractPlayerGui {

	guiBody { arg layout;
		this.durationGui(layout);
		this.sfpBody(layout);
		layout.startRow;
		this.underlyingFileGui(layout);
	}
	tempoGui { arg layout;
		var n;
		CXLabel(layout,"tempo:");
		n = NumberEditor(model.tempo,[1,300])
			.action_({ arg t; model.tempo_(t).changed; });
		n.gui(layout);
		layout.removeOnClose(
			Updater(model,{
				n.value_(model.tempo);
			})
		);
	}
	underlyingFileGui { arg layout;
		model.file.gui(layout);
	}
}

SFPGui : AbstractSFPGui {

	sfpBody { arg layout;
		var pathLabel,hf,ch,sr;

		// click on the path to select a different sound file
		pathLabel=ActionButton(layout,model.fileName ? "click here to select a soundfile",
					{ this.replaceFile },minWidth:150);
		hf=CXLabel(layout,("__________________"));
		ch=CXLabel(layout,("_____________"));
		sr=CXLabel(layout,"_____________");

		//ActionButton(layout,"clip maker...",{ SFPClipMaker(model).topGui });
		//LabelledNumericalView(layout.win,layout.layRight(40,30),\tempo,model.tempo,0,2000,0)
		//		.action_({ arg th; model.tempo = th.value; model.changed; });

		layout.removeOnClose(Updater(model,{
			// in case the file gets swapped
			pathLabel.label_(model.fileName).refresh;
			hf.label_(model.file.headerFormat.asString ++ " " ++ model.file.sampleFormat.asString);
			ch.label_(model.file.numChannels.asString ++ " channels").refresh;
			sr.label_(model.file.sampleRate).refresh;
		}).update);
	}

	replaceFile {
		GetFileDialog({ arg ok,sfPath;
			if(ok,{
				model.init(sfPath);
				model.changed;
			});
		})
	}
	// it dont get more underlying than me
	underlyingFileGui { }
}

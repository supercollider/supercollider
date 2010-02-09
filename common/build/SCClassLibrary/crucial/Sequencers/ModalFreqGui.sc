
ModalFreqGui : KrPlayerGui {

	guiBody { arg layout;
		CXLabel(layout.startRow,"degree:");
		model.degree.gui(layout);
		CXLabel(layout.startRow,"scale:");
		model.scale.gui(layout);
		CXLabel(layout.startRow,"root:");
		model.root.gui(layout);
		CXLabel(layout.startRow,"octave:");
		model.octave.gui(layout);
		CXLabel(layout.startRow,"stepsPerOctave:");
		model.stepsPerOctave.gui(layout);
	}

}


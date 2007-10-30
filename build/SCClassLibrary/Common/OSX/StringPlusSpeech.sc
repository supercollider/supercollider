+ String {
	speak { arg channel=0, force=false;
		if(Speech.initialized.not,{Speech.init});
		Speech.channels[channel].speak(this, force)
	}
}

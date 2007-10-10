+ String {
	speak { arg voice=0, force=false;
		if(Speech.initialized.not,{Speech.init});
		Speech.channels[0].speak(this, force)
	}
}

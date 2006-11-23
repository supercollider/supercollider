+ String {
	speak { arg voice=0;
		if(Speech.initialized.not,{Speech.init});
		this.prSpeak(voice);
	}
	prSpeak { arg voice=0;
		_SpeakText
	}
}

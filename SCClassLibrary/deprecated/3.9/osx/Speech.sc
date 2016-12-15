SpeechChannel{
	var < channel, <pitch, <volume, <pitchMod, <voice, <rate;
	var < wordDoneAction, < doneAction;
	var < paused = false, isActive;


	*new{|chan|
		^super.newCopyArgs(chan);
	}

	wordDoneAction_{|action|
		Speech.wordDoneActions.put(channel, action)
	}

	doneAction_{|action|
		Speech.doneActions.put(channel, action)
	}

	pitch_{|midinote|
		pitch = midinote;
		Speech.setSpeechPitch(channel, pitch);
	}

	volume_{|amp|
		volume = amp;
		Speech.setSpeechVolume(channel, volume);
	}

	pitchMod_{|mod|
		pitchMod = mod;
		Speech.setSpeechPitchMod(channel, pitchMod);
	}

	rate_{|ratein|
		rate = ratein;
		Speech.setSpeechRate(channel, rate);
	}

	voice_{|num|
		voice = num;
		Speech.setSpeechVoice(channel, voice);
	}

	stop{|when=0|
		if(when.isNumber.not){
			when = Speech.stopMethods[when];
		};
		Speech.stop(channel, when);
	}

	pause{|bool|
		paused = bool;
		Speech.pause(channel, bool.binaryValue);
	}

	isActive{
		^this.prIsActive(channel);
	}


	speak{|string, force=false|
		this.deprecated(thisMethod);
		if(force.not){
			this.prSpeak(channel, string);
			^this
		};
		r{
			this.stop(0);
			0.5.wait;
			this.prSpeak(channel, string);
		}.play
	}

	prSpeak{|channel, txt|
		_SpeakText
		^this.primitiveFailed;
	}

	prIsActive{|chan|
		_SpeechVoiceIsSpeaking
		^this.primitiveFailed;
	}

}

Speech {
	classvar <>wordActions, <>doneActions, <>wordAction, <>doneAction, <channels;
	classvar <>initialized = false, <stopMethods;

	*setSpeechVoice { arg chan,voice;
		_SetSpeechVoice
		^this.primitiveFailed;
	}
	*setSpeechRate { arg chan,rate;
		_SetSpeechRate
		^this.primitiveFailed;
	}
	*setSpeechPitch { arg chan,pitch;
		_SetSpeechPitch
		^this.primitiveFailed;
	}
	*setSpeechPitchMod { arg chan,pitchMod;
		_SetSpeechPitchMod
		^this.primitiveFailed;
	}
	*setSpeechVolume { arg chan,volume;
		_SetSpeechVolume
		^this.primitiveFailed;
	}
	*pause { arg chan, paused=0;
		_SetSpeechPause
		^this.primitiveFailed;
	}

	//when: 0 kImmediate, 1 kEndOfWord, 2 kEndOfSentence
	*stop { arg chan, when=0;
		_SetSpeechStopAt
		^this.primitiveFailed;
	}
	//private
	*init { arg num= 1;
		this.deprecated(thisMethod);
		initialized = true;
		channels = Array.new(num);
		wordActions = Array.newClear(num);
		doneActions =  Array.newClear(num);
		num.do{|i|
			channels.add(SpeechChannel(i))
		};
		stopMethods = (immediate: 0, endOfWord: 1, endOfSentence: 2);
		this.prInitSpeech
	}
	*prInitSpeech { arg num=16;
		_InitSpeech
		^this.primitiveFailed;
	}
	*doWordAction { arg chan;
		wordAction.value(chan); //backward compatibility
		wordActions[chan].value(channels[chan]);
	}
	*doSpeechDoneAction { arg chan;
		doneAction.value(chan);
		doneActions[chan].value(channels[chan]);
	}
}

+ String {
	speak { arg channel = 0, force = false;
		var speech;
		this.deprecated(thisMethod);
		// FIXME: this should better be handled by Platform than GUI
		speech = GUI.current.speech;
		if( speech.initialized.not, { speech.init });
		speech.channels[ channel ].speak( this, force );
	}
}

//speech by jan trutzschler

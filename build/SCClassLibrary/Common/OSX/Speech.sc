
Speech {
	classvar <>wordAction, <>doneAction;
	classvar <>initialized = false;

	*setSpeechVoice { arg chan,voice;
		_SetSpeechVoice
	}
	*setSpeechRate { arg chan,rate;
		_SetSpeechRate
	}
	*setSpeechPitch { arg chan,pitch;
		_SetSpeechPitch
	}
	*setSpeechPitchMod { arg chan,pitchMod;
		_SetSpeechPitchMod
	}
	*setSpeechVolume { arg chan,volume;
		_SetSpeechVolume
	}
	*pause { arg chan, paused=0;
		_SetSpeechPause
	}

	//private
	*init { arg num= 1;
		initialized = true;
		this.prInitSpeech	
	}
	*prInitSpeech { arg num=16;
		_InitSpeech
	}
	*doWordAction { arg chan;
		wordAction.value(chan);
	}
	*doSpeechDoneAction { arg chan;
		doneAction.value(chan);
	}
}

//speech by jan trutzschler

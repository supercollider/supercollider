
Speech{
	classvar <> wordAction, <> doneAction;
	classvar <> initialized = false;

	*setSpeechRate { arg chan,rate;
		_SetSpeechRate
	}
	*setSpeechPitch { arg chan,rate;
		_SetSpeechPitch
	}
	*setSpeechPitchMod { arg chan,rate;
		_SetSpeechPitchMod
	}
	*setSpeechVoice { arg chan,rate;
		_SetSpeechVoice
	}
	*setSpeechVolume { arg chan,rate;
		_SetSpeechVolume
	}
	*pause { arg chan, paused=0;
		_SetSpeechPause
	}
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

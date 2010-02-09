
AudioInPlayer : AbstractPlayer {

	classvar <>amp=1.0; // global control
	var <>numChannels,<>firstChannel;

	*new { arg numChannels=2,firstChannel=0;
		^super.new.numChannels_(numChannels).firstChannel_(firstChannel)
	}
	storeArgs { ^[numChannels,firstChannel] }
	ar {
		^AudioIn.ar(Array.series(numChannels,firstChannel+1) )
	}
	//guiClass { ^AudioInPlayerGui }
}

MonoAudioIn : AbstractPlayer {
	classvar <>amp=1.0;
	ar {
		var a;
		a = AudioIn.ar(1,mul: amp.value);
		^[a,a]
	}
	numChannels { ^2 }

}


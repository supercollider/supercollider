
AudioInPlayer : AbstractPlayer {

	classvar <>amp=1.0; // global control

	ar {
		^AudioIn.ar([1,2])
	}
	storeParamsOn { arg stream; } // no children !!

}

MonoAudioIn : AudioInPlayer {
	
	ar {
		var a;
		a = AudioIn.ar(1,mul: amp.value);
		^[a,a]
	}
	
}

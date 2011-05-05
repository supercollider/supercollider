+ Pattern {

	asScore{|duration=1.0, timeOffset=0.0, protoEvent|
		var player;
		^ScoreStreamPlayer.new.makeScore(this.asStream, duration, protoEvent, timeOffset);
	}

}


+ Object {

	render { arg path, maxTime=60, sampleRate = 44100,
			headerFormat = "AIFF", sampleFormat = "int16", options, inputFilePath, action;

		var file, oscFilePath, score;
		oscFilePath = "temp_oscscore" ++ UniqueID.next;
		score = this.asScore(maxTime);
		score.recordNRT(
			oscFilePath, path, inputFilePath, sampleRate, headerFormat, sampleFormat,
			options, "; rm" + oscFilePath, action: action;
		);
	}

}

+ Event {
asOSC {
 	var score;
 	score = Pseq([this]).asScore.score;
 	^score.copyRange(1, score.size - 2);
 }
}

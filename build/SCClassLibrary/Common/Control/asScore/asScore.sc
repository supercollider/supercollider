+ Pattern {
	asScore { arg maxTime=60;
		^this.asStream.asScore(maxTime);
	}
}

+ Stream {
	asScore { arg maxTime=60;
		^RenderNotePlayer(maxTime).read(this).score
			
	}
}
+ Object {
	
	render { arg path, maxTime=60, sampleRate = 44100, 
			headerFormat = "AIFF", sampleFormat = "int16", options;
			
		var file, oscFilePath, inputFilePath, score;
		oscFilePath = "temp_oscscore" ++ this.hash;
		score = this.asScore(maxTime);
		score.write(oscFilePath);
		score.recordNRT(
			oscFilePath, path, inputFilePath, sampleRate, headerFormat, sampleFormat, options
		);
		// todo: remove file
		// unixCmd("rm" + oscFilePath);
		
	}

}
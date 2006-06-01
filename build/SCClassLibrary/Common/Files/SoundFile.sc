
/*
	Sound File Format strings:
		header formats:
			read/write formats:
				"AIFF", 	- Apple's AIFF
				"WAV","RIFF" 	- Microsoft .WAV
				"SD2", 	- Sound Designer 2
				"Sun", 	- NeXT/Sun
				"IRCAM", 	- old IRCAM format
				"none"	- no header = raw data
			A huge number of other formats are supported read only.
			
		sample formats:
			"int8", "int16", "int24", "int32"
			"mulaw", "alaw",
			"float"
			
		not all header formats support all sample formats.

*/

SoundFile {	
	classvar <openFiles;
	
	var <>fileptr;
	var <>headerFormat = "AIFF";
	var <>sampleFormat = "float";
	var <numFrames = 0;		// number of frames
	var <>numChannels = 1;	// number of channels
	var <>sampleRate = 44100.0;
	var <> path;
	
	
	*closeAll {
		if (openFiles.notNil, {
			openFiles.copy.do({ arg file; file.close; });
		});
	}
	isOpen {
		^fileptr.notNil
	}
	
	*new{arg pathName;
		^super.new.path_(pathName);
	}
	
	*openRead{ arg pathName;
		var file;
		file = SoundFile(pathName);
		if(file.openRead(pathName)){^file}{^nil}
	}
	
	*openWrite{ arg pathName;
		var file;
		file = SoundFile(pathName);
		if(file.openWrite(pathName)){ ^file}{^nil}
	}
	
	openRead{ arg pathName; 
		path = pathName ? path;
		^this.prOpenRead(path);
	}
	
	prOpenRead { arg pathName; 
		// returns true if success, false if file not found or error reading.
		_SFOpenRead
		^this.primitiveFailed;
	}
	
	readData { arg rawArray;
		// must have called openRead first!
		// returns true if success, false if file not found or error reading.
		_SFRead
		^this.primitiveFailed;
	}
	
	readHeaderAsString {
		// must have called openRead first!
		//returns the whole header as String
		_SFHeaderInfoString
		^this.primitiveFailed;

	}

	openWrite{ arg pathName;
		pathName = pathName ? path;
		^this.prOpenWrite(pathName)
	}
	
	prOpenWrite { arg pathName;
		// write the header
		// format written is that indicated in headerFormat and sampleFormat. 
		// return true if successful, false if not found or error writing.
		_SFOpenWrite
		^this.primitiveFailed;
	}
	writeData { arg rawArray;
		// must have called openWrite first!
		// format written is that indicated in sampleFormat. 
		// return true if successful, false if not found or error writing.
		_SFWrite
		^this.primitiveFailed;
	}
	close {
		_SFClose
		^this.primitiveFailed;
	}

	seek { arg offset, origin; 
		// offset is in frames
		// origin is an integer, one of: 
		// 0 - from beginning of file
		// 1 - from current position
		// 2 - from end of file
		_SFSeek
		^this.primitiveFailed;
	}
	
	duration { ^numFrames/sampleRate }

		// normalizer utility

	*normalize { |path, outPath, newHeaderFormat, newSampleFormat,
		startFrame = 0, numFrames, maxAmp = 1.0, linkChannels = true, chunkSize = 4194304|
		
		var	file, outFile;
		
		(file = SoundFile.openRead(path.standardizePath)).notNil.if({
				// need to clean up in case of error
			protect {
				outFile = file.normalize(outPath, newHeaderFormat, newSampleFormat,
					startFrame, numFrames, maxAmp, linkChannels, chunkSize);
			} { file.close };
			file.close;
			^outFile	
		}, {
			MethodError("Unable to read soundfile at: " ++ path, this).throw;
		});
	}
	
	normalize { |outPath, newHeaderFormat, newSampleFormat,
		startFrame = 0, numFrames, maxAmp = 1.0, linkChannels = true, chunkSize = 4194304|
		
		var	peak, outFile;
		
		outFile = SoundFile.new.headerFormat_(newHeaderFormat ?? { this.headerFormat })
			.sampleFormat_(newSampleFormat ?? { this.sampleFormat })
			.numChannels_(this.numChannels)
			.sampleRate_(this.sampleRate);
		
			// can we open soundfile for writing?
		outFile.openWrite(outPath.standardizePath).if({
			protect {
				"Calculating maximum levels...".postln;
				peak = this.channelPeaks(startFrame, numFrames, chunkSize);
				Post << "Peak values per channel are: " << peak << "\n";
				peak.includes(0.0).if({
					MethodError("At least one of the soundfile channels is zero. Aborting.",
						this).throw;
				});
					// if all channels should be scaled by the same amount,
					// choose the highest peak among all channels
					// otherwise, retain the array of peaks
				linkChannels.if({ peak = peak.maxItem });
				"Writing normalized file...".postln;
				this.scaleAndWrite(outFile, maxAmp / peak, startFrame, numFrames, chunkSize);
				"Done.".postln;
			} { outFile.close };
			outFile.close;
			^outFile
		}, {
			MethodError("Unable to write soundfile at: " ++ outPath, this).throw;
		});
	}
	
	channelPeaks { |startFrame = 0, numFrames, chunkSize = 1048576|
		var rawData, peak;

		peak = 0 ! numChannels;
		numFrames.notNil.if({ numFrames = numFrames * numChannels; },
			{ numFrames = inf });
		
			// chunkSize must be a multiple of numChannels
		chunkSize = (chunkSize/numChannels).floor * numChannels;

		this.seek(startFrame, 0);
		
		{	(numFrames > 0) and: {
				rawData = FloatArray.newClear(min(numFrames, chunkSize));
				this.readData(rawData);
				rawData.size > 0
			}
		}.while({
			rawData.do({ |samp, i|
				(samp.abs > peak[i % numChannels]).if({
					peak[i % numChannels] = samp.abs
				});
			});
			numFrames = numFrames - chunkSize;
		});
		^peak
	}
	
	scaleAndWrite { |outFile, scale, startFrame, numFrames, chunkSize|
		var	rawData;
		
		numFrames.notNil.if({ numFrames = numFrames * numChannels; },
			{ numFrames = inf });
		(scale.size == 0).if({ scale = [scale] });

			// chunkSize must be a multiple of numChannels
		chunkSize = (chunkSize/numChannels).floor * numChannels;

		this.seek(startFrame, 0);
		
		{	(numFrames > 0) and: {
				rawData = FloatArray.newClear(min(numFrames, chunkSize));
				this.readData(rawData);
				rawData.size > 0
			}
		}.while({
			rawData.do({ |samp, i|
				rawData[i] = rawData[i] * scale.wrapAt(i)
			});
				// write, and check whether successful
				// throwing the error invokes error handling that closes the files
			(outFile.writeData(rawData) == false).if({
				MethodError("SoundFile writeData failed.", this).throw
			});

			numFrames = numFrames - chunkSize;
		});
		^outFile
	}

}

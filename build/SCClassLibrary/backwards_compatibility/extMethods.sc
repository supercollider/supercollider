

+ Mix {

	*ar { arg array;
		^this.new(array);
	}
	*arFill { arg n,func;
		^this.fill(n,func);
	}
}

+ SimpleNumber {
	// rgb() still will not work in variable declarations in classes
	// because rgb is no longer a literal
	
	// move them to init methods
	
	rgb { arg green,blue;
		^Color(this / 255.0,green / 255.0, blue / 255.0)
	}
	
}

+ Synth {
	
	*stop {
		RootNode.new.freeAll;
	}
	*play { arg func;
		Server.default.doWhenBooted({		
			func.play;
		})
	}
	play {
		//if not already playing, play
	}
	*sampleRate {
		^Server.default.sampleRate ? Server.default.options.sampleRate ? 44100.0
	}
}



+ RawArray {

	write { arg path;
		var file;
		file = File.new(path, "wb");
		if (file.notNil, {
			file.write(this);
			if(file.length < this.size,{
				Error("Write file failed !!!! " ++ path).throw;
			});
			file.close;
		});
	}
	
}

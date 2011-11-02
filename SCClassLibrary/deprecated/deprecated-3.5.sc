+ Mix {

	*ar { arg array;
		this.deprecated(thisMethod, Mix.class.findMethod(\new));
		^this.new(array);
	}
	*arFill { arg n,func;
		this.deprecated(thisMethod, Mix.class.findMethod(\fill));
		^this.fill(n,func);
	}
}

+ SimpleNumber {
	// rgb() still will not work in variable declarations in classes
	// because rgb is no longer a literal

	// move them to init methods
	rgb { arg green,blue;
		this.deprecated(thisMethod, Color.class.findMethod(\new255));
		^Color(this / 255.0,green / 255.0, blue / 255.0)
	}

}

+ Synth {

	*stop {
		this.deprecated(thisMethod, Server.findMethod(\free));
		RootNode.new.freeAll;
	}
	*play { arg func;
		this.deprecated(thisMethod, Synth.class.findMethod(\new));
		Server.default.doWhenBooted({
			func.play;
		})
	}
	play {
		this.deprecated(thisMethod, Synth.class.findMethod(\new));
		//if not already playing, play
	}
	*sampleRate {
		this.deprecated(thisMethod, Server.findMethod(\sampleRate));
		^Server.default.sampleRate ? Server.default.options.sampleRate ? 44100.0
	}

}

+ RawArray {

	write { arg path;
		var file;
		this.deprecated(thisMethod, File.findMethod(\write));
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
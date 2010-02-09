
+ Object {
	writeZArchive { arg akv;
		var thing;
		thing = this.asCompileString;
		if(thing.size > 127,{
			akv.putChar($X);
			akv.writeLargeString( thing );
		} , {
			akv.putChar($x);
			akv.writeString( thing );
		})
	}
	// this.new, then new.readZArchive
	*readZArchive { arg ... args;
		var new,akv;
		new = this.new;
		akv = args.first;
		if(akv.isString,{
			akv = ZArchive.read(Document.standardizePath(akv));
			args[0] = akv;
		 });
		new.performList(\readZArchive,args);
		^new
	}
	// see Help file
	readZArchive { /*arg akv; ^akv.readItem;*/ }
}

+ Nil {
	writeZArchive { arg akv;
		akv.putChar($N);
	}
}

+ String {
	asZArchive {
		^ZArchive.write(Document.standardizePath(this))
	}
	writeZArchive { arg akv;
		if(this.size < 128,{
			akv.putChar($s);
			akv.writeString(this);
			^this
		},{	// up to 4294967296
			akv.putChar($S);
			akv.writeLargeString(this);
		});
	}
}
+ Symbol {
	writeZArchive { arg akv;
		akv.putChar($y);
		akv.writeString(this.asString);
	}
}

+ Float {
	writeZArchive { arg akv;
		akv.putChar($F);
		akv.putFloat(this);
	}
}
+ Integer {
	writeZArchive { arg akv;
		akv.putChar($I);
		akv.putInt32(this);
	}
}

/* raw arrays could cut in half by not having to repeat the class
		if(this.isKindOf(RawArray),{ // check the type of this.at(0)
			akv.putChar($S);
			classname = this.class.name.asString;
			akv.putInt8(classname.size);
			akv.putString(classname);
			akv.putInt32(this.size);
			akv.write(this); // do ?
		}
*/

// classname is written, so you will get the correct class back
+ Dictionary {
	writeZArchive { arg akv;
		var classname;
		akv.putChar($D);
		classname = this.class.name.asString;
		akv.writeString(classname);
		akv.putInt32(this.size);
		this.keysValuesDo({ arg k,v,i;
			akv.writeItem(k);
			akv.writeItem(v)
		});
	}
}

+ SequenceableCollection {
	writeZArchive { arg akv;
		var classname;
		akv.putChar($C);
		classname = this.class.name.asString;
		akv.writeString(classname);
		akv.putInt32(this.size);
		this.do({ arg it; akv.writeItem(it) });
	}
}

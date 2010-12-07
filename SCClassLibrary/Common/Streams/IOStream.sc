IOStream : Stream {
	reset { this.pos = 0; }
	skip { arg n;
		this.pos = this.pos + n;
	}
	comma { this.put(Char.comma);}
	space { this.put(Char.space); }
	nl { this.put(Char.nl); }
	ff { this.put(Char.ff); }
	tab { this.put(Char.tab); }

	<< { arg item;
		item.printOn(this);
	}
	<<< { arg item;
		item.storeOn(this);
	}
	<<* { arg collection;
		collection.printItemsOn(this);
	}
	<<<* { arg collection;
		collection.storeItemsOn(this);
	}

	readUpTo { arg delimiter = $\f;
		var string, char;
		string = String.new;
		char = this.next;
		if(char.isNil) { ^nil };
		while ({
			char.notNil and: { char != delimiter }
		},{
			string = string.add(char);
			char = this.next;
		});
		^string
	}
	flush {}
	pos_ { ^this.subclassResponsibility(thisMethod) }
}

CollStream : IOStream {
	var <>collection, <pos = 0;

	*new { arg collection;
		^super.new.collection_(collection ?? { String.new(128) })
	}
	*on { arg aCollection;
		^super.new.on(aCollection);
	}
	on { arg aCollection;
		collection = aCollection;
		this.reset;
	}
	reset { super.reset; collection = collection.extend(0) }


	// seeking
	pos_ { arg toPos;
		pos = toPos.clip(0, collection.size);
	}
	peek {
		^collection.at(pos)
	}

	next {
		if (pos >= collection.size, {
			^nil
		},{
			pos = pos + 1;
			^collection.at(pos - 1);
		})
	}
	nextN { arg n;
		^collection.species.fill(n, { this.next; });
	}
	contents {
		^collection.copyRange(0, collection.size-1);
	}

	put { arg item;
		//_RWStream_Put
		if (pos >= collection.size, {
			pos = collection.size + 1;
			collection = collection.add(item);
		},{
			collection.put(pos, item);
			pos = pos + 1;
		})
	}
	putAll { arg aCollection;
		collection = collection.overWrite(aCollection, pos);
		pos = pos + aCollection.size;
	}


//	write { arg item;
//		/* writes any of the following items as binary:
//			a double float,
//			a long,
//			an rgb color,
//			a char,
//			the name of a Symbol as chars,
//			the indexable part of any non-Slot format object,
//				(i.e. Strings, Int8Arrays, Int16Arrays,
//				Signals, etc.)
//		*/
//		_CollStream_Write
//		^this.primitiveFailed;
//	}

	getChar { ^this.next; }
	getInt8 { ^this.next & 255; }
	getInt16 { ^this.getInt8 << 8 | this.getInt8; }
	getInt32 { ^this.getInt16 << 16 | this.getInt16; }
	getFloat { ^Float.from32Bits(this.getInt32); }
	getDouble { ^Float.from64Bits(this.getInt32, this.getInt32); }

	getPascalString {
		var	size = this.getInt8;
		^String.fill(size, { this.getChar.asAscii })
	}

		// array should be some subclass of RawArray
	read { |array|
		array.readFromStream(this);
	}
//
	// collection should be an Int8Array
	putChar { arg aChar; this.put(aChar.ascii); }
	putInt8 { arg anInteger; this.put(anInteger & 255); }
	putInt16 { arg anInteger;
		this.putInt8(anInteger>>8);
		this.putInt8(anInteger);
	}
	putInt16LE { arg anInteger;
		this.putInt8(anInteger);
		this.putInt8(anInteger>>8);
	}
	putInt32 { arg anInteger;
		this.putInt8(anInteger>>24);
		this.putInt8(anInteger>>16);
		this.putInt8(anInteger>>8);
		this.putInt8(anInteger);
	}
	putInt32LE { arg anInteger;
		this.putInt8(anInteger);
		this.putInt8(anInteger>>8);
		this.putInt8(anInteger>>16);
		this.putInt8(anInteger>>24);
	}
	putFloat { arg aFloat;
		aFloat = aFloat.asFloat;
		this.putInt32(aFloat.as32Bits);
	}
	putDouble { arg aFloat;
		aFloat = aFloat.asFloat;
		this.putInt32(aFloat.high32Bits);
		this.putInt32(aFloat.low32Bits);
	}
	putFloatLE { arg aFloat;
		aFloat = aFloat.asFloat;
		this.putInt32LE(aFloat.as32Bits);
	}
	putDoubleLE { arg aFloat;
		aFloat = aFloat.asFloat;
		this.putInt32LE(aFloat.low32Bits);
		this.putInt32LE(aFloat.high32Bits);
	}
	putString { arg aString;
		aString.do({ arg char; this.putChar(char); });
	}
	putPascalString { arg aString;
		this.putInt8(aString.size);
		this.putString(aString);
	}
}

LimitedWriteStream : CollStream {
	var <>limit, <>limitFunc;

	atLimit { ^pos >= limit }
	put { arg item;
		var newpos;
		newpos = pos + 1;
		if (newpos > limit, {
			limitFunc.value;
			limitFunc = nil;
		},{
			super.put(item);
		});
	}
	putAll { arg aCollection;
		var newpos;
		newpos = pos + aCollection.size;
		if (newpos > limit, {
			aCollection = aCollection.copyFromStart(limit - pos - 1);
			collection = collection.overWrite(aCollection, pos);
			pos = limit;
			limitFunc.value;
			limitFunc = nil;
		},{
			collection = collection.overWrite(aCollection, pos);
			pos = newpos;
		});
	}

}


Post {
	classvar <>formats;
	*initClass {
		formats =  IdentityDictionary[
			$c -> { |x| x.asCompileString },
		];
	}

	//*flush { this.flushPostBuf }
	* << { arg item;
		item.printOn(this);
	}
	* <<< { arg item;
		item.storeOn(this);
	}
	* <<* { arg collection;
		collection.printItemsOn(this);
	}
	* <<<* { arg collection;
		collection.storeItemsOn(this);
	}
	* put { arg item;
		item.post;
	}
	* putAll { arg aCollection;
		aCollection.post;
	}
	* comma { this.put(Char.comma);}
	* space { this.put(Char.space); }
	* nl { this.put(Char.nl); }
	* ff { this.put(Char.ff); }
	* tab { this.put(Char.tab); }
	* close { this.flush; }
}


Pretty : IOStream {
	var <>out, <>level = 0, <>state;
	*new { arg out;
		var stream;
		stream = super.new.out_(out);
		stream.state_(PrettyEcho(stream));
		^stream
	}
	put { arg char;
		state.put(char);
	}
	close { out.close; }
}


PrettyState {
	var <>pretty;
	*new { arg pretty;
		^super.new.pretty_(pretty);
	}
}

PrettyEcho : PrettyState {
	put { arg char;
		// echo chars until new line
		if ((char == $\n) || (char == $\r), {
			pretty.out.put($\n);
			pretty.state_(PrettyEat(pretty));
		},{
			if (char == ${ , /* } */ {
				pretty.out.put($\n);
				pretty.level.do({ pretty.out.put($\t) });
				pretty.out.put(char);
				pretty.out.put($\n);
				pretty.level = pretty.level + 1;
				pretty.state_(PrettyEat(pretty));
			},{
				if ( /*{*/ char == $}, {
					pretty.level = pretty.level - 1;
					pretty.out.put($\n);
					pretty.level.do({ pretty.out.put($\t) });
					pretty.out.put(char);
					pretty.out.put($\n);
					pretty.state_(PrettyEat(pretty));
				},{
					pretty.out.put(char);
				})
			})
		});
	}
}

PrettyEat : PrettyState {
	put { arg char;
		if (char == ${, /*}*/ {
			pretty.level.do({ pretty.out.put($\t) });
			pretty.out.put(char);
			pretty.out.put($\n);
			pretty.level = pretty.level + 1;
		},{
			if (((char == $\n) || (char == $\r)) && (pretty.level == 0), {
				pretty.out.put($\n);
			},{
				if (char.isSpace.not, {
					if ( /*{*/ char == $}, {
						pretty.level = pretty.level - 1;
						pretty.level.do({ pretty.out.put($\t) });
						pretty.out.put(char);
						pretty.out.put($\n);
					},{
						pretty.level.do({ pretty.out.put($\t) });
						pretty.out.put(char);
						pretty.state_(PrettyEcho(pretty));
					});
				});
			});
		});
	}
}

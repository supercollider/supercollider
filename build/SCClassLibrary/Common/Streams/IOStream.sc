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
		while ({
			char = this.next;
			char.notNil and: { char != delimiter }
		},{
			string = string.add(char);
		});
		^string
	}
	flush {}
}

CollStream : IOStream {
	var <>collection, <pos = 0;
	
	*new {
		^super.new.collection_(String.new(128))
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

//	getChar { _CollStream_GetChar; ^this.primitiveFailed; }
//	getInt8 { _CollStream_GetInt8; ^this.primitiveFailed; }
//	getInt16 { _CollStream_GetInt16; ^this.primitiveFailed; }
//	getInt32 { _CollStream_GetInt32; ^this.primitiveFailed; }
//	getFloat { _CollStream_GetFloat; ^this.primitiveFailed; }
//	getDouble { _CollStream_GetDouble; ^this.primitiveFailed; }
//	
//	putChar { arg aChar; _CollStream_PutChar; ^this.primitiveFailed; }
//	putInt8 { arg anInteger; _CollStream_PutInt8; ^this.primitiveFailed; }
//	putInt16 { arg anInteger; _CollStream_PutInt16; ^this.primitiveFailed; }
//	putInt32 { arg anInteger; _CollStream_PutInt32; ^this.primitiveFailed; }
//	putFloat { arg aFloat; _CollStream_PutFloat; ^this.primitiveFailed; }
//	putDouble { arg aFloat; _CollStream_PutDouble; ^this.primitiveFailed; }
//	putString { arg aString; _CollStream_PutString; ^this.primitiveFailed; }

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
	*flush { this.flushPostBuf }
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

PrettyEcho : PrettyState{
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

PrettyEat : PrettyState{
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
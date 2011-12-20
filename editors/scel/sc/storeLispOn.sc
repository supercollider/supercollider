// copyright 2003 stefan kersten <steve@k-hornz.de>
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License as
// published by the Free Software Foundation; either version 2 of the
// License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301
// USA

+ Object {
	asLispString {
		var stream;
		stream = CollStream.new;
		this.storeLispOn(stream);
		^stream.collection
	}
	asLispExpression {
		^this.asLispString
	}
	storeLispOn { arg stream;
		stream << "#<" << this << ">"
	}
}

+ Symbol {
	storeLispOn { arg stream;
		stream.putAll(this.asString)
	}
}

+ String {
	asLispExpression {
		^this
	}
	storeLispOn { arg stream;
		// note, extra space after \\ is for emacs paren matching
		// not syntactically significant
		stream.put($").putAll(this.escapeChar($\\ ).escapeChar($")).put($")
	}
}

+ Char {
	storeLispOn { arg stream;
		stream.putAll("?"++this)
	}
}

+ Color {
	storeLispOn { arg stream;
		("#"++(red*255).asInteger.asHexString(2)
			++(green*255).asInteger.asHexString(2)
			++(blue*255).asInteger.asHexString(2)).storeLispOn(stream)
	}
}

+ Number {
	storeLispOn { arg stream;
		stream << this
	}
}

+ Nil {
	storeLispOn { arg stream;
		stream << "nil"
	}
}

+ True {
	storeLispOn { arg stream;
		stream.put($t)
	}
}

+ False {
	storeLispOn { arg stream;
		nil.storeLispOn(stream)
	}
}

+ Association {
	storeLispOn { arg stream;
		stream.put($();
		this.key.storeLispOn(stream);
		stream.space.put($.).space;
		this.value.storeLispOn(stream);
		stream.put($));
	}
}

+ Collection {
	lispDo { arg function;
		this.do(function);
	}

	storeLispOn { arg stream;
		stream.put($();
		this.lispDo { arg x, i;
			if (i != 0) { stream.space };
			x.storeLispOn(stream);
		};
		stream.put($));
	}
}

+ Dictionary {
	lispDo { arg function;
		this.associationsDo(function)
	}
}

// EOF
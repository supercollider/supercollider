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
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
// USA

+ Object {
	asLispString {
		var stream;
		stream = CollStream.new;
		this.storeLispOn(stream);
		^stream.collection
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
	storeLispOn { arg stream;
		stream.put($").putAll(this.escapeChar($")).put($")
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
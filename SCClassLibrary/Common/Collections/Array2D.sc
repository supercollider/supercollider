Array2D : Collection {
	var <rows, <cols, <array;
	*new { |rows = 1, cols = 1|
		^super.new.init(rows, cols)
	}
	init { |argRows, argCols|
		rows = argRows;
		cols = argCols;
		array = Array.newClear(rows * cols)
	}

	at { |row, col|
		^array.at(row * cols + col)
	}
	put { |row, col, val|
		array.put(row * cols + col, val)
	}

	asArray { ^array }
	*fromArray { |rows, cols, array|
		^this.new(rows, cols).with(array)
	}
	with { |aarray| array = aarray }

	do { |func|
		array.do(func)
	}
	colsDo { |func|
		cols.do { |ci|
			func.value(Array.fill(rows, { |ri| this.at(ri, ci) }), ci)
		}
	}
	rowsDo { |func|
		rows.do { |ri|
			func.value(Array.fill(cols, { |ci| this.at(ri, ci) }), ri)
		}
	}

	colAt { |ci|
		^Array.fill(rows, { |ri| this.at(ri, ci) })
	}
	rowAt { |ri|
		^array.copyRange(ri * cols, ri * cols + cols - 1)
	}

	add { ^this.shouldNotImplement(thisMethod) }
	remove { ^this.shouldNotImplement(thisMethod) }

	printOn { |stream|
		// not a compileable string
		stream << this.class.name << "[";
		this.rowsDo({ |r|
			r.printOn(stream)
		});
		stream << "]"
	}
	storeOn { |stream|
		var title;
		stream << this.class.name << ".fromArray("
			<<<* [rows, cols, this.asArray] << ")";
		this.storeModifiersOn(stream)
	}
}

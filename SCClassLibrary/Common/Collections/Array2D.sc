Array2D : Collection {
	var <rows, <cols, <array;
	*new { arg rows=1, cols=1;
		^super.new.init(rows, cols);
	}
	init { arg argRows, argCols;
		rows = argRows;
		cols = argCols;
		array = Array.newClear(rows * cols);
	}

	at { arg row, col;
		^array.at(row*cols + col)
	}
	put { arg row, col, val;
		array.put(row*cols + col, val)
	}

	asArray { ^array }
	*fromArray { arg rows,cols, array;
		^this.new(rows,cols).with(array);
	}
	with { arg aarray;	array = aarray; }

	do { arg func;
		array.do(func)
	}
	colsDo { arg func;
		cols.do({ arg ci;
			func.value( Array.fill(rows,{ arg ri; this.at(ri,ci) }), ci )
		})
	}
	rowsDo { arg func;
		rows.do({ arg ri;
			func.value( Array.fill(cols,{ arg ci; this.at(ri,ci) }), ri )
		})
	}

	colAt { arg ci;
		^Array.fill(rows,{ arg ri; this.at(ri,ci) })
	}
	rowAt { arg ri;
		^array.copyRange(ri * cols, ri * cols + cols - 1)
	}

	// overide Array
	// add { ^thisMethod.shouldNotImplement }
	printOn { arg stream;
		// not a compileable string
		stream << this.class.name << "[ " ;
		this.rowsDo({ arg r;
			r.printOn(stream);
		});
		stream << " ]" ;
	}
	storeOn { arg stream;
		var title;
		stream << this.class.name << ".fromArray("
			<<<* [rows,cols,this.asArray] << ")";
		this.storeModifiersOn(stream);
	}
}

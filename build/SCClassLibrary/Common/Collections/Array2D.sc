Array2D : Collection {	var <rows, <cols, <array;	*new { arg rows=1, cols=1,data; 
		// a slight hack, but it allows Array2D to be easily storeable
		^if(data.notNil,{
			this.newClear(rows,cols).with(data);
		},{
			this.newClear(rows, cols);
		});
	}
	*newClear { arg rows=1, cols=1;		^this.new(rows, cols);	}	init { arg argArray, argRows, argCols;		array = argArray;		rows = argRows;		cols = argCols;	}
	
	at { arg row, col; 
		^array.at(row*cols + col) 
	}
	put { arg row, col, val; 
		array.put(row*cols + col, val) 
	}
		asArray { ^Array.new(rows * cols).addAll(this) }
	*fromArray { arg rows,cols, array;
		^this.new(rows,cols).with(array);
	}
	with { arg array;	
		array.do({ arg item,i;
			super.put(i,item)
		})
	}

//	do { arg func;//		array.do(func)//	}
	do { arg func;
		rows.do({arg ri;
			cols.do({arg ci;
				func.value(this.at(ri,ci),ri,ci);
			})
		})
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
		// use copyRange
		^Array.fill(cols,{ arg ci; this.at(ri,ci) })
	}

	// overide Array
	printOn { arg stream;
		stream << this.class.name << "[ " ;
		this.printItemsOn(stream);
		stream << " ]" ;
	}
	storeOn { arg stream;
		var title;
		stream << this.class.name << ".new";
		this.storeParamsOn(stream);
		this.storeModifiersOn(stream);
	}
	storeParamsOn { arg stream;
		stream.storeArgs([rows,cols,this.asArray])
	}
}
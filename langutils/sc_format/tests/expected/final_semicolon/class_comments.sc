Foo {
	var a;
	bar { ^a /* c */ }
	baz { ^a // c
	}
	qux {
		// before
		a.postln; // print
		^a // return
	}
	prim { _Prim; /* c */ ^this.primitiveFailed }
	inner { ^{ a /* inner */ } // outer
	}
}

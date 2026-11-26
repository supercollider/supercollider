Foo {
	bar { |x /* in */, y = (4.dbamp)| // params
		^if(x) { y /* yes */ } { // no
			nil
		} // the if
	}
	baz { |x = /* c */ 1|
		x.do { |i| i.postln } /* each */;
		^x // return
	}
	qux { |x|
		^x /* c */ .if { 1 } { 2 } // stays
	}
}

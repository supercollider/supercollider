Foo {
	bar { arg x /* in */, y = 4.dbamp; // params
		^x.if({ y; /* yes */ }, { // no
			nil;
		}); // the if
	}
	baz { |x /* c */ 1|
		x.do({ |i| i.postln; }) /* each */;
		^x; // return
	}
	qux { arg x;
		^x /* c */ .if({ 1; }, { 2; }); // stays
	}
}

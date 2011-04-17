// method extensions
// 	add or replace methods

// This allows you to add or redefine methods without changing the original file.
// A file may contain either class definitions, or method extensions, but never both.
// Do not name an extension file with the same name as a class definition file.

+ Number {
	half { ^this * 0.5 }
	twice { ^this * 2 }
}

+ Point {
	*big { arg x, y;
		^this.new(2*x, 10*y)
	}
	swap {
		^this.class.new(y, x)
	}
	conjugate {
		^this.class.new(x, y.neg)
	}
}

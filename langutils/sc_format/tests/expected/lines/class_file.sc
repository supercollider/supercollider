Foo : Bar {
	var a;
	*new { arg x;
		^super.new.init(x)
	}
	init { |x|
		a = [
			x,
			x
		];
		^this.bar(
			a,
			x
		)
	}
}

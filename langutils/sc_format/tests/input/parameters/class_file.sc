Foo {
	*new { arg server = Server.default, channels = 2;
		^super.new.init(server, channels)
	}
	init { arg server, channels; }
	value { arg; ^1 }
	keep { |a b(1)| ^a }
}

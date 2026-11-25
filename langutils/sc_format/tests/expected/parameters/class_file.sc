Foo {
	*new { |server = (Server.default), channels = 2|
		^super.new.init(server, channels)
	}
	init { |server, channels| }
	value { ^1 }
	keep { |a, b = (1)| ^a }
}

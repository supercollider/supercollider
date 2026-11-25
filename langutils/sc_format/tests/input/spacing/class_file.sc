Foo : Bar {
	var <>a=1, b;
	classvar c=2;
	const d=3;
	*new { arg x=1 ; ^super.new.init(x) }
	init {|x| a = x+1 ; }
	value { ^[ a , b ] }
}

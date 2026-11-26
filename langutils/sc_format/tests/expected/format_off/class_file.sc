Foo {
	bytes {
		^Int8Array[
      // sc-format off
      1, 2, 3, 4,
      5, 6, 7, 8
      // sc-format on
		]
	}

	*new { |a=1|
  // sc-format off
  ^super.new.init(a)  ;
  // sc-format on
	}

	init { |a| ^a + 1 }
}

TestCollection : UnitTest {
	test_asPairs {
		this.assert(
			[2,10,100,1000]
			.collect{ |size|
				size.collect{ |i| i -> i }.asPairs.size >= (size * 2)
			}.reduce('&&'),
			"asPairs on an array of associations should create an array "
			"with size higher or equal to twice the number of elements in the original array"
		);
	}

	test_remove {
		var rejectList = [RingBuffer, ArrayedCollection, SequenceableCollection, RawArray] ++ RawArray.allSubclasses;
		// RingBuffer needs revising.
		// Subclasses of RawArray change the type so this test won't work.
		var testable = Collection.allSubclasses.reject(rejectList.includes(_));

		testable.do { |c|
			var obj, r;
			// only test classes than can be constructed like this.
			try { obj = c.newFrom([1, 2]) };
			obj !? {
				try { r = obj.remove(1) } { |e|
				    // Ignore classes that explicitly don't implement remove.
					if(e.isKindOf(ShouldNotImplementError)){
					    r = \skip;
					} {
                        r = "Error, % should implement remove".format(c);
					}
				};
				if (r !== \skip) {
                    this.assertEquals(r, 1,
                        "%.remove should return the value when present, expected % got %".format(c, 1, r)
                    )
                }
			}
		};

		testable.do { |c|
        			var obj, r;
        			// only test classes than can be constructed like this.
        			try { obj = c.new };
        			obj !? {
        				try { r = obj.remove(1) } { |e|
        				    // Ignore classes that explicitly don't implement remove.
        					if(e.isKindOf(ShouldNotImplementError)){
        					    r = \skip;
        					} {
                                r = "Error, % should implement remove".format(c);
        					}
        				};
        				if (r !== \skip) {
                            this.assertEquals(r, nil,
                                "%.remove should return nil when not present, expected % got %".format(c, nil, r)
                            )
                        }
        			}
		};
	}
}

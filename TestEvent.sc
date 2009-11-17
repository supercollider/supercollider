/*
TestEvent.run
UnitTest.gui
*/
TestEvent : UnitTest {
	
	test_equality {
		var a,b,c, x, y, keys, values;
		
		a = (a: 100, b: 200, c: [1, 2, 3]);
		b = (a: 100, b: 200, c: [1, 2, 3]);
		c = (a: 100, b: 200, c: [1, 2, 4]);
		
		this.assert( a == b, "2 identically specified Events should be equal");

		this.assert( a != c, "2 different Events should not be equal");
		
		this.assert( a == a, "an Events should equal itself");
		
		x = ().putPairs([{ |i| ("key_" ++ i).asSymbol }.dup(80), 0].flop.flat);
		keys = x.keys.asArray;
		values = keys.collect { |key| x.at(key) };
		
		this.assert(values.includes(nil).not, "an Event responds to all its keys");
		
		x.use {
			y = ().putPairs(keys.envirPairs);
		};
		
		this.assert(x == y, "an Event remains invariant under transformation to envirPairs");
		
		
		
	}
		
	
}


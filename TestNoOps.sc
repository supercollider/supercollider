/*

TestNoOps.run
UnitTest.gui

*/

TestNoOps : UnitTest {

	test_array {
		var ops, check, n = 300;
		var a, b, r1, r2;
		
		
		check = { |func, inval|
			func.value(func.value(inval)) == inval
		};
		
		a = Array.rand(300, 0, 100);
		b = a.clump(10);
		
		r1 = b.size.rand;
		r2 = b.size.rand;
		
		this.assert(n.even, "for this test, array must have even number of elements");
		
		ops = (
			\reverse: { |x| x.reverse },
			\rotate: { |x| x.rotate(x.size div: 2) },
			\swap: {|x| x.swap(r1, r2) },
			\flop: {|x| x.flop }
		);
		
		ops.keysValuesDo { |name, func|
			this.assert(check.value(func, b), "the operation '%' should be no-op when applied twice".format(name))
		};
		
		
		this.assert(b.flat == a, "flat should return a flat array of the same order");
		this.assert(b.flatten(1) == a, "flatten should return a flat array of the same order");
		this.assert(b.sum.sum == a.sum, "summing should be invariant with sub arrays");
		this.assert({ |...args| args }.flop.value(*b) == b.flop, 
			"function flop should behave in analogy to array.flop");

		
			
		
	}
	
		
	test_numerical_homomorphisms {
		var n = [
				100.rand2, // integers
				10.0.rand2, // floats
				{ 100.rand2 }.dup(8), // arrays
				{ 100.0.rand2 }.dup(8),
				Complex(100.rand2, 100.rand2), // complex
				{ 1 }, // function
			];
		var numbers = n.dup(2).allTuples.flatten(1).postcs;
		var test = { |name, f, h, a, b|
			var x = f.(h.(a, b));
			var y = h.(f.(a), f.(b));
			this.assert(this.deepEval(x) == this.deepEval(y), 
				name + "should be a homomorphism for" + [a, b]);
		};
		var c = 0.5;
		
		numbers.pairsDo { |n1, n2|
			test.value("add-mul", c * _, _ + _, n1, n2);
			test.value("add-div", _ / c, _ + _, n1, n2);			test.value("bubble-mul", _.bubble, _ * _, n1, n2);
		}
	
	}
	
	deepEval { |item|
		^if(item.isSequenceableCollection) {
			item.collect(this.deepEval(_))
		} {
			item.value
		}
	
	}
	
}
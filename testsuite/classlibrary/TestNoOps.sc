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

		a = Array.series(300, 0, 1);
		b = a.clump(10);

		r1 = 7;
		r2 = 9;

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
			10, // integers
			10.0, // floats
			{ |i| 2 + i }.dup(8), // arrays
			{ |i| 100.0 + i }.dup(8),
			Complex(100, 15), // complex
			{ 1 }, // function
			//{{ 2.0 } }.dup(4), // arrays of functions
			Operand(1),
			Maybe(2)
		];
		var numbers = n.dup(2).allTuples.flatten(1);
		var test = { |name, f, h, a, b|
			var x = f.(h.(a, b));
			var y = h.(f.(a), f.(b));
			this.assert(this.deepEval(x) == this.deepEval(y),
				name + "should be a homomorphism for % \n"
				"returned:\n%\nand:\n%\n"
				.format([a, b], x, y)
			);
		};
		var c = 0.5;

		numbers.pairsDo { |n1, n2|
			test.value("add-mul", c * _, _ + _, n1, n2);
			test.value("add-div", _ / c, _ + _, n1, n2);
			test.value("bubble-mul", _.bubble, _ * _, n1, n2);
		}

	}

	deepEval { |item|
		^if(item.isSequenceableCollection) {
			item.collect(this.deepEval(_))
		} {
			item.value.value // for combinations of functions and Operands call value of the value
		}

	}

	assertEqualValue { |a, b, comment, report = true, onFailure|
		var diff, size;

		"asserting equal value for % and %\n".postf(a, b);

		a = this.deepEval(a);
		b = this.deepEval(b);

		diff = absdif(a, b).asArray.maxItem;
		// failures with 1e-13 in new build
		^this.assert(diff < 1e-12, comment, report, { onFailure.value(diff) })
	}

	test_arithmetics {
		var n = [
			2, // integers
			3.0, // floats
			{ |i| 2 + i }.dup(4), // arrays
			{ |i| 2.0 + i }.dup(4),
			{ 2.0 }, // function
			//{{ 2.0 } }.dup(4), // arrays of functions
			Maybe(2)
		];
		var numbers = n.dup(3).allTuples;
		var farmulae = [
				"{|a,b,c| (a ** b) * (a ** c) }", "{|a,b,c| a ** (b + c) }",
			//	"{|a,b,c| (a ** b ** c) }", "{|a,b,c| a ** (b * c) }",
				"{|a,b,c| (a ** c) * (b ** c) }", "{|a,b,c| (a * b) ** c }",
				"{|a,b,c| (a ** c) / (b ** c) }", "{|a,b,c| (a / b) ** c }",
				"{|a,b,c| (a ** c.neg) }", "{|a,b,c| 1 / (a ** c) }",
				"{|a,b,c| log(a * b) }", "{|a,b,c| log(a) + log(b) }",
				"{|a,b,c| log(a ** c) }", "{|a,b,c| c * log(a) }"
		];

		numbers.do { |triple|
			("Function arguments:" + triple).postln;
			farmulae.pairsDo { |a, b, i|
				var x = a.interpret.value(*triple);
				var y = b.interpret.value(*triple);
				this.assertEqualValue(x, y, "arithmetic test " ++ i,
					onFailure:
						postf("%\nshould equal close enough\n%\n"
							"For the values: %\n. Difference: %\n", a, b, triple, _)
				);
			};
		}
	}

}
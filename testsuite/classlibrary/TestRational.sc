/*
UnitTest.gui
TestRational.run
TestRational().numTests_(100).seed_(999.rand).isVerbose_(true).run;
*/

TestRational : UnitTest {

	var <>minIntVal= -1000, <>maxIntVal=1000;
	var <>minFloatVal= -1000.1, <>maxFloatVal=1000.1;
	var <>numTests = 100;
	var <>seed = 123;
	var <>isVerbose = false;

	setUp { thisThread.randSeed = seed }

	test_Zeros {
		var array;

		this.assert(
			Rational(0,0).isNaN,
			format( "Zeros with Rational(0,0).isNaN test passed.")
		);
		this.assert(
			Rational(rrand(1,10),rrand(1,10)) != Rational(1,0),
			format( "Zeros with Rational(rrand(1,10),rrand(1,10))!=Rational(1,0) test passed.")
		);
		this.assert(
			Rational(rrand(1,10),rrand(1,10)) != Rational(0,0),
			format( "Zeros with Rational(rrand(1,10),rrand(1,10))!=Rational(0,0) test passed.")
		);
		this.assertEquals(
			Rational(1,0),
			inf,
			format( "Zeros test with Rational(1,0)==inf passed.")
		);
		this.assertEquals(
			Rational(9999,0),
			inf,
			format( "Zeros test with Rational(9999,0)==inf passed.")
		);
		100.do {
			this.assertEquals(
				Rational(9999.rand,0),
				Rational(9999.rand,0),
				format( "Zeros test with Rational(9999.rand,0)==Rational(9999.rand,0) passed."),
				isVerbose
			)
		};
		this.assertEquals(
			Rational(0,1),
			Rational(0,9999),
			format( "Zeros test with Rational(0,1)==Rational(0,9999) passed.")
		);
	}

	test_BigNumbers {

		this.assert(
			(2147483646 %/ 1) + 1 == (2147483647 %/ 1),
			format( "Big_Numbers `(2147483646 %/ 1) + 1` test passed."));

		this.assert(
			(2147483647.0 %/ 1) + 1 ==  (2147483648.0 %/ 1),
			format( "Big_Numbers with `(2147483647.0 %/ 1) + 1` test passed."));

		this.assert(
			Rational(2147483647.0, 1) * 2 == ( 4294967294.0 %/ 1),
			format( "Big_Numbers with ` Rational(2147483647.0, 1) * 2` test passed."));

		this.assert(
			(-2147483646 %/ 1) - 1 == (-2147483647 %/ 1),
			format( "Big_Numbers with `(-2147483646 %/ 1) - 1` test passed."));

	}

	test_Pi_Precision {
		var a = pi.asRational(99999);
		var b = pi.asRational(999);
		var c = pi.asRational(99);

		this.assert(
			pi.equalWithPrecision(a.asFloat, 0.000000001),
			format( "Pi_Precision with `pi.asRational(99999)` test passed.")
		);

		this.assert(
			pi.equalWithPrecision(b.asFloat, 0.00001),
			format( "Pi_Precision with `pi.asRational(999)` test passed.")
		);

		this.assert(
			pi.equalWithPrecision(c.asFloat, 0.01),
			format( "Pi_Precision with `pi.asRational(99)` test passed.")
		);

	}

	test_Reciprocals {

		numTests.do {
			var x = rrand(minIntVal,maxIntVal);
			var y = 1 + maxIntVal.rand * [-1,1].choose;
			var z = Rational(x,y);
			this.assertEquals(
				z,
				z.reciprocal.reciprocal,
				format( "Reciprocal test 1 with % passed.", z),
				isVerbose
			);
			};

		numTests.do {
			var x = rrand(1,999);
			var y = rrand(1,999);
			var z = Rational(x,y);
			this.assertEquals(
				z * z.reciprocal,
				Rational(1,1),
				format( "Reciprocal test 2 with % passed.", z),
				isVerbose
			);
			}

	}


	test_newFromString {

		numTests.do({
			var x = rrand(minIntVal,maxIntVal);
			var y = 1 + maxIntVal.rand * [-1,1].choose;
			var xStr = x.asString;
			var yStr = y.asString;
			var rat1 = Rational(x,y);
			var rat2 = Rational.newFrom(xStr ++ "/"++ yStr);

			this.assertEquals(
				rat1,
				rat2,
				format( "String entry test with % passed.", rat1),
				isVerbose
			);
		});
	}

	test_strangeStringInput {

		var strangeStrings = [
			"3 %/ 2  ",
			"3  /  2 ",
			"3    /   2 ",
			"   3    / 2    ",
			"    3     / 2      ",
			"    3   %    /   2   ",
			"    3    /    2 ",
			"        3       %      /         2 "
		];

		strangeStrings.do({ arg i, j;
			this.assertEquals(
				Rational(i),
				Rational(3,2),
				format( "String entry # % test with Rational(%) passed.", j, i),
				isVerbose
			);

		});
	}

	test_StringInput_Floats {
		this.assertEquals(
			"1 / 0.1".asRational,
			Rational(10,1),
			format( "String input test with '1 / 0.1' as Rational(10,1) passed.")
		)
	}

	test_commutativeAdd {

		numTests.do {
			var x1 = rrand(minIntVal,maxIntVal);
			var y1 = 1 + maxIntVal.rand * [-1,1].choose;
			var x2 = rrand(minIntVal,maxIntVal);
			var y2 = 1 + maxIntVal.rand;
			var z1 = Rational(x1,y1);
			var z2 = Rational(x2,y2);
			this.assertEquals(
				z1 + z2,
				z2 + z1,
				format( "Commutative Add test with % and % passed.", z1, z2),
				isVerbose
			);
			}
	}

	test_commutativeMul {

		numTests.do {
			var x1 = rrand(minIntVal,maxIntVal);
			var y1 = 1 + maxIntVal.rand * [-1,1].choose;
			var x2 = rrand(minIntVal,maxIntVal);
			var y2 = 1 + maxIntVal.rand * [-1,1].choose;
			var z1 = Rational(x1,y1);
			var z2 = Rational(x2,y2);
			this.assertEquals(
				z1 * z2,
				z2 * z1,
				format( "Commutative Add test with % and % passed.", z1, z2),
				isVerbose
			);
			}
	}

	test_Additive_Inverse {


		numTests.do {
			var x = rrand(minIntVal,maxIntVal);
			var y = 1 + maxIntVal.rand * [1,1.neg].choose;
			var rat=Rational(x,y);

			this.assertEquals(
				Rational(x * (-1), y),
				Rational(x, y * (-1)),
				format( "Additive Inverse test 1 with % passed.", rat),
				isVerbose
			);
			this.assertEquals(
				(-1) * rat,
				Rational(x * (-1), y),
				format( "Additive Inverse test 2 with % passed.", rat),
				isVerbose
			);
			this.assertEquals(
				(-1) * rat,
				Rational(x, y * (-1)),
				format( "Additive Inverse test 3 with % passed.", rat),
				isVerbose
			)
		}
	}

	test_Multiplicative_Inverse {

		numTests.do {
			var x    = 1 + maxIntVal.rand * [-1,1].choose;
			var y    = 1 + maxIntVal.rand * [-1,1].choose;
			var rat1 = Rational(x,y);
			var rat2 = Rational(y,x);

			this.assertEquals(
				rat1.pow(-1),
				rat2,
				format( "Multiplicative Inverse test with % passed.", rat1),
				isVerbose
			);
		}
	}

	test_Div_Eq_ReciprocalMul  {

		numTests.do {
			var x1 = rrand(minIntVal,maxIntVal);
			var y1 = 1 + maxIntVal.rand * [-1,1].choose;
			var x2 = 1 + maxIntVal.rand * [-1,1].choose;
			var y2 = 1 + maxIntVal.rand * [-1,1].choose;
			var z1 = Rational(x1,y1);
			var z2 = Rational(x2,y2);
			this.assertEquals(
				z1 / z2,
				z1 * z2.reciprocal,
				format( "Division is equivalent to multiplying by the reciprocal test with % and %", z1, z2),
				isVerbose
			);
			}
	}

	test_Neg_Subtraction {

		numTests.do {
			var x1 = rrand(minIntVal,maxIntVal);
			var y1 = 1 + maxIntVal.rand * [-1,1].choose;
			var x2 = rrand(minIntVal,maxIntVal);
			var y2 = 1 + maxIntVal.rand * [-1,1].choose;
			var z1 = Rational(x1,y1);
			var z2 = Rational(x2,y2);
			this.assertEquals(
				z1 - z2,
				z2.neg - z1.neg,
				format( "Subtraction and Inverse test with % and % passed.", z1, z2),
				isVerbose
			);
			}
	}

	test_reciprocal_Div {

		numTests.do {
			var x1 = 1 + maxIntVal.rand * [-1,1].choose;
			var y1 = 1 + maxIntVal.rand * [-1,1].choose;
			var x2 = 1 + maxIntVal.rand * [-1,1].choose;
			var y2 = 1 + maxIntVal.rand * [-1,1].choose;
			var z1 = Rational(x1,y1);
			var z2 = Rational(x2,y2);
			this.assertEquals(
				z1 / z2,
				(z2 / z1).reciprocal,
				format( "Div + reciprocal test with % and % passed.", z1, z2),
				isVerbose
			);
			}
	}

	test_Sort_and_Scramble  {
		var listSize = 100;

		numTests.do {
			var ratList;
			ratList = Array.fill(
				listSize,
				{
					Rational(
						rrand(minIntVal, maxIntVal),
						 1 + maxIntVal.rand * [-1,1].choose;)
				}
				);
			this.assertEquals(
				ratList.sort,
				ratList.scramble.sort,
				format( "Sort and Scramble test passed."),
				isVerbose
			);
		}
	}

	test_Sort_and_asFloat  {
		var listSize = 100;

		numTests.do {
			var ratList;

			ratList = Array.fill(
				listSize,
				{
					Rational(
						rrand(minIntVal, maxIntVal),
						1 + maxIntVal.rand * [-1,1].choose;)
				}
				);

			this.assertEquals(
				ratList.scramble.sort.asFloat,
				ratList.scramble.asFloat.sort,
				format( "Sort and asFloat test passed."),
				isVerbose
			);
		}
	}

	test_Mul_Inverse_DifferentExponents_NonZeroIntInput {

		numTests.do {
			var maxVal = 100;
			var x1 = 1 + maxVal.rand * [-1,1].choose;
			var y1 = 1 + maxVal.rand * [-1,1].choose;
			var rat = Rational(x1,y1);
			var maxExponent = 6;

			maxExponent.do({arg i;
				this.assertEquals(
					rat.pow(i * (-1)),
					rat.pow(i).reciprocal,
					format( "Multiplicative inverse with different exponents test with exp: % and rat: % passed.", i, rat),
					isVerbose
				);

			})

		}
	}

	test_Float_Rat_Float {

		numTests.do {
			var maxVal = 1000.01;
			var x = 1 + maxVal.rand * [1,1.neg].choose;
			var y = 1 + maxVal.rand * [1,1.neg].choose;
			var rat = Rational(x,y);
			var float = x/y;

			this.assertFloatEquals(
				a: rat.asFloat,
				b: float.asRational.asFloat,
				message: format(
					"Float/Rational conversion with  rat: % and float: % passed.", rat, float),
				within: 0.000001,
				report: isVerbose
			);
		}
	}

	test_Rat_Float_Rat {

		numTests.do {
			var maxVal = 100;
			var x = 1 + maxVal.rand * [1,1.neg].choose;
			var y = 1 + maxVal.rand * [1,1.neg].choose;
			var rat = Rational(x,y);
			var float = x/y;

			this.assertEquals(
				a: rat,
				b: float.asRational.asFloat.asRational,
				message: format(
					"Float/Rational conversion with rat: % and float: % passed.", rat, float),
				report: isVerbose
			);
		}
	}


	test_Exponentiation {

	   numTests.do {
			var maxVal = 9;
			var x = 1 + maxVal.rand;
			var y = 1 + maxVal.rand;
			var r = Rational(x,y);
			var e = rrand(1, 5).round;
			this.assertEquals(
				r,
				r.pow(e).pow(e.reciprocal),
				format(
					"Exponentiation rational: % and exponent: % passed.", r, e),
				report: isVerbose
			);
		}
	}

	// associative property
	test_AssociativeAdd {

		numTests.do {
			var x1 = rrand(minIntVal,maxIntVal);
			var y1 = 1 + maxIntVal.rand * [-1,1].choose;
			var x2 = rrand(minIntVal,maxIntVal);
			var y2 = 1 + maxIntVal.rand * [-1,1].choose;
			var x3 = rrand(minIntVal,maxIntVal);
			var y3 = 1 + maxIntVal.rand * [-1,1].choose;
			var z1 = Rational(x1,y1);
			var z2 = Rational(x2,y2);
			var z3 = Rational(x3,y3);
			this.assertEquals(
				(z1 + z2) + z3,
				z1 + (z2 + z3),
				format( "Associative Sum test with %, % and %passed.", z1, z2, z3),
				report: isVerbose
			)
		}
	}

	test_AssociativeMul {

		numTests.do {
			var x1 = rrand(minIntVal,maxIntVal);
			var y1 = 1 + maxIntVal.rand * [-1,1].choose;
			var x2 = rrand(minIntVal,maxIntVal);
			var y2 = 1 + maxIntVal.rand * [-1,1].choose;
			var x3 = rrand(minIntVal,maxIntVal);
			var y3 = 1 + maxIntVal.rand * [-1,1].choose;
			var z1 = Rational(x1,y1);
			var z2 = Rational(x2,y2);
			var z3 = Rational(x3,y3);

			this.assertEquals(
				(z1 * z2) * z3,
				z1 * (z2 * z3),
				format( "Associative Mul test with %, % and %passed.", z1, z2, z3),
				report: isVerbose
			);
		}
	}

	//Distributive property of multiplication over itself
	// (a + b) * c = (a * c) + (b * c)

	test_Distributive_1_PositiveIntInput {

		numTests.do {
			var minVal = 1;
			var maxVal = 20;
			var x1 = rrand(minVal,maxVal);
			var y1 = 1 + maxVal.rand ;
			var x2 = rrand(minVal,maxVal);
			var y2 = 1 + maxVal.rand ;
			var x3 = rrand(minVal,maxVal);
			var y3 = 1 + maxVal.rand ;
			var a = Rational(x1,y1);
			var b = Rational(x2,y2);
			var c = Rational(x3,y3);

			this.assertEquals(
				(a + b) * c,
				(a * c) + (b * c),
				format( "Associative property 1 test with %, % and % passed.", a, b, c),
				report: isVerbose
			);
		}
	}


	test_commutativeAdd_Array {

		numTests.do {
			var n = 20;
			var minVal = -10;
			var maxVal = 10;
			var xs = Array.fill(n, {rrand(minVal,maxVal)});
			var ys = Array.fill(n, {1 + maxVal.rand * [-1,1].choose});
			var rats;

			rats = n.collect({arg i; Rational(xs[i], ys[i])});

			this.assertEquals(
				rats.scramble.sum,
				rats.scramble.sum,
				format( "Commutative with Array summation test with %", rats),
				report: isVerbose
			);
		}
	}
}

/*
UnitTest.gui
TestRational.run
TestRational().numTests_(100).seed_(999.rand).isVerbose_(true).run;
*/

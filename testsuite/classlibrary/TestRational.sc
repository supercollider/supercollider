/*
TestRational.run
UnitTest.gui
*/

TestRational : UnitTest {

    var minIntVal= -1000, maxIntVal=1000;
    var minFloatVal= -1000.1, maxFloatVal=1000.1;
    var numTests = 1000;

    test_reciprocal_NonZeroIntInput {
        numTests.do {
            var x = rrand(minIntVal,maxIntVal);
            var y = 1 + maxIntVal.rand * [-1,1].choose;
            var z = Rational(x,y);
            this.assertEquals(
                z,
                z.reciprocal.reciprocal,
                format( "Reciprocal test with % passed.", z)
            );
            }
    }

    test_newFromString_NonZeroIntInput {
        numTests.do {
            var x = rrand(minIntVal,maxIntVal);
            var y = 1 + maxIntVal.rand * [-1,1].choose;
            var xStr = x.asString;
            var yStr = y.asString;
            var rat1 = Rational(x,y);
            var rat2 = Rational.newFrom(xStr ++ "/"++ yStr);
            this.assertEquals(
                rat1,
                rat2,
                format( "String entry test with % passed.", rat1)
            );
        }
    }

    test_commutativeAdd_NonZeroIntInput {
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
                format( "Commutative Add test with % and % passed.", z1, z2));
            }
    }

    test_commutativeMul_NonZeroIntInput {
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
                format( "Commutative Add test with % and % passed.", z1, z2));
            }
    }

    test_Additive_Inverse_NonZeroIntInput {
        numTests.do {
            var x = rrand(minIntVal,maxIntVal);
            var y = 1 + maxIntVal.rand * [1,1.neg].choose;
            var rat=Rational(x,y);

            this.assertEquals(
                Rational(x * (-1), y),
                Rational(x, y * (-1)),
                format( "Additive Inverse test 1 with % passed.", rat)
            );
            this.assertEquals(
                (-1) * rat,
                Rational(x * (-1), y),
                format( "Additive Inverse test 2 with % passed.", rat)
            );
            this.assertEquals(
                (-1) * rat,
                Rational(x, y * (-1)),
                format( "Additive Inverse test 3 with % passed.", rat)
            )
        }
    }

    test_Multiplicative_Inverse_NonZeroIntInput {
        numTests.do {
            var x    = 1 + maxIntVal.rand * [-1,1].choose;
            var y    = 1 + maxIntVal.rand * [-1,1].choose;
            var rat1 = Rational(x,y);
            var rat2 = Rational(y,x);

            this.assertEquals(
                rat1.pow(-1),
                rat2,
                format( "Multiplicative Inverse test with % passed.", rat1)
            );
        }
    }

    test_Div_Eq_ReciprocalMul_NonZeroIntInput  {
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
                format( "Division is equivalent to multiplying by the reciprocal test with % and %", z1, z2));
            }
    }

    test_Neg_Subtraction_NonZeroIntInput {
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
                format( "Subtraction and Inverse test with % and % passed.", z1, z2));
            }
    }

    test_reciprocal_Div_NonZeroIntInput {
        numTests.do {
            var x1 = rrand(minIntVal,maxIntVal);
            var y1 = 1 + maxIntVal.rand * [-1,1].choose;
            var x2 = rrand(minIntVal,maxIntVal);
            var y2 = 1 + maxIntVal.rand * [-1,1].choose;
            var z1 = Rational(x1,y1);
            var z2 = Rational(x2,y2);
            this.assertEquals(
                z1 / z2,
                (z2 / z1).reciprocal,
                format( "Div + reciprocal test with % and % passed.", z1, z2));
            }
    }

    test_Sort_and_Scramble_NonZeroIntInput  {
        var listSize = 1000;

        //numTests.do {
        10.do {
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
                format( "Sort and Scramble test passed.")
            );
        }
    }

    test_Sort_and_asFloat_NonZeroIntInput  {
        var listSize = 1000;

        //numTests.do {
        10.do {
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
                format( "Sort and asFloat test passed.")
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
                format( "Multiplicative inverse with different exponents test with exp: % and rat: % passed.", i, rat));

            })

            }
    }
}

/*
TestRational.run
UnitTest.gui
*/
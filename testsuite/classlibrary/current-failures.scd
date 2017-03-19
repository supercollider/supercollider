/*

This is a log of failures at the time of the commit.
The tests were done on OSX 10.10.3 with scserver

In order to make the test package automatable, the respective cases were excluded until fixed.

*/


UNIT TEST.............
There were failures:
a TestCoreUGens:test_ugen_generator_equivalences - "DelayC == BufDelayC [control rate]"
2283 of 44100 items in array failed to match. Displaying arrays from index of first failure (3329) onwards:
FloatArray[ -0.0019272565841675, -0.0027968883514404, -0.0035389065742493, -0.0041088461875916, -0.0044671893119812, -0.0045804977416992, -0.0044227689504623, -0.0039761811494827, -0.0032329335808754, -0.0021912027150393, -0.00085901422426105, 0.00074509554542601, 0.0025933999568224, 0.0046492516994476, 0.0068680867552757, 0.0091980397701263, 0.011581495404243, 0.013958901166916, 0.016267150640488, 0.018439888954163, 0.020408034324646, 0.022104442119598, 0.023466646671295, 0.024437189102173, 0.0249652862548...etc...
! =
0

a TestCoreUGens:test_ugen_generator_equivalences - "DelayC == BufDelayC [audio rate]"
44 of 44100 items in array failed to match. Displaying arrays from index of first failure (9) onwards:
FloatArray[ 0.0034608840942383, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -0.0084257125854492, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, ...etc...
! =
0

a TestFilterUGens:test_time_invariance - Friction.ar is time invariant when delayed by 1 sample(s).
4184 of 4410 items in array failed to match. Displaying arrays from index of first failure (226) onwards:
FloatArray[ nan, nan, nan, nan, nan, nan, nan, nan, nan, nan, nan, nan, nan, nan, nan, nan, nan, nan, nan, nan, nan, nan, nan, nan, nan, nan, nan, nan, nan, nan, nan, nan, nan, nan, nan, nan, nan, nan, nan, nan, nan, nan, nan, nan, nan, nan, nan, nan, nan, nan, nan, nan, nan, nan, nan, nan, nan, nan, nan, nan, nan, nan, nan, nan, nan, nan, nan, nan, nan, nan, nan, nan, nan, nan, nan, nan, nan, nan, nan, nan, nan, nan, nan, nan, nan, nan, nan, nan, nan, nan, nan, nan, nan, nan, nan, nan, nan, nan, nan, nan, ...etc...
! =
0

a TestFilterUGens:test_time_invariance - APF.ar is time invariant when delayed by 1 sample(s).
292 of 4410 items in array failed to match. Displaying arrays from index of first failure (2) onwards:
FloatArray[ 0.64000004529953, 1.661988735199, 3.2939584255219, 3.805920124054, 3.5789890289307, 3.2166123390198, 2.9608206748962, 2.7842793464661, 2.5023679733276, 2.3033747673035, 2.1660342216492, 1.9467208385468, 1.7919135093689, 1.6850693225861, 1.514454126358, 1.3940217494965, 1.3109019994736, 1.1781717538834, 1.0844811201096, 1.0198180675507, 0.91656041145325, 0.84367364645004, 0.79336893558502, 0.71303939819336, 0.65633714199066, 0.61720246076584, 0.55471003055573, 0.51059836149216, 0.48015353083611, ...etc...
! =
0

a TestFilterUGens:test_time_invariance - Ramp.ar is time invariant when delayed by 1 sample(s).
4408 of 4410 items in array failed to match. Displaying arrays from index of first failure (2) onwards:
FloatArray[ -0.00022675737272948, -0.00045351474545896, -0.00068027211818844, -0.00090702949091792, -0.0011337868636474, -0.0013605442363769, -0.0015873016091064, -0.0018140589818358, -0.00204081623815, -0.0022675737272948, -0.002494330983609, -0.0027210884727538, -0.0029478457290679, -0.0031746032182127, -0.0034013604745269, -0.0036281179636717, -0.0038548752199858, -0.0040816324763, -0.0043083899654448, -0.0045351474545896, -0.0047619049437344, -0.0049886619672179, -0.0052154194563627, -0.0054421769455075...etc...
! =
0

a TestFilterUGens:test_time_invariance - NestedAllpassN.ar is time invariant when delayed by 64 sample(s).
4 of 4410 items in array failed to match. Displaying arrays from index of first failure (1586) onwards:
FloatArray[ -0.0017358231125399, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,...etc...
! =
0

a TestFilterUGens:test_time_invariance - NestedAllpassN.ar is time invariant when delayed by 1 sample(s).
4 of 4410 items in array failed to match. Displaying arrays from index of first failure (1586) onwards:
FloatArray[ -1.6264470659166e-08, -2.4869824082063e-12, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, ...etc...
! =
0

a TestFilterUGens:test_time_invariance - NestedAllpassC.ar is time invariant when delayed by 1 sample(s).
28 of 4410 items in array failed to match. Displaying arrays from index of first failure (1586) onwards:
FloatArray[ 1.0485844541108e-05, -0.23041822016239, -2.1668151021004e-05, 2.4288892745972e-06, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, ...etc...
! =
0

a TestFilterUGens:test_time_invariance - NestedAllpassC.ar is time invariant when delayed by 64 sample(s).
29 of 4410 items in array failed to match. Displaying arrays from index of first failure (1650) onwards:
FloatArray[ -0.21407826244831, -0.050686255097389, -0.03088167309761, 0.0034960433840752, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,...etc...
! =
0

a TestFilterUGens:test_time_invariance - DoubleNestedAllpassC.ar is time invariant when delayed by 1 sample(s).
907 of 4410 items in array failed to match. Displaying arrays from index of first failure (972) onwards:
FloatArray[ 0.050404109060764, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0...etc...
! =
0

a TestFilterUGens:test_time_invariance - DoubleNestedAllpassL.ar is time invariant when delayed by 64 sample(s).
175 of 4410 items in array failed to match. Displaying arrays from index of first failure (2006) onwards:
FloatArray[ -9.1525726020336e-07, -2.7474015951157e-06, 3.6619603633881e-06, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, ...etc...
! =
0

a TestFilterUGens:test_time_invariance - DoubleNestedAllpassC.ar is time invariant when delayed by 64 sample(s).
911 of 4410 items in array failed to match. Displaying arrays from index of first failure (1034) onwards:
FloatArray[ -0.0048005161806941, 0.050404109060764, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0...etc...
! =
0

a TestFilterUGens:test_time_invariance - Streson.ar is time invariant when delayed by 1 sample(s).
443 of 4410 items in array failed to match. Displaying arrays from index of first failure (131) onwards:
FloatArray[ 5.8128750701769e-09, -5.6251883506775e-07, -4.1723251342773e-07, 5.9604644775391e-08, -7.4505805969238e-09, 4.6566128730774e-10, -5.8207660913467e-11, 7.2759576141834e-12, -9.0949470177293e-13, 5.6843418860808e-14, -7.105427357601e-15, 8.8817841970013e-16, -1.1102230246252e-16, 1.3877787807814e-17, -1.7347234759768e-18, 1.0842021724855e-19, -1.3552527156069e-20, 1.6940658945086e-21, -2.1175823681358e-22, 2.6469779601697e-23, -3.3087224502121e-24, 4.1359030627651e-25, -5.1698788284564e-26, 6.4623...etc...
! =
0

a TestFilterUGens:test_time_invariance - Streson.ar is time invariant when delayed by 64 sample(s).
672 of 4410 items in array failed to match. Displaying arrays from index of first failure (131) onwards:
FloatArray[ 6.6979584516957e-05, -2.3753029381623e-05, 2.6391808205517e-06, -2.9323734906939e-07, 3.2581375819518e-08, -3.6200915598528e-09, 4.0222558617131e-10, -4.4690973144412e-11, 4.9655795968107e-12, -5.5172170001019e-13, 6.1301373590998e-14, -6.8111482064506e-15, 7.5678142790568e-16, -8.4085397425298e-17, 9.3426633931935e-18, -1.0380561442554e-18, 1.153376185186e-19, -1.2815073829551e-20, 1.4238729817783e-21, -1.5820542345335e-22, 1.7578081843679e-23, -1.9530870726951e-24, 2.1700599613211e-25, -2.4111...etc...
! =
0


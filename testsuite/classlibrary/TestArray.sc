/*
TestArray.run
UnitTest.gui
*/

TestArray : UnitTest {

	test_array_nonmutating {
		var ops, classes, apply;
		classes = [
			Array,
			List,
			Set,
			IdentitySet,
			Bag,
			LinkedList,
			Order,
			SparseArray
		];

		// only ops are applied where they are implemented

		ops = [
			\copy,
			\collect,
			\do,
			\reverse,
			\mirror,
			\mirror1,
			\mirror2,
			\stutter,
			\dupEach,
			\rotate,
			\pyramid,
			\pyramidg,
			[\lace, 4],
			[\permute, 4],
			[\wrapExtend, 4],
			[\foldExtend, 4],
			[\clipExtend, 4],
			[\collectAs, _ + 1, Set],
			[\select, _ == 0],
			[\reject, _ == 0],
			\flop,
			[\as, IdentitySet],
			['++', [1, 2, 3]],
			['+++', [1, 2, 3]],
			[\copyRange, [0, 3]],
			[\keep, 4],
			[\drop, 4],
			[\copyToEnd, 3],
			[\copyFromStart, 3],
			[\slide, 3],
		];
		apply = {:[class, op], class<-classes, op<-ops };
		apply.do { |pair|
			var val1, val2, class, op;
			try {	// only apply where it works
				#class, op = pair;
				val1 = class.fill(10, { 1.0.rand });
				val2 = val1;
				val1.perform(*op);
				this.assert(val1 == val2, "operation '%' should not modify the original %".format(op.asArray.first, class))
			}
		};

		classes.do { |class|
			var coll = class.fill(10, { 1.0.rand });
			coll = coll.add(pi);
			this.assert(coll.includes(pi), "adding something to an array it should include it afterwards")
		};

	}

	test_arrayshape {
		var maxLength = 8, maxDepth = 4;
		var deepArray = { |depth|
			if(depth == 0) {
				1000.rand
		} {
				{ deepArray.value(depth - 1) } ! rrand(1, maxLength) }
		};
		20.do {
			var x = deepArray.(rrand(2, maxDepth));
			var y = x.flat.reshapeLike(x);
			this.assertEquals(x, y, "reshape and flat should be congruent")
		}
	}


	test_arraystats {
		var arsize, clumpsize, a, aclumped, ascaled, arev, mean, median, sd;

		#[
			// arsize, clumpsize -- different sizes to test:
			[100,     10],
			[121,     11]
		].do{ |settings|

			arsize    = settings[0];
			clumpsize = settings[1];

			// Create uniform distrib
			a = {1.0.rand}.dup(arsize);

			// sanity of mean, median, stdDev calculations on uniform distribution
			mean = a.mean;
			this.assert((0.2<mean) && (mean<0.8), "mean of uniform expected to be around 0.5");
			median = a.median;
			this.assert((0.2<median) && (median<0.8), "median of uniform expected to be around 0.5");
			if(Collection.findMethod(\stdDev).notNil){
				// stdDev NOT IN CORE!
				// TODO: probably better to move to a dedicated TestMathLib quark
				sd = a.stdDev(mean);
				this.assert((0<sd) && (sd<(2*sqrt(1/12))), "stdDev of uniform expected to be around sqrt(1/12)");
			};

			// sort-based median should be same as hoare-based median
			this.assertEquals(a.median, a.sort.sortedMedian, ".median (Hoare method) should be same as .sort.sortedMedian");

			// check that scrambling doesn't knacker the median (which uses partial sorting)
			10.do{
				this.assertEquals(a.scramble.median, a.scramble.median, "median should be invariant to scrambling");
			};

			// reversing the array shouldn't affect the stats!
			arev = a.reverse;
			this.assertEquals(a.mean,   arev.mean,        "mean   should be invariant to reversing");
			this.assertEquals(a.median, arev.median,      "median should be invariant to reversing");
			if(Collection.findMethod(\stdDev).notNil){
				// stdDev NOT IN CORE!
				this.assertFloatEquals(a.stdDev, arev.stdDev, "stdDev should be invariant to reversing");
			};

			// scale array up by 2^16 - power-of-two scaling should preserve precision
			ascaled = a * 65536.0;

			// these stats should simply scale linearly
			this.assertEquals(ascaled.mean   / 65536.0, mean,    "mean   should be invariant to linear scaling");
			this.assertEquals(ascaled.median / 65536.0, median,  "median should be invariant to linear scaling");
			// stdDev NOT IN CORE! this.assertFloatEquals(ascaled.stdDev / 65536.0, sd, "stdDev should be invariant to linear scaling");

			// Clump into array-of-arrays
			aclumped = a.clump(clumpsize);

			// mean-of-mean on clumped should be same as mean of array
			this.assertFloatEquals(aclumped.mean.mean, mean,     "mean should be invariant to clumping");

			// Insert 0 and 1 into distrib, to guarantee normalization should return same values
			a[(a.size/2).rand] = 0;
			a[(a.size/2).rand + (a.size/2)] = 1;

			// check sanity of maxItem and minItem
			this.assertEquals(a.minItem, 0, "minItem in our array is definitely 0");
			this.assertEquals(a.maxItem, 1, "maxItem in our array is definitely 1");

			// check if normalized == orig
			this.assertEquals(a.normalize, a, "normalising an array that's already 0--1 should be no-op");
			this.assertEquals(a.normalize(-128, 128).normalize(), a, "normalising an array that's already 0--1 should be no-op");
			this.assertEquals(a.integrate.differentiate, a, "integrate should be the inverse of differentiate");
			this.assertEquals(a.differentiate.integrate, a, "differentiate should be the inverse of integrate");
		}

	} // End test_arraystats

	// FIXME/TODO Consider whether this test should be moved into a separate file of GC tests (or superceded by one)
	test_flop_noGC_regression {
		var a, b, arraySize;
		arraySize = 32; // this causes flop to fail

		1000.do { |i|
			var indices;
			indices = (0..arraySize);

			// run identical processing on idx:
			a = [indices].flop.collect {|lm| lm[0] }; // << .lm called in the loop
			b = [indices].flop.collect {|lm| lm[0] }; // << .lm called in the loop

			// are results equal? >> NO
			if(a!=b, {
				this.assertEquals(a, b, "flop: identical flops should match", onFailure: {
					format("\nWARNING: mismatch on iter: %", i).postln;
					a.join(" ").postln;
					b.join(" ").postln;
				});
			});
		}
	}

	// ----- editDistance and similarity --------------------------------------------

	test_editDistance_emptyThis_isSizeThat {
		var result = [].editDistance([0,1,2,3]);
		this.assertEquals(result, 4);
	}

	test_editDistance_emptyThat_isSizeThis {
		var result = [0,1,2,3].editDistance([]);
		this.assertEquals(result, 4);
	}

	test_editDistance_emptyThis_emptyThat_isZero {
		var result = [].editDistance([]);
		this.assertEquals(result, 0);
	}

	test_editDistance_differentTypes_isOk {
		var result = [0,1,2,3].editDistance([\a,\b,\c,\d]);
		this.assertEquals(result, 4);
	}

	test_editDistance_mixedTypes_isOk {
		var result = [0,1,2,3].editDistance([0,"hello",2,3]);
		this.assertEquals(result, 1);
	}

	test_editDistance_countsSubstitution {
		var result = [0,1,2,3].editDistance([0,1,0,3]);
		this.assertEquals(result, 1);
	}

	test_editDistance_countsAddition {
		var result = [0,1,2,3].editDistance([0,1,2,3,4]);
		this.assertEquals(result, 1);
	}

	test_editDistance_countsRemoval {
		var result = [0,1,2,3].editDistance([0,1,2]);
		this.assertEquals(result, 1);
	}

	test_editDistance_countsChanges {
		var result = [0,1,2,3].editDistance([4,5,6,7,8]);
		var expected = 5; // 0:4, 1:5, 2:6, 3:8, addition:8

		this.assertEquals(result, expected);
	}

	test_editDistance_rawIntArraysSameType {
		var result = Int8Array[0, 1].editDistance(Int8Array[0, 1]);
		this.assertEquals(result, 0);
	}

	test_editDistance_rawIntArraysDifferentTypes {
		var result = Int8Array[0, 1].editDistance(Int16Array[0, 1]);
		this.assertEquals(result, 0);
	}

	test_editDistance_rawIntArrayAndArray {
		var result = Int8Array[0, 1].editDistance(Array[0, 1]);
		this.assertEquals(result, 0);
	}

	test_editDistance_arrayAndLinkedList {
		var result = Array[0, 1].editDistance(LinkedList[0, 1]);
		this.assertEquals(result, 0);
	}

	test_editDistance_listAndLinkedList {
		var result = List[0, 1].editDistance(LinkedList[0, 1]);
		this.assertEquals(result, 0);
	}

	test_editDistance_rawArrayAndPolymorphicArray {
		var result = Int8Array[0, 1].editDistance(Array[0, \x]);
		this.assertEquals(result, 1);
	}

	test_editDistance_nilCompareFunc_testsIdentityForRawArrays {
		this.assertEquals(Array["a", "b"].editDistance(Array["a", "b"]), 2);
		this.assertEquals(Array[\a, \b].editDistance(Array[\a, \b]), 0);
	}

	test_editDistance_nilCompareFunc_testsIdentityForLists {
		// we also want to check that non-raw arrays use identity comparison by default
		this.assertEquals(List["a", "b"].editDistance(List["a", "b"]), 2);
		this.assertEquals(List[\a, \b].editDistance(List[\a, \b]), 0);
	}

	test_editDistance_usesCompareFuncForRawArrays {
		var result = Array["a", "b"].editDistance(Array["a", "c"], _ == _);
		this.assertEquals(result, 1);
	}

	test_editDistance_usesCompareFuncForLists {
		var result = List["a", "b"].editDistance(List["a", "c"], _ == _);
		this.assertEquals(result, 1);
	}

	test_similarity_emptyThis_emptyThat_isOne {
		var result = [].similarity([]);
		var expected = 1; // they're the same, even though they're empty

		this.assertEquals(result, expected);
	}

	test_similarity_same_isOne {
		var result = [0,1,2,3].similarity([0,1,2,3]);
		this.assertEquals(result, 1);
	}

	test_similarity_different_isZero {
		var result = [0,1,2,3].similarity([4,5,6,7]);
		this.assertEquals(result, 0);
	}

	// ----- mirror, mirror1, mirror2 --------------------------------------------

	test_mirror_onEmptyArray_sizeIsZero { this.assertEquals([].mirror.size, 0); }
	test_mirror1_onEmptyArray_sizeIsZero { this.assertEquals([].mirror1.size, 0); }
	test_mirror2_onEmptyArray_sizeIsZero { this.assertEquals([].mirror2.size, 0); }

	test_mirror_onEmptyArray_newArray {
		var input = [];
		this.assert(input.mirror !== input);
	}

	test_mirror1_onEmptyArray_newArray {
		var input = [];
		this.assert(input.mirror1 !== input);
	}

	test_mirror2_onEmptyArray_newArray {
		var input = [];
		this.assert(input.mirror2 !== input);
	}

	test_mirror_oneElementArray_result { this.assertEquals([1].mirror, [1]); }
	test_mirror1_oneElementArray_result { this.assertEquals([1].mirror1, [1]); }
	test_mirror2_oneElementArray_result { this.assertEquals([1].mirror2, [1, 1]); }

	test_mirror_oneElementArray_size { this.assertEquals([1].mirror.size, 1); }
	test_mirror1_oneElementArray_size { this.assertEquals([1].mirror1.size, 1); }
	test_mirror2_oneElementArray_size { this.assertEquals([1].mirror2.size, 2); }

	test_mirror_twoElementArray_result { this.assertEquals([1, 2].mirror, [1, 2, 1]); }
	test_mirror1_twoElementArray_result { this.assertEquals([1, 2].mirror1, [1, 2]); }
	test_mirror2_twoElementArray_result { this.assertEquals([1, 2].mirror2, [1, 2, 2, 1]); }

	test_mirror_twoElementArray_size { this.assertEquals([1, 2].mirror.size, 3); }
	test_mirror1_twoElementArray_size { this.assertEquals([1, 2].mirror1.size, 2); }
	test_mirror2_twoElementArray_size { this.assertEquals([1, 2].mirror2.size, 4); }

	test_is_rectangular_flat {
	    this.assert([1, 2, 3, 4].isRectangular, "isRectangular: flat arrays, [n1, n2,...], should always be rectangular");
    }
	test_is_rectangular_nested_flat {
	    this.assert([[1, 2, 3, 4]].isRectangular, "isRectangular: single nested flat array, [[n1, n2 ...]] should be rectangular");
    }
	test_is_rectangular_square {
	    this.assert([[1, 2], [3, 4]].isRectangular, "isRectangular: square matrix is rectangular");
    }
	test_is_rectangular_deep {
	    this.assert([[[[[1], [2]], [[3], [4]]]]].isRectangular, "isRectangular: should work at any depth");
    }
	test_is_rectangular_false_case {
	    this.assert([1, [2, 3]].isRectangular.not, "isRectangular: should reject different dimension sizes");
    }
	test_is_rectangular_with_raw_array {
	    this.assert([FloatArray[1,2], Signal[3,4]].isRectangular, "isRectangular: should work on RawArrays (true case)");
    }
	test_is_rectangular_with_raw_array_false_case {
	    this.assert([FloatArray[1,2], Signal[3,4,5]].isRectangular.not, "isRectangular: should work on RawArrays (false case)");
	}

} // End class

TestArrayLace : UnitTest {
	test_empty_returns_empty {
		var array, result;
		array = [];
		result = array.lace;
		this.assertEquals(result, [], "lace: empty array with no length argument should return empty array");
	}

	test_empty_with_length_argument_returns_empty {
		var array, result;
		array = [];
		result = array.lace(8);
		this.assertEquals(result, [], "lace: empty array with length argument should return empty array");
	}

	test_empty_subarray_returns_nil {
		var array, result;
		array = [[]];
		result = array.lace;
		this.assertEquals(result, [ nil ], "lace: empty subarray should return empty array");
	}

	test_flat_without_length_arg {
		var array, result;
		array = [1, 2, 3, 4, 5];
		result = array.lace;
		this.assertEquals(result, array, "lace: flat array should return a copy of itself");
	}

	test_flat_with_empty_subarray_substitutes_nil {
		var array, result;
		array = ["one", "two", [], "four", "five"];
		result = array.lace;
		this.assertEquals(result, ["one", "two", nil, "four", "five"], "lace: empty subarrays should be substituted for nil");
	}

	test_flat_with_length_arg_small {
		var array, result, n;
		n = 3;
		array = [1, 2, 3, 4, 5];
		result = array.lace(n);
		this.assertEquals(result, array.keep(n),
			"lace: flat array should return a copy of itself shortened to the length given in the argument");
	}

	test_flat_with_length_arg_large {
		var array, result, n;
		n = 18;
		array = [1, 2, 3, 4, 5];
		result = array.lace(n);
		this.assertEquals(result, array.wrapExtend(n),
			"lace: flat array should return itself extended to the length given in the argument");
	}

	test_subarrays_of_equal_length_without_length_arg {
		var array, result;
		array = [[1, 4, 7], [2, 5, 8], [3, 6, 9]];
		result = array.lace;
		this.assertEquals(result, [1, 2, 3, 4, 5, 6, 7, 8, 9],
			"lace: subarrays of equal length should lace correctly without length arugment");
	}

	test_subarrays_of_unequal_length_without_length_arg {
		var array, result;
		array = [[1, 4], [2, 5, 7], [3, 6, 8, 9]];
		result = array.lace;
		this.assertEquals(result, [1, 2, 3, 4, 5, 6],
			"lace: if not supplied, length should be computed using the minimum length subarray");
	}

	test_mixture_of_flat_and_arrays_without_length_arg {
		var array, result;
		array = [[10, 9], 8, [7, 6, 5], [4], [3, 2, 1, 0]];
		result = array.lace;
		this.assertEquals(result, [10, 8, 7, 4, 3],
			"lace: single-length items with no length argument should only be copied once");
	}

	test_list_with_sublists_without_length_arg {
		var list, result;
		list = List[List[1, 2, 3], List[4, 5, 6], List[7, 8]];
		result = list.lace;
		this.assertEquals(result, List[1, 4, 7, 2, 5, 8],
			"lace: lists with sublists and no length argument should compute length using minimum length sublist");
	}

	test_subarray_and_sublist_mixture_without_length_arg {
		var array, result;
		array = [["z", "y"], List[9, 8, 7], [\a, \b, \c]];
		result = array.lace;
		this.assertEquals(result, ["z", 9, \a, "y", 8, \b],
			"lace: arrays with mixture of sublists and subarrays should compute length using minimum length element");
	}

	test_subarrays_with_complete_length_arg {
		var array, result;
		array = [[1, 2, 3, 4], [5, 6, 7, 8], [-1, -2, -3, -4]];
		result = array.lace(12);
		this.assertEquals(result, [1, 5, -1, 2, 6, -2, 3, 7, -3, 4, 8, -4],
			"lace: complete length argument should return completely laced array");
	}

	test_subarrays_with_length_arg_shorter_than_number_of_subarrays {
		var array, result;
		array = [["a", "b"], ["c", "d"], ["e", "f"], ["g", "h"]];
		result = array.lace(3);
		this.assertEquals(result, ["a", "c", "e"],
			"lace: short length argument should return first members of first subarrays");
	}

	test_subarrays_with_length_arg_equal_to_number_of_subarrays {
		var array, result;
		array = [[\tok1, \tok2, \tok3], [\tok4], [\tok5, \tok6], [\tok7, \tok8, \tok9, \tok10]];
		result = array.lace(4);
		this.assertEquals(result, [\tok1, \tok4, \tok5, \tok7],
			"lace: equal length argument should return first members of all subarrays");
	}

	test_subarrays_with_length_arg_greater_than_number_of_subarrays {
		var array, result;
		array = [[1.0, -1.0, 2.0, -2.0, 3.0, -3.0], [4.0, -4.0, 5.0, -5.0]];
		result = array.lace(5);
		this.assertEquals(result, [1.0, 4.0, -1.0, -4.0, 2.0],
			"lace: should iterate through each subarray until length argument elements copied");
	}

	test_subarrays_with_length_arg_equal_to_total_number_of_elements {
		var array, result;
		array = [[1], [2, 3], [4, 5, 6], [7, 8, 9, 10], [11, 12, 13, 14, 15]];
		result = array.lace(15);
		this.assertEquals(result, [1, 2, 4, 7, 11, 1, 3, 5, 8, 12, 1, 2, 6, 9, 13],
			"lace: should advance through each array independently");
	}

	test_subarrays_with_length_arg_greater_than_total_number_of_elements {
		var array, result;
		array = [["one", "two", "three"], ["four", "five"], ["six"], "seven"];
		result = array.lace(12);
		this.assertEquals(result,
			["one", "four", "six", "seven", "two", "five", "six", "seven", "three", "four", "six", "seven"],
			"lace: should continue to fill from each array until reaching length argument");
	}

	test_subarrays_with_length_arg_equal_to_complete_lace {
		var array, result;
		array = [[3, 2, 1], 0, [5, 6], [10, 9, 8, 7], [12]];
		result = array.lace(20);
		this.assertEquals(result,
			[3, 0, 5, 10, 12, 2, 0, 6, 9, 12, 1, 0, 5, 8, 12, 3, 0, 6, 7, 12],
			"lace: should fill array with all interleaved values");
	}

	test_subarrays_with_length_arg_greater_than_complete_lace {
		var array, result;
		array = [[\a, \b], [\c, \d, \e], [\f]];
		result = array.lace(12);
		this.assertEquals(result, [\a, \c, \f, \b, \d, \f, \a, \e, \f, \b, \c, \f],
			"lace: should start over once array is completely laced");
	}

	test_sublists_work_like_arrays {
		var array, result;
		array = [List[1, 2, 3], List[4, 5, 6], List[7, 8, 9]];
		result = array.lace(9);
		this.assertEquals(result, [1, 4, 7, 2, 5, 8, 3, 6, 9],
			"lace: sublists should lace just like arrays do");
	}
} // End class

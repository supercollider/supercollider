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
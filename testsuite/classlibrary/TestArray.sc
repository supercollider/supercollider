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
			[121,     11],
			[100000, 100]
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
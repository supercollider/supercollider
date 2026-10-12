TestGeneratorExpr : UnitTest {

	test_from_docs {
		var z = {: [baker, cooper, fletcher, miller, smith],
			var floors = (1..5),
			baker <- floors,  baker != 5,  // Baker does not live on the top floor.
			// remove baker's floor from the list.
			// var creates a new scope, so the 'floors' on the left is a new binding.
			var floors = floors.removing(baker),
			cooper <- floors, cooper != 1, // Cooper does not live on the bottom floor.
			var floors = floors.removing(cooper), // remove cooper's floor from the list.
			fletcher <- floors, (fletcher != 5) && (fletcher != 1) // Fletcher does not live on either top or bottom floor.
				&& (absdif(fletcher, cooper) > 1), // Fletcher does not live on a floor adjacent to Cooper's.
			var floors = floors.removing(fletcher), // remove fletcher's floor
			miller <- floors, miller > cooper, // Miller lives on a higher floor than does Cooper.
			var floors = floors.removing(miller), // remove miller's floor
			smith <- floors, absdif(fletcher, smith) > 1  // Smith does not live on a floor adjacent to Fletcher's.
		};

		this.assertEquals(z.next, [3, 2, 4, 5, 1]);
	}

	test_two_args {
		var array = [10, 11, 12, 13, 14, 15];
		this.assertEquals(
			all {: [elem, count], elem count <- array},
			[[10, 0], [11, 1], [12, 2], [13, 3], [14, 4], [15, 5]]
		);
	}

	test_side_effect_and_exit {
		var log = [];
		var result = all {: z, x <- (1..20), var z = (x*x-x) div: 2,  :: log = log.add([x,z]), :while z < 50 };
		this.assertEquals(
			result,
			[0, 1, 3, 6, 10, 15, 21, 28, 36, 45]
		);

		this.assertEquals(
			log,
			[[1, 0], [2, 1], [3, 3], [4, 6], [5, 10], [6, 15], [7, 21], [8, 28], [9, 36], [10, 45], [11, 55]]
		);
	}
}

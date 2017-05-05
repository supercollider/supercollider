TestClock : UnitTest {


	test_nextTimeOnGrid {

		try {
			1.nextTimeOnGrid(1)
		} { |err|
			this.assert(err.isKindOf(DoesNotUnderstandError), "1.nextTimeOnGrid(1) should throw an error");
		};

		[2, 5, 8, 100, 1000].every { |b|
			var t = TempoClock.new(1, b);
			this.assertEquals(1.nextTimeOnGrid(t) - b, 0, "nextTimeOnGrid should be zero after an integer number of beats and tempo 1" );
			t.stop;
		};

		[2, 5, 8, 100, 1000].every { |b|
			var t = TempoClock.new(1, b);
			t.sched(0.05524, {
				this.assertEquals(1.nextTimeOnGrid(t) - b, 1);
				t.stop;
				nil
			});

		}


	}


}
TestStream : UnitTest {

	test_funcStream {
		var str, envir;

		envir = Environment.new;
		envir.use {
			~count = 0;
			str = FuncStream({ ~count = ~count + 1 }, { ~count = 0 });
		};

		this.assert(str.nextN(8).postln == (1..8), "FuncStream should call the nextFunc in original environment");
		str.reset;
		this.assert(str.nextN(8).postln == (1..8), "FuncStream should call the resetFunc in original environment");
		envir.use { ~count = 0 };
		this.assert(str.nextN(8).postln == (1..8), "FuncStream should call the nextFunc in original environment");


	}

	test_funcStream1 {
		var str, envir, arr;

		envir = Environment.new;
		envir.use {
			~count = 0;
			str = FuncStream1({ ~count = ~count + 1 }, { ~count = 0 }, 5);
		};

		arr =  (1..5) ++ (nil ! 3);

		this.assert(str.nextN(8).postln == arr, "FuncStream1 should call the nextFunc in original environment and be limited to repeats items");
		str.reset;
		this.assert(str.nextN(8).postln == arr, "FuncStream1 should call the resetFunc in original environment and be limited to repeats items");
		envir.use { ~count = 0 };
		this.assert(str.nextN(8).postln == (nil ! 8), "FuncStream1 should be limited to repeats items");

	}



}

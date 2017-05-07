TestStream : UnitTest {

	test_funcStream {
		var str, envir;

		envir = Environment.new;
		envir.use {
			~count = 0;
			str = FuncStream({ ~count = ~count + 1 }, { ~count = 0 });
		};

		this.assertEquals(str.nextN(8), (1..8), "FuncStream should call the nextFunc in original environment");
		str.reset;
		this.assertEquals(str.nextN(8), (1..8), "FuncStream should call the resetFunc in original environment");
		envir.use { ~count = 0 };
		this.assertEquals(str.nextN(8), (1..8), "FuncStream should call the nextFunc in original environment");


	}


}

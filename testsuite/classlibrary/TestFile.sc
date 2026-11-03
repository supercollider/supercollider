TestFile : UnitTest {
	test_readWrite {
		var path = PathName.tmp +/+ "rest_readWrite.bin";
		var putData = [
			[ $c, \putChar, \getChar ],
			[ 99, \putInt8, \getInt8 ],
			[ 12568, \putInt16, \getInt16 ],
			[ 0xCD518461, \putInt32, \getInt32 ],
			[ 1024.125, \putFloat, \getFloat ],
			[ pi, \putDouble, \getDouble ],
			[ "this is a test", \putPascalString, \getPascalString ]
		];
		var writeData = [
			$c, // Char
			0xCD518461, // Integer
			pi, // Float
			\thisIsASymbol, // Symbol
			"this is a test", // String
			Int16Array.newFrom((-10..10)),
			Int32Array.newFrom((-10..10)),
			FloatArray.newFrom((-10..10).collect(_ * pi)),
			DoubleArray.newFrom((-10..10).collect(_ * pi))
		];

		File.use(path, "w") { |file|
			putData.do { |item|
				var elem = item[0];
				var sel = item[1];
				file.perform(sel, elem);
			};

			writeData.do { |array| file.write(array) };
		};

		File.use(path, "r") { |file|
			putData.do { |item|
				var elem = item[0];
				var sel = item[2];
				var result = file.perform(sel, elem);
				this.assertEquals(result, elem);
			};

			writeData.do { |elem|
				var buffer;
				switch (elem.class)
				{ Char } { this.assertEquals(file.getChar, elem) }
				{ Integer } { this.assertEquals(file.getInt32, elem) }
				{ Float } { this.assertEquals(file.getDouble, elem) }
				{ Symbol } {
					var buffer = String.newClear(elem.asString.size);
					file.read(buffer);
					this.assertEquals(buffer.asSymbol, elem);
				}
				{
					// raw array (including String)
					var buffer = elem.class.newClear(elem.size);
					file.read(buffer);
					this.assertEquals(buffer, elem);
				}
			};
		};

		File.delete(path);
	}

	test_readWriteLE {
		var path = PathName.tmp +/+ "rest_readWrite.bin";
		var putData = [
			[ $c, \putChar, \getChar ],
			[ 99, \putInt8, \getInt8 ],
			[ 12568, \putInt16LE, \getInt16LE ],
			[ 0xCD518461, \putInt32LE, \getInt32LE ],
			[ 1024.125, \putFloatLE, \getFloatLE ],
			[ pi, \putDoubleLE, \getDoubleLE ],
			[ "this is a test", \putPascalString, \getPascalString ]
		];
		var writeData = [
			$c, // Char
			0xCD518461, // Integer
			pi, // Float
			\thisIsASymbol, // Symbol
			"this is a test", // String
			Int16Array.newFrom((-10..10)),
			Int32Array.newFrom((-10..10)),
			FloatArray.newFrom((-10..10).collect(_ * pi)),
			DoubleArray.newFrom((-10..10).collect(_ * pi))
		];

		File.use(path, "w") { |file|
			putData.do { |item|
				var elem = item[0];
				var sel = item[1];
				file.perform(sel, elem);
			};

			writeData.do { |array| file.writeLE(array) };
		};

		File.use(path, "r") { |file|
			putData.do { |item|
				var elem = item[0];
				var sel = item[2];
				var result = file.perform(sel, elem);
				this.assertEquals(result, elem);
			};

			writeData.do { |elem|
				var buffer;
				switch (elem.class)
				{ Char } { this.assertEquals(file.getChar, elem) }
				{ Integer } { this.assertEquals(file.getInt32LE, elem) }
				{ Float } { this.assertEquals(file.getDoubleLE, elem) }
				{ Symbol } {
					var buffer = String.newClear(elem.asString.size);
					file.readLE(buffer);
					this.assertEquals(buffer.asSymbol, elem);
				}
				{
					// raw array (including String)
					var buffer = elem.class.newClear(elem.size);
					file.readLE(buffer);
					this.assertEquals(buffer, elem);
				}
			};
		};

		File.delete(path);
	}
}

TestCompilerOptimisations : UnitTest {
	test_extended_bytecodes {
		this.assertEquals(Integer.findMethod(\do).code, Int8Array[-113, 0, -113, 1], "Int:do");
		this.assertEquals(Integer.findMethod(\reverseDo).code, Int8Array[-113, 2, -113, 3, -113, 4], "Int:reverseDo");
		this.assertEquals(Integer.findMethod(\for).code, Int8Array[-113, 5, -113, 6, -113, 16], "Int:for");
		this.assertEquals(Integer.findMethod(\forBy).code, Int8Array[-113, 7, -113, 8, -113, 9], "Int:forBy");
		this.assertEquals(ArrayedCollection.findMethod(\do).code, Int8Array[-113, 10, -113, 1], "ArrayedCollection:do");
		this.assertEquals(ArrayedCollection.findMethod(\reverseDo).code, Int8Array[-113, 11, -113, 12, -113, 4] , "ArrayedCollection:reverseDo");
		this.assertEquals(Dictionary.findMethod(\keysValuesArrayDo).code, Int8Array[-113, 13, -113, 14] , "Dictionary:keysValuesArrayDo");
		this.assertEquals(Number.findMethod(\forSeries).code, Int8Array[-113, 29, -113, 30, -113, 31] , "Number:forSeries");
		this.assertEquals(Float.findMethod(\do).code, Int8Array[-113, 17, -113, 18] , "Float:do");
		this.assertEquals(Float.findMethod(\reverseDo).code, Int8Array[-113, 19, -113, 20, -113, 21] , "Float:reverseDo");
	}
}

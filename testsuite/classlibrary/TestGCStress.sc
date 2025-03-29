TestGCStress : UnitTest {
	test_large_array {
		var b = Pwhite().asStream;
		Array.fill(1810000, { b.next });
		this.assert(true, "This test segfaults if it fails.", false);
	}
}

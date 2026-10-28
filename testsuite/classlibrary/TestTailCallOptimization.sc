TestTailCallOptimization : UnitTest {
	test_compiler_generates_tail_calls {
		this.assert(Process.tailCallOptimize, "the compiler should be generating tail calls");
	}

	// Without a working tail call in the interpreter this exhausts the stack.
	test_deep_tail_recursion_does_not_overflow {
		var f, result;
		f = { |n, acc| if(n == 0) { acc } { f.(n - 1, acc + 1) } };
		result = try { f.(100000, 0) };
		this.assertEquals(result, 100000, "tail recursion 100000 deep should not overflow the stack");
	}
}

TestNode : UnitTest {

	var node;

	setUp {
		node = Node.basicNew
	}

	tearDown {
		node.free
	}

	test_release_nil {
		var msg = node.releaseMsg(nil);
		this.assertEquals(msg[3], 0, "nil should set gate to 0");
	}

	test_release_zero {
		var msg = node.releaseMsg(0);
		this.assertEquals(msg[3], -1, "0 should set gate to -1");
	}

	test_release_posValue {
		var msg = node.releaseMsg(2);
		this.assertEquals(msg[3], -3, "A positive value (N) should set gate to -(N + 1)");
	}

	test_release_negValue {
		var msg = node.releaseMsg(-2);
		this.assertEquals(msg[3], -1, "A negative value should set gate to -1");
	}

}

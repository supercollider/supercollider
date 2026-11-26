TestServerOptions : UnitTest {
	test_deviceSetter {
		var o = ServerOptions();

		o.device = "foo";
		this.assertEquals(o.inDevice, "foo");
		this.assertEquals(o.outDevice, "foo");

		// Symbols should be converted to Strings
		o.device = \baz;
		this.assertEquals(o.inDevice, "baz");
		this.assertEquals(o.outDevice, "baz");

		o.device = nil;
		this.assertEquals(o.inDevice, nil);
		this.assertEquals(o.outDevice, nil);

		o.device = ["foo", "bar"];
		this.assertEquals(o.inDevice, "foo");
		this.assertEquals(o.outDevice, "bar");

		// Symbols should be converted to Strings
		o.device = [\cat, \dog];
		this.assertEquals(o.inDevice, "cat");
		this.assertEquals(o.outDevice, "dog");

		// elements should wrap around
		o.device = ["baz"];
		this.assertEquals(o.inDevice, "baz");
		this.assertEquals(o.outDevice, "baz");

		o.device = [nil, nil];
		this.assertEquals(o.inDevice, nil);
		this.assertEquals(o.outDevice, nil)
	}

	test_deviceGetter {
		var o = ServerOptions(), dev;

		// same device -> String resp. nil
		o.inDevice = "foo";
		o.outDevice = "foo";
		this.assertEquals(o.device, "foo");

		o.inDevice = nil;
		o.outDevice = nil;
		this.assertEquals(o.device, nil);

		// different devices -> Array
		o.inDevice = "foo";
		o.outDevice = "bar";
		dev = o.device;
		this.assert(dev.isKindOf(Array));
		this.assertEquals(dev[0], "foo");
		this.assertEquals(dev[1], "bar");

		o.inDevice = "baz";
		o.outDevice = nil;
		dev = o.device;
		this.assert(dev.isKindOf(Array));
		this.assertEquals(dev[0], "baz");
		this.assertEquals(dev[1], nil);

		o.inDevice = nil;
		o.outDevice = "baz";
		dev = o.device;
		this.assert(dev.isKindOf(Array));
		this.assertEquals(dev[0], nil);
		this.assertEquals(dev[1], "baz")
	}
}

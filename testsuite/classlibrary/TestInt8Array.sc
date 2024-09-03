TestInt8Array : UnitTest {
	test_asOSC_convertMsg {
		var msg = [ '/foo', 1, 1.5, 2, 'bar' ];
		var raw = msg.asRawOSC;

		var msg2 = raw.asOSC;

		this.assertEquals(msg2, msg);
	}


	test_asOSC_convertMsgWithBlobArgument {
		var msg = ['/foo', Int8Array[1, 2, 3]];
		var raw = msg.asRawOSC;

		var msg2 = raw.asOSC;

		this.assertEquals(msg2, msg);
	}

	test_asOSC_convertMsgWithSpecialArguments {
		var msg = ['/foo', true, false, nil];
		// NB: we can't do msg.asRawOSC because it converts true and false to 1 resp. 0
		var raw = Int8Array[47, 102, 111, 111, 0, 0, 0, 0, 44, 84, 70, 78, 0, 0, 0, 0];

		var msg2 = raw.asOSC;

		this.assertEquals(msg2, msg);
	}

	test_asOSC_convertMsgWithEmbeddedArray {
		var msg = ['/foo'] ++ [1, 2, 3].asOSCArgEmbeddedArray;
		var raw = msg.asRawOSC;

		var msg2 = raw.asOSC;

		this.assertEquals(msg2, msg);
	}

	test_asOSC_convertEmptyMsg {
		// strip typetags
		var msg = [ '/foo' ];

		var raw1 = msg.asRawOSC;
		var raw2 = raw1[0..(raw1.size - 5)]; // without typetags
		var raw3 = raw2 ++ Int8Array[40, 0, 0, 0]; // not a typetag string

		var msg1 = raw1.asOSC;
		var msg2 = raw2.asOSC;
		var msg3 = raw3.asOSC; // error message, but no exception!

		this.assertEquals(msg1, msg);
		this.assertEquals(msg2, msg);
		this.assertEquals(msg3, msg);
	}

	test_asOSC_convertImmediateBundle {
		var bundle = [ nil, [ '/foo', 1, 1.5, 2, 'bar' ] ];
		var raw = bundle.asRawOSC;

		var bundle2 = raw.asOSC;

		this.assertEquals(bundle2, bundle);
	}

	test_asOSC_convertEmptyImmediateBundle {
		var bundle = [nil];
		var raw = bundle.asRawOSC;

		var bundle2 = raw.asOSC;

		this.assertEquals(bundle2, bundle);
	}

	test_asOSC_convertBundleWithTimetag {
		var timeTags = #[0.0, 10.5];
		timeTags.do { |t|
			var bundle = [ t, [ '/foo', 1, 1.5, 2 ] ];
			var raw = bundle.asRawOSC;

			var bundle2 = raw.asOSC;

			// we cannot directly compare the bundles because the timetags
			// are converted to OSC time and back. Instead check if
			// bundle2 is equivalent to bundle.
			this.assertEquals(bundle2.size, bundle.size); // same size
			this.assertFloatEquals(bundle2[0], bundle[0]); // same timetag
			this.assertEquals(bundle2[1], bundle[1]); // same message
		}
	}

	test_asOSC_convertEmptyBundleWithTimetag {
		var bundle = [5.0];
		var raw = bundle.asRawOSC;

		var bundle2 = raw.asOSC;

		// see test_asOSC_convertBundleWithTimetag
		this.assertEquals(bundle2.size, bundle.size);
		this.assertFloatEquals(bundle2[0], bundle[0]);
	}

	test_asOSC_convertBundleWithMultipleElements {
		var bundle = [ nil, [ '/foo', 1, 1.5, 2 ], [ '/bar', '/baz' ] ];
		var raw = bundle.asRawOSC;

		var bundle2 = raw.asOSC;

		this.assertEquals(bundle2, bundle);
	}

	test_asOSC_throwOnBadMsg {
		this.assertException({
			// empty data
			Int8Array[].asOSC
		}, PrimitiveFailedError);

		this.assertException({
			// not an OSC message or bundle
			Int8Array[ 98, 111, 103, 117, 115 ].asOSC
		}, PrimitiveFailedError);
	}

	test_asOSC_throwOnBadMsgSize {
		this.assertException({
			// message size is not a multiple of 4
			Int8Array[47, 102, 111, 111, 0, 0, 0, 0, 44].asOSC;
		}, PrimitiveFailedError);
	}

	test_asOSC_throwOnBadBundleSize {
		this.assertException({
			// bundle size is not a multiple of 4
			Int8Array[35, 98, 117, 110, 100, 108, 101, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0].asOSC;
		}, PrimitiveFailedError);

		this.assertException({
			// bundle size must be at least 16 bytes (#bundle + timetag)
			Int8Array[35, 98, 117, 110, 100, 108, 101, 0].asOSC
		}, PrimitiveFailedError);


	}

	test_asOSC_throwOnBadBundleElemSize {
		this.assertException({
			// bundle element size is zero
			Int8Array[35, 98, 117, 110, 100, 108, 101, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0].asOSC;
		}, PrimitiveFailedError);

		this.assertException({
			// bundle element size is not a multiple of 4
			Int8Array[35, 98, 117, 110, 100, 108, 101, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 27].asOSC;
		}, PrimitiveFailedError);
	}
}

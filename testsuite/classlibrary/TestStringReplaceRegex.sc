TestStringReplaceRegex : UnitTest {
	test_replace_simple {
		this.assertEquals(
			"foo, foo, bar, foo".replaceRegexp("foo", "car"),
			"car, car, bar, car"
		);
	}
	test_replace_empty {
		this.assertEquals(
			"".replaceRegexp("foo", "car"),
			""
		);
	}
	test_replace_more_complex {
		this.assertEquals(
			"texttextte9xtte823x234t".replaceRegexp("[0-9]", ""),
			"texttexttexttext"
		);
		this.assertEquals(
			"hello--Remove--rem0ve".replaceRegexp("(r|R)em(o|0)ve", ""),
			"hello----"
		);
	}
	test_replace_captures {
		this.assertEquals(
			"HelLO WoRlD".replaceRegexp("(\\\w)", "\\\L$1"),
			"hello world"
		)
	}
	test_no_replace {
        this.assertEquals("hello".replaceRegexp("w", ""), "hello");
    }
    test_wrong_args {
        this.assertException({"hello".replaceRegexp(nil, "foo")}, PrimitiveFailedError);
    }
    test_invalid_regex {
        this.assertException({"hello".replaceRegexp("\\a[\o", "foo").postln}, PrimitiveFailedError);
    }
}

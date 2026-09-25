TestAbstractObjectExperimental : UnitTest {
	// Note that some of these tests cannot be written as AbstractObjectExperimental isn't a real object, and therefore doesn't work in many common sc contexts, such as, print to the interpreter, or using it is a 'try' block.

	test_dont_crash {
		// Can't wrap this in a this.assertNoException as AbstractObjectExperimental doesn't support isException.
		AbstractObjectExperimental();
	}

	test_object_superclass {
		this.assertEquals(Object.superclass, AbstractObjectExperimental);
	}
	test_abstract_object_superclass {
		this.assertEquals(AbstractObjectExperimental.superclass, nil)
	}
	test_abstract_object_class {
		this.assertEquals(AbstractObjectExperimental.class, Meta_AbstractObjectExperimental)
	}
	test_meta_abstract_object_class {
		this.assertEquals(Meta_AbstractObjectExperimental.class, Class)
	}

	test_identity {
		var o = AbstractObjectExperimental();
		var i = AbstractObjectExperimental();

		this.assert(o === o);
		this.assert(o !== i);

		// This method cannot be written because isException isn't implemented.
		// this.assertException({ o == i }, DoesNotUnderstandError, "AbstractObjectExperimental should not support equality by itself.");
	}

	test_copy {
		var a = AbstractObjectExperimental();
		this.assert(a.identityHash != AbstractObjectExperimental().identityHash, "AbstractObjectExperimentals should have unique hashes, technically this could be a false negative, but seems unlikely.");
	}
}

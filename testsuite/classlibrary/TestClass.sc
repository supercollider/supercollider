TestClass : UnitTest {

	test_respondingMethods_no_duplicates {
		var methods = Array.respondingMethods;
		var selectors = methods.collect(_.name);
		var selectorsWithoutDuplicates = selectors.as(IdentitySet);
		var hasDuplicates = selectors.size > selectorsWithoutDuplicates.size;
		this.assert(hasDuplicates.not, "The response from respondingMethods should contain only one method of each selector");
	}

	test_findRespondingSubclasses_from_Object {
		var classesFound = Object.findRespondingSubclasses(\dummyMethodForTest);
		this.assertEquals(classesFound, [this.class] ++ TestClassDummy, "Object findRespondingSubclasses should find correct class")
	}

	test_findRespondingUpperSubclasses {
		var classesFound = this.class.findRespondingUpperSubclasses(\dummyMethodForTest);
		this.assertEquals(classesFound, [this.class], "Class findRespondingUpperSubclasses should find correct class")
	}

	test_findRespondingSubclasses {
		var classesFound = this.class.findRespondingSubclasses(\dummyMethodForTest);
		this.assertEquals(classesFound, [TestClassDummy], "Class findRespondingSubclasses should find correct class")
	}

	dummyMethodForTest {}


}

TestClassDummy : TestClass {}

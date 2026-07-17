TestExtension_Base {
	*foo { ^\foo }
	*bar { ^\bar }
}

TestExtension_Derived : TestExtension_Base {
	*foo { ^\fooDerived }
}

TestExtension : UnitTest {
	test_extensions_basic {
		this.assertEquals(TestExtension_Base.meow, \meow, "basic extension works.");
		this.assertEquals(TestExtension_Base.foo, \foo2, "basic extension override works.");
	}
	test_extensions_inheritance {
		this.assertEquals(TestExtension_Derived.foo, \fooDerived, "extension override to a base class should be overridden by the derived class.");
		this.assertEquals(TestExtension_Derived.meow, \meow, "inheriting from an extension shows methods.");
	}
}

+TestExtension_Base {
	*foo { ^\foo2 }
	*meow { ^\meow }
}

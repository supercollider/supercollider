TestFunctionDeduceName : UnitTest {
	test_vardef {
		var foo = {};
		var boo = _ + 1;
		this.assertEquals(foo.def.name, 'foo'); 
		this.assertEquals(boo.def.name, 'boo'); 
	}

	test_assign {
		var foo, boo;
		foo = {};
		boo = _ * 2;
		this.assertEquals(foo.def.name, 'foo');
		this.assertEquals(boo.def.name, 'boo');
	}

	test_memberSet {
		var ev = ();
		ev.foo = {};
		ev.boo = {};
		this.assertEquals(ev[\foo].def.name, 'foo');
		this.assertEquals(ev[\boo].def.name, 'boo');
	}

	test_environment {
		~foo = {};
		~boo = _.meow;
		this.assertEquals(~foo.def.name, 'foo');
		this.assertEquals(~boo.def.name, 'boo');
	}

}

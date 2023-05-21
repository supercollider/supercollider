
Dln {
	*one {
		arg foo;
		"DebugMyTrace".throw
	}

	*two {
		arg foo;
		this.one(foo)
	}

}


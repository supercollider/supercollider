
PrettyPrintStream : CollStream { // well prettier anyway

	var tabs=0;

	nl {
		super.nl;
		tabs.do({		this.tab; });
	}

	storeArgs { arg  args,itemsPerLine=1;
		this << "(";
		tabs = tabs + 1;
		this.nl;
		args.storeItemsOn(this,itemsPerLine);
		tabs = tabs - 1;
		this.nl;
		this << ")";
	}
	indent { arg way=1; tabs = tabs + way; }

}
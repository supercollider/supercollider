f = {
	// A control message's functions on their own lines are one level more, if each is on one line.
	if(scroll)
	{ view = ScrollTopView.new(this, name, bounds) }
	{ view = TopView.new(this, name, bounds) };
	x.do
	{ |i| i.postln };
	// With a multi-line one, as written.
	if(x)
	{ 1 }
	{
		2
	};
	if(x) { 1 } { 2 };
};

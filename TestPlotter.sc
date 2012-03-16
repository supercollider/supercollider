
TestPlotter : ViewTester {
	
	test_suppliedBounds {
		var b,p;
		b = Rect(100, 100, 200, 200);
		p = Plotter("name",
		  b,
		  parent: w);
		p.interactionView.background = Color.black;
		p.setValue([0,1,0,1]);

		this.assertEquals(p.interactionView.bounds,b,"interactionView should be placed on the supplied bounds")
	}
}




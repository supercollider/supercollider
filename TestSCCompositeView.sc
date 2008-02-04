
TestSCCompositeView : UnitTest {
	
	
	test_nestingRelVsAbs {
		var vs,trueBounds,falseBounds;
		
		vs = [true,false].collect({ |relativeOrigin|
			var ratio = 1.25, x = 0, lastx = 0, lastView,views,w;
			views = [];
			
			w = SCWindow("Albers ad infinitum", Rect(10, 10, 600, 600));
		
			lastView = w;
			while { x < 299 } {
				lastView = SCCompositeView(lastView,
					Rect(x - lastx, (x - lastx) * ratio, 600 - (2*x), 600 - (2*x)))
					.background_(Color.rand.alpha_(rrand(0.3, 0.7)))
					.relativeOrigin_(relativeOrigin);
				views = views.add(lastView);
				lastx = x;
				x = x + 12;
			};
			w.front;
			views
		});
		
		// bounds of each vs array should be equal
		trueBounds = vs[0].collect({ |v| v.bounds });
		falseBounds = vs[1].collect({ |v| v.bounds });
		
		this.assertEquals( trueBounds, falseBounds, "regardless of relativeOrigin, bounds of nestsed composite views should be equal");
		
		
	}
	
	
	
}
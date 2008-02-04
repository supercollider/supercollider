
TestSCScrollView : UnitTest {
	
	test_nestingRelVsAbs {

		var vs,trueBounds,falseBounds;
		
		vs = [true,false].collect({ |relativeOrigin|
			var ratio = 1.25, x = 0, lastx = 0, lastView,views,w,b;
			views = [];
			
			w = SCWindow("Albers ad infinitum", Rect(10, 10, 600, 600));
		
			lastView = w;
			while { x < 299 } {
				b = relativeOrigin.if( 
						{Rect(x - lastx, (x - lastx) * ratio, 600 - (2*x), 600 - (2*x))},
						{Rect(x, x * ratio, 600 - (2*x), 600 - (2*x))});

				lastView = SCScrollView(lastView,b)
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
		trueBounds = vs[0].collect({ |v| v.absoluteBounds });
		falseBounds = vs[1].collect({ |v| v.absoluteBounds });
		
		this.assertEquals( trueBounds, falseBounds, "regardless of relativeOrigin, bounds of nested scroll views should be equal");

	}


}
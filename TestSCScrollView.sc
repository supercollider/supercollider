
TestSCScrollView : UnitTest {
	
	test_nestingRelVsAbs {
		// Albers ad infinitum
		// but this should look centered, just the one in composite view
		var vs,trueBounds,falseBounds;
		
		vs = [true,false].collect({ |relativeOrigin|
			var ratio = 1.25, x = 0, lastx = 0, lastView,views,w,b;
			var title;
			views = [];
			title = relativeOrigin.if("Relative","Absolute") + "origin should be identical" + TestSCScrollView + "test_nestingRelVsAbs";
			w = SCWindow(title, Rect(10 + relativeOrigin.if(600,0), 10, 600, 600));
		
			lastView = w;
			while { x < 299 } {
				b = relativeOrigin.if( 
						{Rect(x - lastx, (x - lastx) * ratio, 600 - (2*x), 600 - (2*x))},
						{Rect(x, x * ratio, 600 - (2*x), 600 - (2*x))});

				lastView = SCScrollView(lastView,b)
					.background_(Color(x/299,x/299,x/299))
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
		
		// says it right, looks wrong though
		this.assertEquals( trueBounds, falseBounds, "regardless of relativeOrigin, bounds of nested scroll views should be equal");

	}

	// scroll in the center of a scroll
	test_absoluteBounds  {
		
		var w,c,d,s,t,bounds,absoluteBounds;
		w = SCWindow.new("TestSCCompositeView:test_absoluteBounds white comp should be in the middle of blue comp");

		c = GUI.scrollView.new(w,Rect(30,30,300,300));
		c.background = Color.blue;
	
		t = GUI.scrollView.new(c,Rect(100,100,100,100));
		t.background = Color.white;
	
		w.front;
		
		// absoluteBounds gotten directly from the view's internals
		absoluteBounds = t.absoluteBounds;

		//it LOOKS correct but the absoluteBounds returns wrong
		// I think that if you run this separately (with a pause after w.front) then the bounds return correct
		this.assertEquals(absoluteBounds , Rect(130,130,100,100),"bounds should be 130,130,100,100 for scrollView");
	}

	test_scrollInsideRelativeComposite {
		var w,c,d,s,t;
		w = SCWindow.new;


		c = SCCompositeView(w,Rect(30,30,300,300));
		c.relativeOrigin = true;
		c.background = Color.blue;


		d = SCStaticText(c,Rect(0,200,300,40));
		d.background = Color.white;
		d.string = "the yellow scroll should be in the top left corner of the blue compostive view";

		// although its parent is relativeOrigin, the scroll view does not position itself
		s = SCScrollView(c,Rect(0,0,100,100));
		s.background = Color.yellow;
		s.hasBorder = true;

		// the yellow background only is drawn if there is contents
		// in other words is checkMinimumSize is not 0,0,0,0
		// inside the scroll, this is clear so you can see the yellow
		t = SCStaticText(s,Rect(0,0,80,80));
		t.background = Color.green(alpha: 0.3);
		t.string = "text inside the yellow scroll";


		w.front;
	}
}

TestSCCompositeView : UnitTest {
	
	test_absoluteBounds  { // just put a comp inside a relative comp and check absoluteBounds
		
		var w,c,d,s,t,bounds,absoluteBounds;
		w = SCWindow.new("TestSCCompositeView:test_absoluteBounds white comp should be in the middle of blue comp");


		c = SCCompositeView(w,Rect(30,30,300,300));
		c.relativeOrigin = true;
		c.background = Color.blue;
	
		t = GUI.compositeView.new(c,Rect(100,100,100,100));
		t.relativeOrigin = false;
		t.background = Color.white;
	
		w.front;
		
		// absoluteBounds gotten directly from the view's internals
		absoluteBounds = t.absoluteBounds;
	
		// should be the same as this:
		bounds = t.bounds;
		t.getParents.detect({ |parent|
			(parent.tryPerform(\relativeOrigin) == true).if({
				bounds = bounds.moveBy(parent.bounds.left, parent.bounds.top);
				false
			}, {
				true
			});
		});
		this.assertEquals( absoluteBounds , bounds,".absoluteBounds from the view should be same as the logical calculated bounds for compositeView");
		this.assertEquals(absoluteBounds , Rect(130,130,100,100),"bounds should be 130,130,100,100 for compositeView");
		
	}
	
	test_allViewsAbsoluteBounds  {
		
		var w,c,d,s,t,bounds,absoluteBounds;
		w = SCWindow.new("TestSCCompositeView:test_allViewsAbsoluteBounds");

		c = SCCompositeView(w,Rect(30,30,300,300));
		c.relativeOrigin = true;
		c.background = Color.blue;
	
		[\compositeView,\hLayoutView,\vLayoutView,\slider,\rangeSlider,\slider2D,\tabletSlider2D,
		\button,\popUpMenu,\staticText,\listView,\dragSource,\dragSink,\dragBoth,\numberBox,\textField,\userView,
		\multiSliderView,\envelopeView,\tabletView,\soundFileView,\movieView,\textView,\quartzComposerView,
		\scrollView].do({ |classSelector|
				//\ezSlider,\ezNumber takes a point

			t = GUI.perform(classSelector).new(c,Rect(100,100,100,100));
			t.background = Color.white;
		
			w.front;
			
			// absoluteBounds gotten directly from the view's internals
			absoluteBounds = t.absoluteBounds;
		
			// should be the same as this:
			bounds = t.bounds;
			t.getParents.detect({ |parent|
				(parent.tryPerform(\relativeOrigin) == true).if({
					bounds = bounds.moveBy(parent.bounds.left, parent.bounds.top);
					false
				}, {
					true
				});
			});
			this.assert( absoluteBounds == bounds,"calculated bounds should be the same as .absoluteBounds for " + classSelector);
			this.assert(absoluteBounds == Rect(130,130,100,100),"bounds should be 130,130,100,100 for " + classSelector);
		})
	}
	test_nestingRelVsAbs {
		// taken from some code by JH
		// Albers ad infinitum
		var vs,trueBounds,falseBounds,w;
		
		vs = [true,false].collect({ |relativeOrigin|
			var ratio = 1.25, x = 0, lastx = 0, lastView,views,w,b,title;
			views = [];
			
			title = relativeOrigin.if("Relative","Absolute") + "origin should be identical" + TestSCCompositeView + "test_nestingRelVsAbs";
			w = SCWindow(title, Rect(10 + relativeOrigin.if(600,0), 10, 600, 600));
		
			lastView = w;
			while { x < 299 } {
				b = relativeOrigin.if( 
						{Rect(x - lastx, (x - lastx) * ratio, 600 - (2*x), 600 - (2*x))},
						{Rect(x, x * ratio, 600 - (2*x), 600 - (2*x))});

				lastView = SCCompositeView(lastView,b)
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
		
		this.assertEquals( trueBounds, falseBounds, "regardless of relativeOrigin, bounds of nested composite views should be equal");
		
	}
	
	test_disappearingContents {
		
		// last one on the right disappers
		// the problem is mouseTrack -> setValue -> refresh -> 
		//    [view setNeedsDisplayInRect: SCtoNSRect(inRect)];

		var f,sliders,last;
		f = MultiPageLayout.new("last bottom right slider should not disapear when touched");
	
		sliders=Array.fill(rrand(16,32),{ arg i;
			GUI.slider.new( f, 10@150 );
		});

		f.flow({ arg subf;
			GUI.slider.new( subf, 30@30 );
			GUI.slider.new( subf, 30@30 );
			GUI.slider.new( subf, 30@30 );
			last = GUI.slider.new( subf, 30@30 );
		}, 50@100 );
		"TestSCCompositeView-test_disappearingContents".gui(f);
		f.resizeToFit;
		f.front;
	}

	
	
}
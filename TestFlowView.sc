
TestFlowView : ViewTester {
	
	test_startRow {
		var f,g,a,b,c;

		f = FlowView.new(w);
		

		a = Slider(f,Rect(0,0,100,100));
		b = Slider(f,Rect(0,0,100,100));
		f.startRow;
		c = Slider(f,Rect(0,0,100,100));

	
		this.assert( a.bounds.left < b.bounds.left,"b should not be on a new row, it should be to the right of a");
		this.assert( a.bounds.left == c.bounds.left,"a and c should be aligned on the left edge");
		
	}
	
	test_removeAll {
		var f,g,k;

		//g.front;
		f = FlowView.new(w);

		Button(f,Rect(0,0,100,17)).states_([["a"]]);

		Slider(f,Rect(0,0,100,100));

		// this is the problem: the start rows don't get removed
		f.startRow;

		Button(f,Rect(0,0,100,17)).states_([["next line"]]);

		//
		k = f.children;
		f.removeAll;
		
		this.assertEquals( f.children.size, 0 , "after removeAll there should be 0 children");
		k.do({ |oldchild|
			this.assert( oldchild.isClosed,"children views should all now be isClosed : " + oldchild )
		});
	}
	
	test_flow { 
		// this is a minor change in  behavior that would affect the swing method too
		var comp,innerFlow;
		w.flow({ arg f;
			f.comp({ |comp|
				innerFlow = comp.flow({ arg layout;
					//ActionButton(layout,"yo");
				},Rect(100,100,100,100))
				.background = Color.blue;
		
			},Rect(0,0,500,500))
			.background_(Color.red)
		});

		this.assertEquals( innerFlow.absoluteBounds.moveTo(0,0),Rect(0,0, 100, 100),
					" flow should not resizeToFit if the bounds passed to it were explicit");
	}
	
	test_indentedRemaining {
		var b,p,f;
		f = FlowView(w);
		Slider(f,100@10);
		f.startRow;
		b = f.indentedRemaining;
		p = f.bounds;
		this.assert(p.containsRect(b),"indented remaining should be inside parent");
		this.assert(p.height > b.height,"indentedRemaining height should be smaller than parent");
		this.assertEquals(p.width, (b.width + (f.decorator.gap.x)),
			"indentedRemaining width should be equal to parent plus gap");
	}
	
	
	test_withNilBounds {
		var f,c,fb,cb;
		c = CompositeView(w,Rect(100,100,200,200));
		// nil bounds
		f = FlowView(c);
		c.background = Color.rand;
		f.background = Color.rand;
		cb = c.absoluteBounds;
		fb = f.absoluteBounds;
		this.assertEquals( cb.origin, fb.origin, "with no supplied bounds, origin should be at the parent's origin");	
	}
	
	test_remove {
		/*
			Qt now gives the decorator a chance to remove the view
			so this is fixed for Qt but still remains an issue for Cocoa,
			so the fix in FlowViewLayout.reflow is still needed for Cocoa for the time being.
		*/
		var a,b,c;
		a = FlowView(w,Rect(0,0,200,200));
		b = FlowView(b,Rect(0,0,200,200));
		"hello".gui(b);
		b.background = Color.blue;
		a.background = Color.white;

		// the blue view disappears
		b.remove(true);

		// now reflow
		b.decorator.reflow;

		// bounds checking error
		/*
		ERROR: Qt: Failed to get the value of property 'geometry'
		ERROR: Primitive '_QObject_GetProperty' failed.
		Failed.
		RECEIVER:
		Instance of QView {    (0x117312d48, gc=AC, fmt=00, flg=00, set=06)
		  instance variables [34]
		    qObject : RawPointer 0x11f398460
			*/

		// b's QView did .remove and .destroy
		// and it doesn't show up in w's children
		this.assertEquals( b.children.size, 0, "having removed, the inner view should have no children");

		// but the QView is still in the decorator
		// because nothing removed it from there
		
		this.assertEquals( b.decorator.rows.first.size, 0, "having removed, the inner view's decorator should have nothing in its first row");
		// was
		//[ List[ a QView ] ]
	}
	
	/*

w = GUI.window.new;
w.front;

w.view.horz({ |h|
	GUI.staticText.new(h,100@100).string_("horz").background = Color.red;

	GUI.staticText.new(h,100@100).string_("horz").background = Color.red;

	// should be to the right of the other two
	h.scroll({ |s|
		GUI.staticText.new(s,200@200)
			.string_("scroll123456789abcdefghijklmnopqrstuvwxyz").background = Color.blue;
	},100@100)


})

*/

}


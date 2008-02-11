
TestSCHLayout : UnitTest {
	
	test_elasticBackground {
		var q,w,h;
		q = 10;
		w = SCWindow.new("background should be elastic when you resize the window");

		h = SCHLayoutView(w,Rect(0,0,300,300));
		h.background = Color.red(alpha:0.1);
		h.resize = 5; //   elastic
		Array.fill(q,{  arg i;
			var s;
			s = SCSlider(h,Rect(0,0,20,75));
			s.value = i / q;
			s
		});

		w.front

	}
	
	test_flowInRelativeComposite {
	
		var shouldBe;
		shouldBe = [Rect(120, 60, 50, 40),
		Rect(180, 60, 50, 40),
		Rect(240, 60, 50, 40),
		Rect(300, 60, 50, 40),
			Rect(120, 110, 50, 40),
			Rect(180, 110, 50, 40),
			Rect(240, 110, 50, 40),
			Rect(300, 110, 50, 40),
			Rect(120, 160, 50, 40),
			Rect(180, 160, 50, 40),
			Rect(240, 160, 50, 40),
			Rect(300, 160, 50, 40)
			];
				
		[GUI.compositeView,GUI.hLayoutView].do({ |childClass,i|
			var win, parent, numChildren = 12,k;
			
			win = GUI.window.new(childClass.asString, Rect(100, 100 + (330*i), 400, 300));
			
			parent = GUI.compositeView.new(win, Rect(50, 20, 300, 200));
			parent.background = Color(0.7, 0.7, 0.9);
			parent.decorator = FlowLayout(parent.bounds, 20 @ 20, 10 @ 10);
			
			parent.relativeOrigin = true; /* makes no difference here */
			
			k = Array.fill(numChildren, { |i| var kid;
				//parent.decorator.left.postln;
			Ê Ê Ê Êkid = childClass.perform(\new, parent, 50 @ 40);
			Ê Ê Ê Êkid.background = Color.rand(0.1, 0.2).alpha_(0.2);
				kid
			});
		
			k.do({ |kid,i|
				this.assertEquals(kid.absoluteBounds,shouldBe[i],"should be Rect" + childClass);
			});
			
			win.front;
		});

	}

}


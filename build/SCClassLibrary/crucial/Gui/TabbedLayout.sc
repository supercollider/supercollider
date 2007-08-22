
TabbedLayout {
	
	var prev,e,v,c;
	var <>action;
	
	*new { |layout,bounds,tabs|
		^super.new.gui(layout,bounds,tabs.clump(2))
	}
	
	gui { |layout,bounds,tabs|
		var c,names;
		if(bounds.notNil,{ bounds = bounds.asRect });
		layout = layout.asFlowView(bounds);

		e = SCButton(layout,Rect(0,0,min(160,bounds.width),20));
		names = tabs.collect({ |t| t.first  });
		e.states = Array.fill(tabs.size,{ |i|
			[ names[i].asString, Color.black,Color.white ]
		});
		e.action = { arg butt;
			this.select(butt.value);
			action.value(this,butt.value);
		};
		
		layout.startRow;
	
		c = SCCompositeView(layout,bounds.resizeBy(0,20).moveBy(0,20));
		v = [];
		tabs.do({|tb|
			var inner,title,buildFunction;
			buildFunction = tb[1];
			inner = FlowView(c,c.bounds);
			buildFunction.value(inner);
			v = v.add( inner );
		});

		v.do({ |inner,i|
			if(i > 0,{
				inner.visible = false
			})
		});
		prev = v.first;
	}
	select { arg i; 
		prev.visible = false;
		prev = v.at(i);
		prev.visible = true;
	}
	selected { ^v.indexOf(prev) }
	background_ { arg color; c.background = color; }
}


	// platform independent wrapper for a range slider. 
	
EZRangerJ { 
	var <w, <zone, <labelView, <loBox, <rangeSlider, <hiBox, <unitView, <views;
	var <spec, <lo, <hi, <>round=0.001, <>action;
	
	*new { |w, dimensions, label, spec, action, initVal, initAction=false, 
		labelWidth=60, numberWidth=40, unitWidth=30, useTwoLines=false| 
		
			^super.new.init(w, dimensions, label, spec, action, initVal, initAction, 
			labelWidth, numberWidth, unitWidth, useTwoLines)
	} 
	
	init { |argW, dimensions, argLabel, argSpec, argAction, initVal, initAction=false, 
		argLabelWidth, numberWidth, unitWidth, useTwoLines|

		var sys = GUI.scheme; 
		var skin = GUI.skin;
		var font = sys.font.new(*skin.fontSpecs);
		var lineHeight;
		
		spec = argSpec.asSpec;
		action = argAction; 
		w = argW; 
		
		dimensions = (dimensions ?? { if (useTwoLines, 300@40, 400 @ 20) });
		
		zone = sys.compositeView.new(argW, dimensions);
		zone.background_(skin.foreground); 
		zone.decorator = FlowLayout(zone.bounds, 0@0, skin.gap);

			// different between 
		if (useTwoLines) { 
			lineHeight = dimensions.y - skin.gap.y div: 2; 
						
			labelView = GUI.staticText.new(zone, argLabelWidth @ lineHeight);
			loBox = GUI.numberBox.new(zone, numberWidth @ lineHeight );
			hiBox = GUI.numberBox.new(zone, numberWidth@ lineHeight);
			unitView = GUI.staticText.new(zone, unitWidth @ lineHeight);

			zone.decorator.nextLine; 
			
			rangeSlider = GUI.rangeSlider.new(zone, dimensions.x @ lineHeight);
		} {	
			lineHeight = dimensions.y;
			labelView = GUI.staticText.new(zone, argLabelWidth @ lineHeight);
			loBox = GUI.numberBox.new(zone, numberWidth @ lineHeight);
			
			rangeSlider = GUI.rangeSlider.new(zone, 
				dimensions.x - (numberWidth * 2 + argLabelWidth + unitWidth + (skin.gap.x * 5))
				@ lineHeight
			);
			hiBox = GUI.numberBox.new(zone, numberWidth @ lineHeight);
			unitView = GUI.staticText.new(zone, unitWidth @ lineHeight);
		};
	
		labelView.align_(0).string_(argLabel);
		loBox.action_({ |box| this.lo_(box.value).doAction; });
		rangeSlider.action_({ |sl| 
				this.lo_(spec.map(sl.lo)); 
				this.hi_(spec.map(sl.hi)); 
				this.doAction;
			});
		hiBox.action_({ |box| this.hi_(box.value).doAction; });
		unitView.align_(0).string_(spec.units ? "");
		views = [labelView, loBox, rangeSlider, hiBox, unitView];
		
		if (initVal.notNil) { this.value_(initVal) };
		if (initAction) { this.doAction };
	}
	
	spec_ { }

	doAction { action.value(this); } 
	value { ^[lo, hi] }
	value_ { |vals| this.lo_(vals[0]).hi_(vals[1]) }
	valueAction_ { |vals| this.value_(vals).doAction }
	
	lo_ { |val|
		lo = spec.constrain(val);
		loBox.value_(lo.round(round));
		rangeSlider.lo_(spec.unmap(lo));
	}
	
	hi_ { |val|
		hi = spec.constrain(val);
		hiBox.value_(hi.round(round));
		rangeSlider.hi_(spec.unmap(hi));
	}
	
	visible { ^zone.visible }
	visible_ { |flag| zone.visible_(flag) }
	enabled { ^zone.enabled }
	enabled_ { |flag| zone.enabled_(flag) }

	remove { 	zone.remove }
}

	// EZRanger laid out in two lines

EZRanger2 : EZRanger { 
	*new { |w, dimensions, label, spec, action, initVal, initAction=false, 
		labelWidth=60, numberWidth=40, unitWidth=30, useTwoLines=true|
				
		^EZRanger.new(w, dimensions, label, spec, action, initVal, initAction, 
		labelWidth, numberWidth, unitWidth, useTwoLines);
	}
}

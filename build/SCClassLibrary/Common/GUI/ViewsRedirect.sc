ViewRedirecter { // Abstract class
	*implClass { ^GUI.scheme.perform(this.key) }
	*new { arg parent, bounds; ^this.implClass.new(parent, bounds) }
	*browse { ^this.implClass.browse }
	*doesNotUnderstand{|selector ... args|	^this.implClass.perform(selector, *args)  }
}


Window : ViewRedirecter { 
	*key { ^\window }
	*new { arg name = "panel", bounds, resizable = true, border = true, server, scroll = false;
		^this.implClass.new(name, bounds, resizable, border, server, scroll)
	}
}

CompositeView : ViewRedirecter { *key { ^\compositeView }}
ScrollView : ViewRedirecter { *key { ^\scrollView }}
HLayoutView : ViewRedirecter { *key { ^\hLayoutView }}
VLayoutView : ViewRedirecter { *key { ^\vLayoutView }}

Slider : ViewRedirecter { *key { ^\slider }}

//Knob : SCSlider {
//}

//Font : ViewRedirecter { *key { ^\font }}
Pen : ViewRedirecter { *key { ^\pen }}

Stethoscope : ViewRedirecter { *key { ^\stethoscope }}
ScopeView : ViewRedirecter { *key { ^\scopeView }}
FreqScopeView : ViewRedirecter { *key { ^\freqScopeView }} // redirects to SCFreqScope
FreqScope : ViewRedirecter { *key { ^\freqScope }} // redirects to SCFreqScopeWindow
QuartzComposerView : ViewRedirecter { *key { ^\quartzComposerView }}
Dialog : ViewRedirecter { *key { ^\dialog }}
View : ViewRedirecter { *key { ^\view }}

RangeSlider : ViewRedirecter { *key { ^\rangeSlider }}
Slider2D : ViewRedirecter { *key { ^\slider2D }}
TabletSlider2D : ViewRedirecter { *key { ^\tabletSlider2D }}
Button : ViewRedirecter { *key { ^\button }}

PopUpMenu : ViewRedirecter { *key { ^\popUpMenu }}
StaticText : ViewRedirecter { *key { ^\staticText }}
NumberBox : ViewRedirecter { *key { ^\numberBox }}
ListView : ViewRedirecter { *key { ^\listView }}

DragSource : ViewRedirecter { *key { ^\dragSource }}
DragSink : ViewRedirecter { *key { ^\dragSink }}
DragBoth : ViewRedirecter { *key { ^\dragBoth }}

UserView : ViewRedirecter { *key { ^\userView }}
MultiSliderView : ViewRedirecter { *key { ^\multiSliderView }}
EnvelopeView : ViewRedirecter { *key { ^\envelopeView }}

TextField : ViewRedirecter  { *key { ^\textField }}


TabletView : ViewRedirecter { *key { ^\tabletView }}
SoundFileView : ViewRedirecter { *key { ^\soundFileView }}
MovieView : ViewRedirecter { *key { ^\movieView }}
TextView : ViewRedirecter  {	*key { ^\textView }}

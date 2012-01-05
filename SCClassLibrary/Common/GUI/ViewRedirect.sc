ViewRedirect { // Abstract class

	classvar <>redirectQueries = false;

	*implScheme{
		^GUI.scheme
	}

	*implClass {
		^GUI.scheme.perform(this.key)
	}
	*key { ^\viewRedirect }
	*new { arg parent, bounds;
		var	impl;
		if ( this.implScheme.notNil ){
			if((impl = this.implClass).notNil) {
				^impl.new(parent, bounds)
			} {
				MethodError("ViewRedirect is an abstract class and should not be instantiated directly. *new method not valid.", this).throw;}
		}{
			("No GUI scheme active: " + this.key ++ ".new" + parent ).warn;
		}
	}
	*browse { ^ClassBrowser(this.implClass ?? { ViewRedirect }) }
	*doesNotUnderstand{|selector ... args|
		var	impl;
		if ( this.implScheme.notNil ){
			if((impl = this.implClass).notNil) {
				^impl.perform(selector, *args)
			} {
				DoesNotUnderstandError(this, selector, args).throw;
			}
		}{
			("No GUI scheme active: " + selector + args ).warn;
		}
	}
	*classRedirect { ^redirectQueries.if({this.implClass ? this}, this)}
}

Window : ViewRedirect {
	*key { ^\window }
	*new { arg name = "panel", bounds, resizable = true, border = true, server, scroll = false;
		^this.implClass.new(name, bounds, resizable, border, server, scroll)
	}
}

CompositeView : ViewRedirect { *key { ^\compositeView }}
ScrollView : ViewRedirect { *key { ^\scrollView }}
HLayoutView : ViewRedirect { *key { ^\hLayoutView }}
VLayoutView : ViewRedirect { *key { ^\vLayoutView }}

Slider : ViewRedirect { *key { ^\slider }}

//Knob : SCSlider {
//}

//Font : ViewRedirect { *key { ^\font }}
Pen : ViewRedirect { *key { ^\pen }}

Stethoscope : ViewRedirect {
	*new {  arg server, numChannels = 2, index, bufsize = 4096, zoom, rate, view, bufnum;
		index = index.asControlInput;
		^this.implClass.new(server, numChannels, index, bufsize, zoom, rate, view, bufnum)
	}
	*key { ^\stethoscope }

}
ScopeView : ViewRedirect { *key { ^\scopeView }}
FreqScopeView : ViewRedirect { *key { ^\freqScopeView }} // redirects to FreqScope

FreqScope : ViewRedirect { // redirects to FreqScopeWindow
	*new { arg width=512, height=300, busNum=0, scopeColor, bgColor;
		busNum = busNum.asControlInput;
		^this.implClass.new(width, height, busNum, scopeColor)
	}
	*key { ^\freqScope }
}

Dialog : ViewRedirect { *key { ^\dialog }}
View : ViewRedirect { *key { ^\view }}

RangeSlider : ViewRedirect { *key { ^\rangeSlider }}
Slider2D : ViewRedirect { *key { ^\slider2D }}
TabletSlider2D : ViewRedirect { *key { ^\tabletSlider2D }}
Button : ViewRedirect { *key { ^\button }}

PopUpMenu : ViewRedirect { *key { ^\popUpMenu }}
StaticText : ViewRedirect { *key { ^\staticText }}
NumberBox : ViewRedirect { *key { ^\numberBox }}
ListView : ViewRedirect { *key { ^\listView }}

DragSource : ViewRedirect { *key { ^\dragSource }}
DragSink : ViewRedirect { *key { ^\dragSink }}
DragBoth : ViewRedirect { *key { ^\dragBoth }}

UserView : ViewRedirect { *key { ^\userView }}
MultiSliderView : ViewRedirect { *key { ^\multiSliderView }}
EnvelopeView : ViewRedirect { *key { ^\envelopeView }}

TextField : ViewRedirect  { *key { ^\textField }}


TabletView : ViewRedirect { *key { ^\tabletView }}
SoundFileView : ViewRedirect { *key { ^\soundFileView }}
MovieView : ViewRedirect { *key { ^\movieView }}
TextView : ViewRedirect  {	*key { ^\textView }}

Font : ViewRedirect  {
	*key { ^\font }
	*findFirstAvailable { |fontNames, action|
		Routine {
			fontNames.do { |name|
				if(this.availableFonts.any(_.contains(name))) {
					action.value(name);
					nil.alwaysYield;
				}
			}
		}.play(AppClock)
}
}

Knob : ViewRedirect  {	*key { ^\knob }}

LevelIndicator : ViewRedirect  {	*key { ^\levelIndicator }}

Image : ViewRedirect { *key { ^\image }}

WebView : ViewRedirect { *key { ^\webView }}

CheckBox : ViewRedirect { *key { ^\checkBox }}

TreeView : ViewRedirect { *key { ^\treeView }}

LayoutRedirect : ViewRedirect {
	*new { arg ... args;
		^this.implClass.new(*args);
	}
}

HLayout : LayoutRedirect { *key { ^\hLayout }}
VLayout : LayoutRedirect { *key { ^\vLayout }}
GridLayout : LayoutRedirect { *key { ^\gridLayout }}

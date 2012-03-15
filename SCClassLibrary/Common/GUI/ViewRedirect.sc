GuiRedirect { // Abstract
	classvar <>redirectQueries = false;

	*implScheme{
		^GUI.scheme
	}

	*implClass {
		var scheme, key, target;
		scheme = GUI.scheme;
		if (scheme.isNil) {
			Error("No GUI scheme active.", this).throw
		};
		key = this.key;
		target = key !? {GUI.scheme.tryPerform(key)};
		if (target.isNil) {
			Error("No redirection for" + this.name + "available in the active GUI scheme:" +
				scheme.id ++ ".", this).throw;
		}
		^target;
	}

	*classRedirect { ^redirectQueries.if({this.implClass ? this}, this)}

	*key { ^nil }

	*new { arg ...args;
		^this.implClass.new(*args);
	}

	*doesNotUnderstand{|selector ... args|
		var impl;
		^this.implClass.perform(selector, *args);
	}

	*browse { ^ClassBrowser( this.key !? {this.implClass} ? this ) }
}

ViewRedirect : GuiRedirect { // Abstract class
	*new { arg parent, bounds;
		^super.new(parent, bounds);
	}
}

Window : GuiRedirect {
	*key { ^\window }
	*new { arg name = "panel", bounds, resizable = true, border = true, server, scroll = false;
		^super.new(name, bounds, resizable, border, server, scroll)
	}
}

CompositeView : ViewRedirect { *key { ^\compositeView }}
ScrollView : ViewRedirect { *key { ^\scrollView }}
HLayoutView : ViewRedirect { *key { ^\hLayoutView }}
VLayoutView : ViewRedirect { *key { ^\vLayoutView }}

Slider : ViewRedirect { *key { ^\slider }}

Pen : GuiRedirect { *key { ^\pen }}

Stethoscope : GuiRedirect {
	*new {  arg server, numChannels = 2, index = 0, bufsize = 4096, zoom = 1, rate = \audio, view, bufnum;
		index = index.asControlInput;

		if (GUI.id == \qt and:
			{ server.serverRunning and: server.hasShmInterface.not })
		{
			^GUI.current.stethoscope1.new
				(server, numChannels, index, bufsize, zoom, rate, view, bufnum)
		}{
			^super.new(server, numChannels, index, bufsize, zoom, rate, view, bufnum)
		};
	}
	*key { ^\stethoscope }
}

ScopeView : ViewRedirect { *key { ^\scopeView }}
FreqScopeView : ViewRedirect { *key { ^\freqScopeView }} // redirects to FreqScope

FreqScope : GuiRedirect { // redirects to FreqScopeWindow
	*new { arg width=512, height=300, busNum=0, scopeColor, bgColor;
		busNum = busNum.asControlInput;
		^super.new(width, height, busNum, scopeColor)
	}
	*key { ^\freqScope }
}

Dialog : GuiRedirect {
	*key { ^\dialog }
	*openPanel { arg okFunc, cancelFunc, multipleSelection=false;
		^super.openPanel(okFunc, cancelFunc, multipleSelection)
	}
	*savePanel { arg okFunc, cancelFunc;
		^super.savePanel(okFunc, cancelFunc)
	}
}

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
TextView : ViewRedirect  { *key { ^\textView }}

Font : GuiRedirect  {
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
	*new { arg name, size, bold = false, italic = false, isPointSize = false;
		^super.new(name, size, bold, italic, isPointSize)
	}
}

Knob : ViewRedirect  { *key { ^\knob }}

LevelIndicator : ViewRedirect  { *key { ^\levelIndicator }}

Image : ViewRedirect { *key { ^\image }}

WebView : ViewRedirect { *key { ^\webView }}

CheckBox : ViewRedirect { *key { ^\checkBox }}

TreeView : ViewRedirect { *key { ^\treeView }}

HLayout : GuiRedirect { *key { ^\hLayout }}
VLayout : GuiRedirect { *key { ^\vLayout }}
GridLayout : GuiRedirect { *key { ^\gridLayout }}
StackLayout : GuiRedirect { *key { ^\stackLayout } }

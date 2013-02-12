/**
 *	This is the Cocoa framework GUI implementation
 *	for Mac OS X. It can be accessed from the GUI
 *	class using GUI.cocoa, GUI.fromID( \cocoa ) or GUI.get( \cocoa ).
 *
 *	@version	0.14, 17-Jan-08
 */
CocoaGUI {
	classvar extraClasses;

	*initClass {
		Class.initClassTree( Event );
		extraClasses = Event.new;
		Class.initClassTree( GUI );
		GUI.add( this );
	}

	*id { ^\cocoa }

	*put { arg key, object;
		extraClasses.put( key, object );
	}

	*doesNotUnderstand { arg selector ... args;
		^extraClasses.perform( selector, *args );
	}

	///////////////// Common -> GUI /////////////////

	*freqScope     { ^PlusFreqScopeWindow }
	*freqScopeView { ^PlusFreqScope }
	*scopeView { ^SCScope }
	*stethoscope { ^SCStethoscope }

	///////////////// Common -> GUI -> Base /////////////////

	*view { ^SCView }
	*window { ^SCWindow }
	*compositeView { ^SCCompositeView }
	*hLayoutView { ^SCHLayoutView }
	*vLayoutView { ^SCVLayoutView }
	*slider { ^SCSlider }
	*rangeSlider { ^SCRangeSlider }
	*slider2D { ^SC2DSlider }
	*tabletSlider2D { ^SC2DTabletSlider }
	*button { ^SCButton }
	*popUpMenu { ^SCPopUpMenu }
	*staticText { ^SCStaticText }
	*listView { ^SCListView }
	*dragSource { ^SCDragSource }
	*dragSink { ^SCDragSink }
	*dragBoth { ^SCDragBoth }
	*numberBox { ^SCNumberBox }
	*textField { ^SCTextField }
	*userView { ^SCUserView }
	*multiSliderView { ^SCMultiSliderView }
	*envelopeView { ^SCEnvelopeView }
	*tabletView { ^SCTabletView }
	*soundFileView { ^SCSoundFileView }
	*movieView { ^SCMovieView }
	*textView { ^SCTextView }
	*quartzComposerView { ^SCQuartzComposerView }
	*scrollView { ^SCScrollView }
	*ezScroller { ^EZScroller }
	*ezSlider { ^EZSlider }
	*ezListView { ^EZListView }
	*ezPopUpMenu { ^EZPopUpMenu}
	*ezNumber { ^EZNumber}
	*ezRanger { ^EZRanger }
	*menuItem { ^CocoaMenuItem }
	*levelIndicator { ^SCLevelIndicator }

	*knob { ^SCKnob }

	*font { ^SCFont }
	*pen { ^SCPen }
	*image { ^SCImage }
	*webView {^SCWebView }

	///////////////// Common -> Audio /////////////////

	*mouseX { ^MouseX }
	*mouseY { ^MouseY }
	*mouseButton { ^MouseButton }
	*keyState { ^KeyState }

	///////////////// Common -> OSX /////////////////

	*dialog { ^CocoaDialog }
	*speech { ^Speech }

	///////////////// extras /////////////////

	*cursorPosition { ^this.prGetMouseCoords(Point.new) }

	*prGetMouseCoords {|point|
		_Mouse_getCoords
		^this.primitiveFailed
	}

//	checkBox: SCCheckBox,
//	tabbedPane: SCTabbedPane

	///////////////// crucial /////////////////
	// no longer needed b/c SwingOSC can add the StartRow class directly
	// as of at least 0.56
//	*startRow { ^StartRow }

	///////////////// support methods with different GUI scheme implementations /////////////////
	*stringBounds { |string, font|
		^string.prBounds(Rect.new, font)
	}
}

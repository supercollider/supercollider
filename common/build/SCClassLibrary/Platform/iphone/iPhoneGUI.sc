/**
 *	This is the Cocoa framework GUI implementation
 *	for Mac OS X. It can be accessed from the GUI
 *	class using GUI.cocoa, GUI.fromID( \cocoa ) or GUI.get( \cocoa ).
 *
 *	@version	0.14, 17-Jan-08
 */
IPhoneGUI {
	classvar extraClasses;

	*initClass {
		Class.initClassTree( Event );
		extraClasses = Event.new;
		Class.initClassTree( GUI );
		GUI.add( this );
	}

	*id { ^\iphone }

	*put { arg key, object;
		extraClasses.put( key, object );
	}

	*doesNotUnderstand { arg selector ... args;
		^extraClasses.perform( selector, *args );
	}

	///////////////// Common -> GUI /////////////////
/*
	*freqScope { ^SCFreqScopeWindow }
	*freqScopeView { ^SCFreqScope }
	*scopeView { ^SCScope }
	*stethoscope { ^SCStethoscope }
*/
	///////////////// Common -> GUI -> Base /////////////////

	*view { ^SCView }
	*compositeView { ^SCCompositeView }
	*hLayoutView { ^SCHLayoutView }
	*vLayoutView { ^SCVLayoutView }
	*window { ^SCWindow }
	*slider { ^SCSlider }
	*button { ^SCButton }
	*staticText { ^SCStaticText }
	*numberBox { ^SCNumberBox }
	*scrollView { ^SCScrollView }
	*font { ^SCFont }
/*
	*rangeSlider { ^SCRangeSlider }
	*slider2D { ^SC2DSlider }
	*tabletSlider2D { ^SC2DTabletSlider }
	*popUpMenu { ^SCPopUpMenu }
	*listView { ^SCListView }
	*dragSource { ^SCDragSource }
	*dragSink { ^SCDragSink }
	*dragBoth { ^SCDragBoth }
	*textField { ^SCTextField }
	*userView { ^SCUserView }
	*multiSliderView { ^SCMultiSliderView }
	*envelopeView { ^SCEnvelopeView }
	*tabletView { ^SCTabletView }
	*soundFileView { ^SCSoundFileView }
	*movieView { ^SCMovieView }
	*textView { ^SCTextView }
	*quartzComposerView { ^SCQuartzComposerView }
	*ezScroller { ^EZScroller }
	*ezSlider { ^EZSlider }
	*ezListView { ^EZListView }
	*ezPopUpMenu { ^EZPopUpMenu}
	*ezNumber { ^EZNumber}
	*ezRanger { ^EZRanger }
	*menuItem { ^CocoaMenuItem }

	*knob { ^SCKnob }

	*pen { ^SCPen }
*/
	///////////////// Common -> Audio /////////////////

/*
	*mouseX { ^MouseX }
	*mouseY { ^MouseY }
	*mouseButton { ^MouseButton }
	*keyState { ^KeyState }
*/
	///////////////// Common -> OSX /////////////////

/*
	*dialog { ^CocoaDialog }
	*speech { ^Speech }
*/
	///////////////// extras /////////////////

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

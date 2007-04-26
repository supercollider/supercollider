/**
 *	This is the Cocoa framework GUI implementation
 *	for Mac OS X. It can be accessed from the GUI
 *	class using GUI.cocoa, GUI.fromID( \cocoa ) or GUI.get( \cocoa ).
 *
 *	@version	0.12, 25-Apr-07
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
	*scopeView { ^SCScope }
	*freqScope { ^FreqScope }
	*freqScopeView { ^SCFreqScope }
	*ezSlider { ^EZSlider }
	*ezNumber { ^EZNumber }
	*stethoscope { ^Stethoscope }
	
	*font { ^Font }
	*pen { ^Pen }
			
	///////////////// Common -> Audio /////////////////

	*mouseX { ^MouseX }
	*mouseY { ^MouseY }
	*mouseButton { ^MouseButton }
	*keyState { ^KeyState }
			
	///////////////// Common -> OSX /////////////////

	*dialog { ^CocoaDialog }
	*speech { ^Speech }

	///////////////// extras /////////////////
			
//	checkBox: SCCheckBox,
//	tabbedPane: SCTabbedPane

	///////////////// crucial /////////////////
	*startRow { ^StartRow }
}

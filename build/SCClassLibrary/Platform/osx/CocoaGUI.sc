/**
 *	This is the Cocoa framework GUI implementation
 *	for Mac OS X. It can be accessed from the GUI
 *	class using GUI.cocoa, GUI.fromID( \cocoa ) or GUI.get( \cocoa ).
 *
 *	@version	0.1, 29-Jan-06
 */
CocoaGUI {
	*initClass {
		Class.initClassTree( GUI );
		GUI.add( this );
	}

	*id { ^\cocoa }
	
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
}

/**
 *	Factory abstraction for all GUI related core classes.
 *	Each GUI kit is described by a scheme that maps class names
 *	to actual classes.
 *
 *	See the help file for details. The default
 *	scheme is cocoa.
 *
 *	Changelog:
 *		-
 *
 *	@version	0.1, 13-Apr-06
 */
GUI { 
	classvar <scheme, schemes, <skin, skins;
	
	*new { |key| ^scheme.at(key) }
	
	*initClass { 
		skins = (
			default: (
				fontSpecs: 	["Monaco", 10],
				fontColor: 	Color.black,
				background: 	Color(0.8, 0.85, 0.7, 0.5),
				foreground:	Color.grey(0.95),
				onColor:		Color(0.5, 1, 0.5), 
				offColor:		Color.clear,
				gap:			0 @ 0,
				margin: 		2@2,
				buttonHeight:	16
			)
		); 
		
		skin = skins.default;
		
		schemes = (
			cocoa: (
				id: \cocoa,
				
				///////////////// Common -> GUI -> Base /////////////////
				
				view: SCView,

				window: SCWindow,

// abstract
//				containerView: SCContainerView,
				compositeView: SCCompositeView,
// shouldn't be instantiated directly
//				topView: SCTopView,
// quasi abstract
//				layoutView: SCLayoutView, 
				hLayoutView: SCHLayoutView,
				vLayoutView: SCVLayoutView,
				
				slider: SCSlider,
// not yet implemented ?
//				knob: SCKnob, 
				rangeSlider: SCRangeSlider,
				slider2D: SC2DSlider, 				// cannot start with a numeric character
				tabletSlider2D: SC2DTabletSlider,  	// cannot start with a numeric character
				
				button: SCButton,
				popUpMenu: SCPopUpMenu,
				staticText: SCStaticText,
				listView: SCListView,

				dragSource: SCDragSource,
				dragSink: SCDragSink,
				dragBoth: SCDragBoth, 

				numberBox: SCNumberBox,
				textField: SCTextField,

				userView: SCUserView,
				multiSliderView: SCMultiSliderView,
				envelopeView: SCEnvelopeView,
				
			 	tabletView: SCTabletView,
				soundFileView: SCSoundFileView,
				movieView: SCMovieView,
				textView: SCTextView,
				
				scopeView: SCScope,
				freqScope: FreqScope,
				freqScopeView: SCFreqScope,
								
				ezSlider: EZSlider, 
				ezNumber: EZNumber,
				stethoscope: Stethoscope,
				
				font: Font,
				
				///////////////// Common -> Audio /////////////////

				mouseX: MouseX, 
				mouseY: MouseY,
				mouseButton: MouseButton,
				keyState: KeyState,
				
				pen: Pen,
				
				///////////////// Common -> OSX /////////////////

				dialog: CocoaDialog,
				speech: Speech
			),
			
			swing: (
				id: \swing,

				///////////////// Common -> GUI -> Base /////////////////

				view: JSCView,

				window: JSCWindow,

// abstract
//				containerView: JSCContainerView,
				compositeView: JSCCompositeView,
//				topView: JSCTopView,
// quasi abstract
//				layoutView: JSCLayoutView, 
				hLayoutView: JSCHLayoutView,
				vLayoutView: JSCVLayoutView,
				
				slider: JSCSlider,
//				knob: SCKnob, 
				rangeSlider: JSCRangeSlider,
				slider2D: JSC2DSlider,
//				tabletSlider2D: JSC2DTabletSlider,
				
				button: JSCButton,
				popUpMenu: JSCPopUpMenu,
				staticText: JSCStaticText,
				listView: JSCListView,

				dragSource: JSCDragSource,
				dragSink: JSCDragSink,
				dragBoth: JSCDragBoth, 

				numberBox: JSCNumberBox,
				textField: JSCTextField,

//				userView: JSCUserView,
//				multiSliderView: JSCMultiSliderView,
//				envelopeView: JSCEnvelopeView,
					
//			 	tabletView: JSCTabletView,
//				soundFileView: JSCSoundFileView,
//				movieView: JSCMovieView,
//				textView: JSCTextView,
							
				scopeView: JSCScope,
				freqScope: JFreqScope,
				freqScopeView: JSCFreqScope,
				
				ezSlider: JEZSlider, 
				ezNumber: JEZNumber,
				stethoscope: JStethoscope,
				
				font: JFont,
				
				///////////////// Common -> Audio /////////////////

				mouseX: JMouseX, 
				mouseY: JMouseY,
				mouseButton: JMouseButton
//				keyState: JKeyState,

				///////////////// Common -> OSX /////////////////

//				pen: JPen,

//				dialog: JDialog,
//				speech: JSpeech
			)
		);
		scheme = this.cocoa;
	}

	/**
	 *	Makes Cocoa (Mac OS X GUI) the current scheme
	 *	and returns it. Subsequent GUI object calls
	 *	to GUI are deligated to cocoa.
	 *
	 *	@returns	the current (cocoa) scheme
	 */
	*cocoa { 
		^scheme = schemes.cocoa;
	}
	
	/**
	 *	Makes Swing (Java GUI) the current scheme
	 *	and returns it. Subsequent GUI object calls
	 *	to GUI are deligated to swing.
	 *
	 *	@returns	the current (swing) scheme
	 */
	*swing { 
		if (\JSCWindow.asClass.isNil) { 
			warn("	You do not seem to have SwingOSC installed - JSCWindow is missing." 
			"	So, no switching to SwingOSC, GUI.scheme is still cocoa.");
			^this;
		};
		^scheme = schemes.swing;
	}
	
	/**
	 *	Changes the current scheme and returns it.
	 *
	 *	@param	name		(Symbol) the identifier of the scheme to
	 *					use, such as returned by calling
	 *					aScheme.id
	 *	@returns	the new current scheme
	 */
	*fromID { arg id;
		^scheme = schemes[ id.asSymbol ];
	}

	/**
	 *	Returns the current scheme. This
	 *	is useful for objects that, upon instantiation,
	 *	wish to store the then-current scheme, so as
	 *	to be able to consistently use the same scheme
	 *	in future method calls.
	 *
	 *	Note: the caller shouldn't make any assumptions about
	 *	the nature (the class) of the returned object, so that
	 *	the actual implementation (an Event) may change in the future.
	 *
	 *	@returns	the current scheme
	 */
	*current {
		^scheme;
	}
	
	/**
	 *	All method calls are mapped to the current
	 *	scheme, so that for example GUI.button can be used
	 *	and is delegated to the button association of the
	 *	current scheme.
	 */
	*doesNotUnderstand { arg selector ... args;
		^scheme.perform( selector, args );
	}
}
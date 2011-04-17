/**
 *	Factory abstraction for all GUI related core classes.
 *	Each GUI kit is described by a scheme that maps class names
 *	to actual classes.
 *
 *	See the help file for details. The default
 *	scheme is cocoa.
 *
 *	Changelog:
 *		- jrh added makeGUI
 *		- sciss added add, set, get, use, useID
 *
 *	@version	0.16, 10-Apr-07
 */
GUI {
	classvar <scheme, <schemes, <skin, <skins;

	*new { arg key; ^scheme.perform( key )}

	*makeGUI { arg key, args, properties;
		var meth, class;
		class = scheme.at(key);
		if(properties.notNil) {
			meth = class.class.findMethod(\new);
			if(meth.notNil) {
				meth.argNames.drop(args.size).do { |key| args = args ++ properties[key] }
			}
		};
		meth.argNames.postln;
		args.postln;
		^class.new(*args)
	}

	*initClass {
		skins = (
			default: (
				fontSpecs: 	["Helvetica", 10],
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

		skin		= skins.default;
		schemes	= IdentityDictionary.new;
	}

	/**
	 *	Makes Cocoa (Mac OS X GUI) the current scheme
	 *	and returns it. Subsequent GUI object calls
	 *	to GUI are delegated to cocoa.
	 *
	 *	@return	the current (cocoa) scheme
	 */
	*cocoa {
		^this.fromID( \cocoa );
	}

	/**
	 *	Makes Swing (Java GUI) the current scheme
	 *	and returns it. Subsequent GUI object calls
	 *	to GUI are delegated to swing.
	 *
	 *	@return	the current (swing) scheme
	 */
	*swing {
		^this.fromID( \swing );
	}

	/**
	*  Makes qt (Qt GUI) the current scheme
	*  and returns it. Subsequent GUI object calls
	*  to GUI are delegated to qt.
	*
	*  @return the current (qt) scheme
	*/
	*qt {
		^this.fromID( \qt );
	}

	/**
	 *	Changes the current scheme and returns it.
	 *
	 *	@param	id		(Symbol) the identifier of the scheme to
	 *					use, such as returned by calling
	 *					aScheme.id
	 *	@return	the new current scheme
	 */
	*fromID { arg id;
		var newScheme = schemes[ id.asSymbol ];
		if( newScheme.notNil, {
			scheme = newScheme;
		}, {
			("GUI.fromID : The GUI scheme '" ++ id ++ "' is not installed\n" ++
			 "The current scheme is still '" ++ if( scheme.notNil, { scheme.id }) ++ "'!").warn;
		});
		^scheme;
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
	 *	@return	the current scheme
	 */
	*current {
		^scheme;
	}

	/**
	 *	Returns the scheme for a given identifier.
	 *	Does not switch the current scheme.
	 *
	 *	@param	id		(Symbol) the identifier of the scheme to
	 *					retrieve, such as returned by calling
	 *					aScheme.id
	 *	@return	the scheme for the given id or nil
	 */
	*get { arg id;
		^schemes[ id.asSymbol ];
	}

	/**
	 *	Changes the current scheme.
	 *
	 *	@param	aScheme	the scheme to use as current scheme
	 */
	*set { arg aScheme;
		scheme = aScheme;
	}

	/**
	 *	Executes a function body, temporarily
	 *	setting the current GUI scheme. This is usefull inside
	 *	view's action functions in order to make this function
	 *	use the GUI scheme that was originally used for the
	 *	view of the action, even if the scheme has been switched meanwhile.
	 *
	 *	@param	scheme	the scheme to use during the function execution
	 *	@param	func		(Function) a body to execute
	 *	@return	the result of the function
	 */
	*use { arg aScheme, func;
		var recent	= scheme;
		scheme		= aScheme;
		^func.protect({ scheme = recent });
	}

	/**
	 *	Same as 'use' but using a scheme's id as first argument
	 *
	 *	@param	id	the id of the scheme to use during the function execution
	 *	@param	func		(Function) a body to execute
	 *	@return	the result of the function
	 */
	*useID { arg id, func;
		^this.use( schemes[ id.asSymbol ], func );
	}

	/**
	 *	Registers a new scheme. This is typically called
	 *	by external libraries in their startup procedure.
	 *	If a scheme with the same identifier (scheme.id)
	 *	exists, it is overwritten.
	 *
	 *	@param	aScheme	the scheme to add
	 */
	*add { arg aScheme;
		schemes.put( aScheme.id, aScheme );
		if( scheme.isNil, {				// first registration = first default kit
			scheme = aScheme;
		}, {
			scheme = schemes[ scheme.id ];	// in case we are updating an existing scheme
		});
	}

	/**
	 *	All method calls are mapped to the current
	 *	scheme, so that for example GUI.button can be used
	 *	and is delegated to the button association of the
	 *	current scheme.
	 */
	*doesNotUnderstand { arg selector ... args;
		if ( scheme.notNil ){
			^scheme.performList( selector, args );
		}{
			("No GUI scheme active: " + selector + args ).warn;
		}
	}

	/**
	 *	Add skins : GUI.skins.put(skinName,( fontSpecs: etc.  )  )
	 *	then set that as default here.
	 *  Note that if you wanted to do this in an *initClass that the scheme and GUI must be initialized
	 * and the scheme must be created in CocoaGUI.initClass (class varies on platform) so you cannot
	 * explicitly init that class by name since it varies across platforms.  so you can't really setSkin
	 * in an initClass. Your initClass method could do StartUp.add { GUI.setSkin(...) };
	 */
	*setSkin { arg skinName;
		skin = skins[skinName];
		scheme.font.new(*GUI.skin.fontSpecs).setDefault;
	}
}

GeneralHID{
	classvar <>all, <>maps, <>verbose=false;
	classvar <deviceList;
	classvar <scheme, <schemes;

	*initClass {
		schemes	= IdentityDictionary.new;
	}

	*new { arg key; ^scheme.perform( key )}

	*current {
		^scheme;
	}

	*get { arg id;
		^schemes[ id.asSymbol ];
	}

	*set { arg aScheme;
		scheme = aScheme;
	}

	*use { arg aScheme, func;
		var recent	= scheme;
		scheme		= aScheme;
		^func.protect({ scheme = recent });
	}

	*useID { arg id, func;
		^this.use( schemes[ id.asSymbol ], func );
	}

	*doesNotUnderstand { arg selector ... args;
		^scheme.perform( selector, args );
	}

	*add { arg aScheme;
		schemes.put( aScheme.id, aScheme );
		if( scheme.isNil, {				// first registration = first default kit
			scheme = aScheme;
		}, {
			scheme = schemes[ scheme.id ];	// in case we are updating an existing scheme
		});
	}

	*fromID { arg id;
		var newScheme = schemes[ id.asSymbol ];
		if( newScheme.notNil, {
			scheme = newScheme;
		}, {
			("GeneralHID.fromID : The HID scheme '" ++ id ++ "' is not installed\n" ++
			 "The current scheme is still '" ++ if( scheme.notNil, { scheme.id }) ++ "'!").warn;
		});
		^scheme;
	}

}

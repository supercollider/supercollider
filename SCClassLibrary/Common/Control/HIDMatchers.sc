// used for matching an HID device, matches should be passed an instance of HID
HIDProto {

	var <>id;

	var <>vendorID, <>productID;
	var <>path;
	var <>serialNumber;
	var <>vendorName, <>productName;
	var <>releaseNumber;
	var <>interfaceNumber;
	var <>usagePage, <>usage;

	var <>pageName, <>usageName;

	var <shouldMatch;

	*new {
		^super.new.init;
	}

	*newType { |uName, pName| // from HIDFunc.usage
		^super.new.init.addTypeMatch(uName, pName);
	}

	*newProduct { |pName, vName| // from HIDFunc.usageID, .device, .element
		^super.new.init.addProductMatch(pName, vName);
	}

	*newFromDict { |dict| // from any HIDFunc matcher
		^super.new.init.addDictionaryMatch( dict );
	}

	init {
		shouldMatch = Set[];
	}

	addTypeMatch { |uName, pName|
		if ( uName.notNil ) {
			shouldMatch.add( \usageName );
			usageName = uName;
		};
		if ( pName.notNil ) {
			shouldMatch.add( \pageName );
			pageName = pName;
		}
	}

	addProductMatch { |pName, vName|
		if ( pName.notNil ) {
			shouldMatch.add( \productName );
			productName = pName;
		};
		if ( vName.notNil ) {
			shouldMatch.add( \vendorName );
			vendorName = vName;
		}
	}

	addDictionaryMatch { |dict|
		dict.keysValuesArrayDo { |key, val|
			shouldMatch.add( key );
			this.performList( (key ++ "_").asSymbol, [ val ] );
		}
	}

	matches { |hid|
		var matches = true; // check all until a match is false
		var keysToMatch, keyMatches;
		var newShouldMatch;
		shouldMatch.copy.do { |key|
			if ( key != \id ) {
				keysToMatch = this.perform( key );
				if ( keysToMatch.isNil ) {
					shouldMatch = shouldMatch.reject { |it| it == key };
				} {
					if ( keysToMatch.isKindOf( Array ) ) {
						keyMatches = false;
						keysToMatch.do { |it|
							keyMatches = keyMatches or: ( it == hid.info.perform( key ) );
						};
						matches = matches and: keyMatches;
					} { // not an array
						matches = matches and: ( keysToMatch == hid.info.perform( key ) );
					}
				}
			} {
				matches = matches and: ( this.id.matchItem( hid.id ) );
			};
		};
		^matches;
	}

}

// used for matching an HID device element, matches should be passed an instance of HIDElement
HIDElementProto {

	var <id;
	var <ioType, <type;
	var <usagePage, <usage;
	var <usageMin, <usageMax;
	var <pageName, <usageName, <iotypeName, <typeSpec;

	var <shouldMatch;

	*new {
		^super.new.init;
	}

	*newType { |uName, pName| // from HIDFunc.usage
		^super.new.init.addTypeMatch(uName, pName);
	}

	*newTypeID { |uID, pID| // from HIDFunc.usage
		^super.new.init.addTypeIDMatch(uID, pID);
	}

	*newFromDict { |dict| // from any HIDFunc matcher
		^super.new.init.addDictionaryMatch( dict );
	}

	init {
		shouldMatch = Set[];
	}

	id_ { |argid|
		id = argid;
		shouldMatch.add( \id );
	}

	ioType_ { |argio|
		ioType = argio;
		shouldMatch.add( \ioType );
	}

	type_ { |argio|
		type = argio;
		shouldMatch.add( \type );
	}

	usagePage_ { |argup|
		usagePage = argup;
		shouldMatch.add( \usagePage );
	}

	usage_ { |argu|
		usage = argu;
		shouldMatch.add( \usage );
	}

	usageMin_ { |argup|
		usageMin = argup;
		shouldMatch.add( \usageMin );
	}

	usageMax_ { |argup|
		usageMax = argup;
		shouldMatch.add( \usageMax );
	}

	pageName_ { |argup|
		pageName = argup;
		shouldMatch.add( \pageName );
	}

	usageName_ { |argup|
		usageName = argup;
		shouldMatch.add( \usageName );
	}

	iotypeName_ { |argup|
		iotypeName = argup;
		shouldMatch.add( \iotypeName );
	}

	typeSpec_ { |argup|
		typeSpec = argup;
		shouldMatch.add( \typeSpec );
	}

	addTypeMatch { |uName, pName|
		if ( uName.notNil ) {
			this.usageName = uName;
		};
		if ( pName.notNil ) {
			this.pageName = pName;
		}
	}

	addTypeIDMatch { |uID, pID|
		if ( uID.notNil ) {
			this.usage = uID;
		};
		if ( pID.notNil ) {
			this.usagePage = pID;
		}
	}

	addDictionaryMatch { |dict|
		dict.keysValuesArrayDo { |key, val|
			this.performList( (key ++ "_").asSymbol, [ val ] );
		}
	}

	matches { |ele|
		var matches = true; // check all until a match is false
		shouldMatch.copy.do { |key|
			this.perform( key ).isNil {
				shouldMatch = shouldMatch.reject { |it| it == key };
			} {
				switch( key,
					\usageMin, { matches = matches and: ( ele.usage >= this.usageMin ) },
					\usageMax, { matches = matches and: ( ele.usage <= this.usageMax ) },
					 { // default
						matches = matches and: ( this.perform( key ).matchItem( ele.perform( key ) ) )
					}
				);
			};
		};
		^matches;
	}

}
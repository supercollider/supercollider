// used for matching an HID device, matches should be passed an instance of HID
HIDProto{

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

    *newType{ |uName,pName| // from HIDFunc.usage
        ^super.new.init.addTypeMatch(uName,pName);
    }

    *newProduct{ |pName,vName| // from HIDFunc.usageID, .device, .element
        ^super.new.init.addProductMatch(pName,vName);
    }

    *newFromDict{ |dict| // from any HIDFunc matcher
        ^super.new.init.addDictionaryMatch( dict );
    }

    init{
        shouldMatch = Set[];
    }

    addTypeMatch{ |uName, pName|
        if ( uName.notNil ){
            shouldMatch.add( \usageName );
            usageName = uName;
        };
        if ( pName.notNil ){
            shouldMatch.add( \pageName );
            pageName = pName;
        }
    }

    addProductMatch{ |pName, vName|
        if ( uName.notNil ){
            shouldMatch.add( \productName );
            productName = uName;
        };
        if ( pName.notNil ){
            shouldMatch.add( \vendorName );
            vendorName = pName;
        }
    }

    addDictionaryMatch{ |dict|
        dict.keysValuesArrayDo{ |key,val|
            shouldMatch.add( key );
            this.performList( (key ++ "_").asSymbol, [ val ] );
        }
    }

    matches{ |hid|
        matches = true; // check all until a match is false
        shouldMatch.do{ |key|
            if ( key != \id ){
                matches = matches and: ( hid.info.perform( key ) == this.perform( key ) );
            }{
                matches = matches and: ( hid.id == this.id );
            };
        };
        ^matches;
    }

}

// used for matching an HID device element, matches should be passed an instance of HIDElement
HIDElementProto{

    var <>id;
    var <>ioType, <>type;
    var <>usagePage, <>usage;
    var <>usageMin, <>usageMax;
    var <>pageName, <>usageName, <>iotypeName, <>typeSpec;

    var <shouldMatch;

    *newType{ |uName,pName| // from HIDFunc.usage
        ^super.new.init.addTypeMatch(uName,pName);
    }

    *newProduct{ |pName,vName| // from HIDFunc.usageID, .device, .element
        ^super.new.init.addProductMatch(pName,vName);
    }

    *newFromDict{ |dict| // from any HIDFunc matcher
        ^super.new.init.addDictionaryMatch( dict );
    }

    init{
        shouldMatch = Set[];
    }

    addTypeMatch{ |uName, pName|
        if ( uName.notNil ){
            shouldMatch.add( \usageName );
            usageName = uName;
        };
        if ( pName.notNil ){
            shouldMatch.add( \pageName );
            pageName = pName;
        }
    }

    addDictionaryMatch{ |dict|
        dict.keysValuesArrayDo{ |key,val|
            shouldMatch.add( key );
            this.performList( (key ++ "_").asSymbol, [ val ] );
        }
    }

    matches{ |ele|
        matches = true; // check all until a match is false
        shouldMatch.do{ |key|
            switch( key ){
                \usageMin, { matches = matches and: ( ele.usage >= this.usageMin ) },
                \usageMax, { matches = matches and: ( ele.usage <= this.usageMax ) },
                { // default
                    matches = matches and: ( ele.perform( key ) == this.perform( key ) ) }
            };
        };
        ^matches;
    }

}
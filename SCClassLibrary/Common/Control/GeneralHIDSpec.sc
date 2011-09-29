GeneralHIDSpec{
	classvar <>all,<folder;

	var <map, <device, <info;

	*initClass {
		folder = (Platform.userAppSupportDir +/+ "GeneralHIDSpecs").standardizePath;
		if ( this.checkSaveFolder ){ this.loadSavedInfo; };
		all	= all ? IdentityDictionary.new;
	}

	*loadSavedInfo{
		var filename;
		filename = (folder+/+"allspecs.info");
		if ( File.exists(filename) ){
			all = filename.load;
		}
	}

	*checkSaveFolder{
		^File.exists( folder );
	}

	*makeSaveFolder {
		var testfile, testname = "zzz_generalhid_test_delete_me.txt";
		folder = (Platform.userAppSupportDir +/+ "GeneralHIDSpecs").standardizePath;
		testfile = File(folder +/+ testname, "w");

		if (testfile.isOpen.not)
			{ folder.mkdir }
			{ testfile.close;  unixCmd("rm" + folder.escapeChar($ ) +/+ testname) }
	}

	*new { |dev|
		^super.new.init(dev);
	}

	init{ |dev|
		device = dev;
		map = IdentityDictionary.new;
		info = IdentityDictionary.new;
		info.put( \vendor, device.info.vendor );
		info.put( \name, device.info.name );
		info.put( \product, device.info.product );
		info.put( \version, device.info.version );
		info.put( \scheme, GeneralHID.scheme );
	}

	find{
		^all.select{ |thisspec,key|
			( thisspec[\vendor] == device.info.vendor ) and:
			( thisspec[\name] == device.info.name ) and:
			( thisspec[\product] == device.info.product ) and:
			( thisspec[\version] == device.info.version ) and:
			( thisspec[\scheme] == GeneralHID.current )
		}.keys.asArray;
	}

	add{ |key, slot|
		map.put( key, slot );
		this.at( key ).key = key;
	}

	// returns the slot
	at{ |key|
		var id1,id2;
		id1 = map.at(key)[0];
		id2 = map.at(key)[1];
		^device.slots[id1][id2];
		// map.at(key)
	}

	value{ |key|
		^this.at(key).value;
	}

	value_{ |key,value|
		var slot;
		slot = this.at(key);
		slot.value_(value);
		^slot;
	}

	action_{ |key,action|
		var slot;
		slot = this.at(key);
		slot.action_(action);
		^slot;
	}

	bus{ |key|
		^this.at(key).bus;
	}

	createBus{ |name,server|
		this.at( name ).createBus( server );
	}

	freeBus{ |name|
		this.at( name ).freeBus;
	}

	/*	setAllActions{ |action|
		map.do{ |it|
			device.slots.at( it[0] ).at( it[1] ).action_( action );
		};
		}*/

	createAllBuses{ |server|
		map.do{ |it|
			device.slots.at( it[0] ).at( it[1] ).createBus( server );
		};
	}

	freeAllBuses{
		map.do{ |it|
			device.slots.at( it[0] ).at( it[1] ).freeBus;
		};
	}

	save{ |name|
		var file, res = false;
		var filename;
		all.put( name.asSymbol, info );
		if (  GeneralHIDSpec.checkSaveFolder.not ) {  GeneralHIDSpec.makeSaveFolder };
		filename = folder +/+ name ++ ".spec";
		file = File(filename, "w");
		if (file.isOpen) {
			res = file.write(map.asCompileString);
			file.close;
		};
		this.class.saveAll;
		^res;
	}

	*saveAll{
		var file, res = false;
		var filename;
		if ( GeneralHIDSpec.checkSaveFolder.not ) {  GeneralHIDSpec.makeSaveFolder };
		filename = folder +/+ "allspecs.info";
		file = File(filename, "w");
		if (file.isOpen) {
			res = file.write(all.asCompileString);
			file.close;
		};
		^res;
	}

	fromFile { |name|
		if (  GeneralHIDSpec.checkSaveFolder.not ) {  GeneralHIDSpec.makeSaveFolder };
		map = (folder +/+ name++".spec").load;
		map.keysValuesDo{ |key,it|
			this.at( key ).key = key;
		}
	}

}

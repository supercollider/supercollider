OSXPlatform : UnixPlatform
{
	initPlatform {
		super.initPlatform;
		recordingsDir = "~/Music/SuperCollider Recordings".standardizePath;
		this.declareFeature(\findHelpFile); // Announce that we have our own way of finding helpfiles
	}

	name { ^\osx }

	startupFiles {
		var filename = "startup.rtf";
		^[this.systemAppSupportDir +/+ filename, this.userAppSupportDir +/+ filename];
	}
	
	startup {
		Document.implementationClass.startup;
		// make server window
		Server.internal.makeWindow;
		Server.local.makeWindow;
		this.loadStartupFiles;
		Help.addToMenu;
	}
	shutdown {
		HIDDeviceService.releaseDeviceList;
		CocoaMenuItem.clearCustomItems;
	}
	
		// only osx uses Cocoa guis
	defaultGUIScheme { ^\cocoa }
	defaultHIDScheme { ^\osx_hid }
	
	recompile { _Recompile }

	findHelpFile { | string |
		_Cocoa_HelpFileForString_
		^this.primitiveFailed
	}
}


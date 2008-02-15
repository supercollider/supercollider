OSXPlatform : UnixPlatform
{
	initPlatform {
		super.initPlatform;
		recordingsDir = "~/Music/SuperCollider Recordings".standardizePath;
	}

	name { ^\osx }

	startupFiles {
		var filename = "startup.rtf";
		^[this.systemAppSupportDir +/+ filename, this.userAppSupportDir +/+ filename];
	}
	
	startup {
		Document.implementationClass.startup;
		// make server window
		// Server.internal.makeWindow;
		Server.local.makeWindow;
		this.loadStartupFiles;
	}
	shutdown {
		HIDDeviceService.releaseDeviceList;
	}
	
		// only osx uses Cocoa guis
	defaultGUIScheme { ^\cocoa }
	defaultHIDScheme { ^\osx_hid }
}

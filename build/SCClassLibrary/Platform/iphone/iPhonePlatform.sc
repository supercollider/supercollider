IPhonePlatform : UnixPlatform
{
	initPlatform {
		super.initPlatform;
	}

	name { ^\iphone }

	startupFiles {
		var filename = "startup.rtf";
		^[this.systemAppSupportDir +/+ filename];
	}

	startup {
		"starting up iPhone Platform !".postln;
		PathName.tmp_("~/Documents/tmp/".standardizePath);
		//Document.implementationClass.startup;
		// make server window
		Server.internal.recSampleFormat = "int16";
		Server.internal.makeWindow;
		Server.internal.options.hardwareBufferSize = 256;
		Server.internal.options.blockSize = 256;
		Server.default = Server.internal;
		//Server.local.makeWindow;
		this.loadStartupFiles;
	}
	shutdown {
		//HIDDeviceService.releaseDeviceList;
	}

		// only osx uses Cocoa guis
	defaultGUIScheme { ^\iphone }
	defaultHIDScheme { ^\osx_hid }
}

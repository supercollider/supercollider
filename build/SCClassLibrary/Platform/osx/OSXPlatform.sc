OSXPlatform : UnixPlatform
{
	name { ^\osx }
	startupFiles { ^#["~/scwork/startup.rtf"] }
	startup {
		Document.startup;
		// make server window
		Server.internal.makeWindow;
		Server.local.makeWindow;
		this.loadStartupFiles;
	}
	shutdown {
		HIDDeviceService.releaseDeviceList;
	}
}

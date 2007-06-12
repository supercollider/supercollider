OSXPlatform : UnixPlatform
{
	name { ^\osx }
	startup {
		Document.startup;
		// make server window
		Server.internal.makeWindow;
		Server.local.makeWindow;
		"~/scwork/startup.rtf".loadPaths;
	}
	shutdown {
		HIDDeviceService.releaseDeviceList;
	}
}

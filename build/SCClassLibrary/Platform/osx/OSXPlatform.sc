OSXPlatform : UnixPlatform
{
	name { ^\osx }
	systemExtensionDir { ^"/usr/local/share/SuperCollider/Extensions" }
	userExtensionDir { ^"~/share/SuperCollider/Extensions".standardizePath }
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

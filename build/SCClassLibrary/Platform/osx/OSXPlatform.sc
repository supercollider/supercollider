OSXPlatform : UnixPlatform
{
	name { ^\osx }
	systemAppSupportDir { ^"/Library/Application Support/SuperCollider" }
	userAppSupportDir { ^"~/Library/Application Support/SuperCollider".standardizePath }
	systemExtensionDir { ^"/Library/Application Support/SuperCollider/Extensions" }
	userExtensionDir { ^"~/Library/Application Support/SuperCollider/Extensions".standardizePath }
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

OSXPlatform : UnixPlatform
{
	var <>preferencesAction;
	var <>sleepAction, <>wakeAction, <>isSleeping=false;

	initPlatform {
		super.initPlatform;
		recordingsDir = "~/Music/SuperCollider Recordings".standardizePath;
		this.declareFeature(\findHelpFile); // Announce that we have our own way of finding helpfiles
		this.declareFeature(\unixPipes); // pipes are possible (can't declare in UnixPlatform since IPhonePlatform is unixy yet can't support pipes)
	}

	name { ^\osx }

	startupFiles {
		var filename = "startup.rtf";
		^[this.systemAppSupportDir +/+ filename, this.userAppSupportDir +/+ filename];
	}

	startup {
		if(Platform.ideName == "scapp"){
			Document.implementationClass.startup;
			// make server window
			Server.internal.makeWindow;
			Server.local.makeWindow;
		};
		this.loadStartupFiles;
		if(Platform.ideName == "scapp"){
			Help.addToMenu;
		};
	}
	shutdown {
		HIDDeviceService.releaseDeviceList;
		if(Platform.ideName == "scapp"){
			CocoaMenuItem.clearCustomItems;
		};
	}

		// only osx uses Cocoa guis
	defaultGUIScheme { ^\cocoa }
	defaultHIDScheme { ^\osx_hid }

	recompile { _Recompile }

	escapeWindow {
		var currentAppModal, currentFullScreen;
		currentAppModal = SCModalWindow.current;
		currentAppModal.notNil.if({
			currentAppModal.close;
			"An instance of SCModalWindow was aborted".warn;
		});
		GUI.window.allWindows.do({|window|
			window.isKindOf(SCModalSheet).if({
				window.close;
				"An instance of SCModalSheet was aborted".warn;
			});

		});
		currentFullScreen = SCWindow.currentFullScreen;
		currentFullScreen.notNil.if({currentFullScreen.endFullScreen; currentFullScreen.close});
	}

	exitFullScreen {
		var currentFullScreen;
		currentFullScreen = SCWindow.currentFullScreen;
		currentFullScreen.notNil.if({currentFullScreen.endFullScreen;});
	}

	findHelpFile { | string |
		_Cocoa_HelpFileForString_
		^this.primitiveFailed
	}
}


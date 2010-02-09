+ OSXPlatform {
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
}

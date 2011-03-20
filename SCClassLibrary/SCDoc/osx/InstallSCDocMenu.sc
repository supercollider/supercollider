// install a menu to access scdoc
SCDocGlue {
	*initClass {
		var scDocMenu;
		if (thisProcess.platform.class.name == 'OSXPlatform') {
			StartUp.add({	
				if (GUI.scheme.name == \QtGUI) {
					scDocMenu = SCMenuGroup(nil, "SCDoc", 10);
					SCMenuItem(scDocMenu, "browse")
						.action_({ var path;
							path = thisProcess.platformClass.userAppSupportDir ++ 
									"/Help/Browse.html";
							QHelpBrowser.new.front.load(path) });
					SCMenuItem(scDocMenu, "help")
						.action_({var path;
							path = "Help/Help.html".standardizePath;
							QHelpBrowser.new.front.load(path) });
				};
				~scDocMenu = scDocMenu;
			})
		}
	}
}


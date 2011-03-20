// install a menu to access scdoc
InstallSCDocMenu {
	*initClass {
		var scDocMenu;
		if (thisProcess.platform.class.name == 'OSXPlatform') {
			StartUp.add({	
				if (GUI.scheme.name == \QtGUI) {
					scDocMenu = SCMenuGroup(nil, "SCDoc", 10);
					SCMenuItem(scDocMenu, "help")
					.action_({ 
					QHelpBrowser.newUnique.front.load(SCDoc.helpTargetDir ++ "/Help.html") });
					SCMenuItem(scDocMenu, "browse")
					.action_({ 
					QHelpBrowser.newUnique.front.load(SCDoc.helpTargetDir ++ "/browse.html") });
				};
				~scDocMenu = scDocMenu;
			})
		}
	}
}


// install a menu to access scdoc
InstallSCDocMenu {
	*initClass {
		var scDocMenu;
		if (thisProcess.platform.class.name == 'OSXPlatform') {
			StartUp.add({	
				if (\SCMenuGroup.asClass.notNil) {
					
					scDocMenu = SCMenuGroup(nil, "SCDoc", 10);
					SCMenuItem(scDocMenu, "help")
					.action_({ 
						if (GUI.scheme.name == \QtGUI) {
							QHelpBrowser.newUnique.front.load(SCDoc.helpTargetDir ++ "/Help.html")
						};
						if (GUI.scheme.name == \CocoaGUI) {
							Document.open(SCDoc.helpTargetDir ++ "/Help.html")
						};
					});
					SCMenuItem(scDocMenu, "browse")
					.action_({ 
						if (GUI.scheme.name == \QtGUI) {
							QHelpBrowser.newUnique.front.load(SCDoc.helpTargetDir ++ "/browse.html")
						};
						if (GUI.scheme.name == \CocoaGUI) {
							Document.open(SCDoc.helpTargetDir ++ "/browse.html")
						};
					});
				};
					
			});
		}
	}
}


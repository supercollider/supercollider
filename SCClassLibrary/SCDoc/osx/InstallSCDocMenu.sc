// install a menu to access scdoc
InstallSCDocMenu {
	*makeUrl {|path|
		var url = path;
		if(url.contains("://")) {
			^url;
		} {
			^"file://"++url;
		};
	}
	*openFile {|path,ref|
		var url = this.makeUrl(path);
		if(ref.notNil) {
			url = url ++ "#" ++ ref;
		};
		switch(GUI.scheme.name,
			\QtGUI, {QHelpBrowser.newUnique.front.load(url)},
			\CocoaGUI, {("open"+url).systemCmd}
		);
	}
	*initClass {
		var scDocMenu;
		if (thisProcess.platform.class.name == 'OSXPlatform') {
			StartUp.add({
				if (\SCMenuGroup.asClass.notNil) {
					scDocMenu = SCMenuGroup(nil, "SCDoc", 10);

					SCMenuItem(scDocMenu, "Help (for selected text)")
					.action_({
						if(Document.current.selectionSize > 0) {
							this.openFile(SCDoc.findClassOrMethod(Document.current.selectedString));
						} {
							this.openFile(SCDoc.helpTargetDir+/+"Help.html");
						};
					})
					.setShortCut("d",false,true);

					SCMenuItem(scDocMenu, "Search (for selected text)")
					.action_({
						this.openFile(SCDoc.helpTargetDir+/+"Search.html",
							if(Document.current.selectionSize > 0,
								{Document.current.selectedString},nil));
					})
					.setShortCut("s",false,true);

					SCMenuItem(scDocMenu, "Browse")
					.action_({
						this.openFile(SCDoc.helpTargetDir+/+"Browse.html");
					})
					.setShortCut("b",false,true);

					SCMenuItem(scDocMenu, "Update help")
					.action_({
					    SCDoc.updateAll;
					});
				};
			});
		}
	}
}


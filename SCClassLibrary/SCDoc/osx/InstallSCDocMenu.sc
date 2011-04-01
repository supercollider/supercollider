// install a menu to access scdoc
InstallSCDocMenu {
	*openFile {|url,ref|
		if(ref.notNil) {
			url = url ++ "#" ++ ref;
		};
		HelpBrowser.instance.goTo(url).window.front;
	}
	*initClass {
		var scDocMenu;
		if (thisProcess.platform.class.name == 'OSXPlatform') {
			StartUp.add({
				if (\SCMenuGroup.asClass.notNil) {
					scDocMenu = SCMenuGroup(nil, "SCDoc", 10);

					SCMenuItem(scDocMenu, "Help (for selected text)")
					.action_({
					    var sz = Document.current.selectionSize;
					    var txt = Document.current.selectedString;
					    this.openFile(SCDoc.findHelpFile(txt));
					})
					.setShortCut("d",false,true);

					SCMenuItem(scDocMenu, "Search (for selected text)")
					.action_({
					    var sz = Document.current.selectionSize;
					    var txt = Document.current.selectedString;
					    this.openFile(SCDoc.helpTargetDir+/+"Search.html", if(sz>0,{txt},nil));
					})
					.setShortCut("s",false,true);

					SCMenuItem(scDocMenu, "Browse")
					.action_({
						this.openFile(SCDoc.helpTargetDir+/+"Browse.html");
					})
					.setShortCut("b",false,true);
				};
			});
		}
	}
}


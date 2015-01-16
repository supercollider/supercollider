
QuarksGui {

	var model, quarks, selectedQuark;

	var window,
		treeView,
		quarkRows,
		infoView,
		palette,
		lblMsg,
		btnRecompile;

	*new { ^super.new.init }

	init {
		var bounds;
		var btnUpdateDirectory, btnQuarksHelp, btnOpenFolder,
			lblCaption, lblExplanation,
			btnQuarkHelp, btnQuarkOpen, btnQuarkClasses,
			btnQuarkMethods, txtDescription, btnCloseDetails,
			gizmo;

		model = Quarks;
		// model.addDependent(this);
		palette = GUI.current.palette;

		bounds = Window.flipY(Window.availableBounds);
		window = Window("Quarks", Rect(0, 0, 700, bounds.height * 0.9).center_(bounds.center));

		lblCaption = StaticText().font_(GUI.font.new(size:16, usePointSize:true)).string_("Quarks");

		btnUpdateDirectory = Button()
			.states_([["Refresh Quarks directory"]])
			.toolTip_("Download directory listing from" + Quarks.directoryUrl)
			.action_({
				treeView.enabled = false;
				this.setMsg("Fetching" + Quarks.directoryUrl, \yellow);
				AppClock.sched(0.2, {
					protect {
						model.fetchDirectory(true);
					} {
						treeView.enabled = true;
						this.setMsg("Quarks directory has been updated.", \green);
						this.update;
					}
				});
				nil
			});

		btnQuarksHelp = Button().states_([["Quarks Help"]])
			.toolTip_("Open Quarks documentation")
			.action_({ HelpBrowser.openBrowsePage("Quarks") });

		btnOpenFolder = Button().states_([["Open Quarks Folder"]])
			.toolTip_("Open" + model.folder)
			.action_({ model.openFolder });

		btnRecompile = Button().states_([["Recompile class library"]])
			.toolTip_("You will need to recompile the class library after making any changes")
			.action_({ thisProcess.recompile })
			.enabled_(false);

		lblMsg = StaticText().font_(GUI.font.new(size:12, usePointSize:true));

		treeView = TreeView()
			.setProperty(\rootIsDecorated, false)
			.columns_(["Install", "Name", "Summary"])
			.itemPressedAction_({ |v|
				infoView.visible = true;
			})
			.onItemChanged_({ |v|
				var curItem, curView, isInstalled;
				curItem = v.currentItem;
				selectedQuark = nil;
				if(curItem.notNil) {
					// update
					curView = quarkRows.values().detect({ |view| view.treeItem == curItem });
					if(curView.notNil) {
						selectedQuark = curView.quark;
						txtDescription.string = QuarksGui.descriptionForQuark(selectedQuark);
						btnQuarkOpen.enabled = selectedQuark.isDownloaded;
						isInstalled = selectedQuark.isInstalled;
						btnQuarkClasses.enabled = isInstalled;
						btnQuarkMethods.enabled = isInstalled;
					}
				}{
					infoView.visible = false
				}
			});

		txtDescription = TextView(bounds:10@10)
			.font_(GUI.font.new(size:10, usePointSize:true))
			.tabWidth_(15)
			.autohidesScrollers_(true)
			.hasVerticalScroller_(true)
			.editable_(false)
			//.minSize_(Size(0,0));
			.minHeight_(50);

		btnQuarkHelp = Button()
			.states_([["Help"]])
			.toolTip_("Show help for this Quark")
			.action_({
				selectedQuark.help
			});

		btnQuarkOpen = Button()
			.states_([["Open Folder"]])
			.toolTip_("Open source folder for this quark")
			.action_({
				selectedQuark.localPath.openOS;
			});

		btnQuarkClasses = Button()
			.states_([["Classes"]])
			.toolTip_("Show classes defined by this quark")
			.enabled_(false)
			.action_({
				var cls = selectedQuark.definesClasses;
				var tree, item, buts = [
					Button().states_([["Browse"]]).action_({
						cls[item.index].browse;
					}),
					Button().states_([["Help"]]).action_({
						cls[item.index].help;
					}),
					Button().states_([["Open File"]]).action_({
						cls[item.index].openCodeFile;
					})
				];
				buts.do(_.enabled_(false));
				Window("% Classes".format(selectedQuark.name)).layout_(
					VLayout(
						tree = TreeView()
							.setProperty(\rootIsDecorated, false)
							.columns_(["Classes"])
							.onItemChanged_({|v| item = v.currentItem}),
						HLayout(*buts)
					)
				).front;
				if(cls.size > 0) {
					cls.do {|c| tree.addItem([c.name.asString])};
					tree.itemPressedAction = { buts.do(_.enabled_(true)) }
				} {
					tree.addItem(["No classes"]);
				};
				tree.invokeMethod(\resizeColumnToContents, 0);
			});

		btnQuarkMethods = Button()
			.states_([["Extension methods"]])
			.toolTip_("Show extension methods defined in this quark that overwrite methods in the common library")
			.enabled_(false)
			.action_({
				var mets = selectedQuark.definesExtensionMethods;
				var tree, item, buts = [
					Button().states_([["Browse"]]).action_({
						mets[item.index].ownerClass.browse;
					}),
					Button().states_([["Help"]]).action_({
						mets[item.index].help;
					}),
					Button().states_([["Source"]]).action_({
						mets[item.index].openCodeFile;
					})
				];
				buts.do(_.enabled_(false));
				Window("% Extension Methods".format(selectedQuark.name)).layout_(
					VLayout(
						tree = TreeView()
							.setProperty(\rootIsDecorated, false)
							.columns_(["Class", "Method"])
							.onItemChanged_({|v| item = v.currentItem }),
						HLayout(*buts)
					)
				).front;
				if(mets.size > 0) {
					mets.collect { |m|
						var x = m.ownerClass.name;
						tree.addItem(if(x.isMetaClassName) {[x.asString[5..], "*" ++ m.name]} {[x.asString, "-" ++ m.name]});
					};
					tree.itemPressedAction = { buts.do(_.enabled_(true)) }
				} {
					tree.addItem([nil,"No extension methods"]);
				};
				tree.invokeMethod(\resizeColumnToContents, 0);
				tree.invokeMethod(\resizeColumnToContents, 1);
			});

		// TODO
		// open homepage or github
		// checkout if not yet downloaded
		// update
		//
		// switch versions

		btnCloseDetails = StaticText()
			.string_("X")
			.align_(\center)
			.toolTip_("Close detail view")
			.mouseDownAction_({
				infoView.visible = false;
			});
		gizmo = btnCloseDetails.sizeHint;
		gizmo.width = gizmo.width + 20;
		btnCloseDetails.fixedSize = gizmo;

		infoView = View();
		infoView.layout = VLayout(
			HLayout(btnQuarkHelp, btnQuarkOpen, btnQuarkClasses, btnQuarkMethods, btnCloseDetails, nil).margins_(0),
			txtDescription
		).spacing_(0).margins_(0);
		infoView.visible = false;

		window.layout =
			VLayout(
				lblCaption,
				HLayout(btnUpdateDirectory, btnOpenFolder, btnQuarksHelp, btnRecompile, nil),
				lblMsg,
				HLayout([lblExplanation, s:1]).margins_(0),
				[treeView, s:5],
				[infoView, s:2]
			);

		quarkRows = Dictionary.new;
		this.update;
		window.front;
	}
	update {
		var recompile = false;
		treeView.canSort = false;
		model.all.do({ |quark|
			var qrv;
			qrv = quarkRows.at(quark.name);
			if(qrv.isNil, {
				quarkRows[quark.name] = QuarkRowView(treeView, quark, this);
			}, {
				qrv.update;
				if(qrv.quark.changed, { recompile = true });
			});
		});
		treeView.canSort = true;
		treeView.sort(1);
		treeView.invokeMethod(\resizeColumnToContents, 0);
		treeView.invokeMethod(\resizeColumnToContents, 1);
		btnRecompile.enabled = recompile;
	}
	setMsg { |msg, color|
		lblMsg.background = palette.button.blend(Color.perform(color), 0.2);
		lblMsg.string = msg;
	}
	*descriptionForQuark { |quark|
		var lines, dependencies;
		lines = [
			quark.asString,
			"downloaded:" + quark.isDownloaded,
			"installed:" + quark.isInstalled,
			"path:" + quark.localPath,
			"url:" + quark.url
		];
		quark.data.keysValuesDo({ |k, v|
			if(k !== \name) {
				lines = lines.add(k.asString ++ ":" + v.asString);
			}
		});
		dependencies = quark.dependencies;
		if(dependencies.notEmpty) {
			lines = lines ++ ["Dependencies:"] ++ dependencies.collect(_.asString);
		};
		lines = lines ++ [
			Char.nl,
			quark.summary
		];
		^lines.join(Char.nl);
	}
}


QuarkRowView {

	var <quark, <treeItem, quarksGui;
	var btn;

	*new { |parent, quark, quarksGui|
		^super.new.init(parent, quark, quarksGui)
	}

	init { |parent, aQuark, qg|
		quark = aQuark;
		quarksGui = qg;

		btn = Button().fixedSize_(Size(20, 20));
		treeItem = parent.addItem([
			nil,
			"",
			""
		]).setView(0, btn);

		btn.action = { |btn|
			if(btn.value > 0, {
				quark.install
			}, {
				quark.uninstall
			});
			quarksGui.update;
		};
		this.update;
	}

	update {
		var palette = GUI.current.tryPerform(\palette),
			c = palette !? {palette.button},
			green = c.notNil.if({Color.green.blend(c, 0.5)}, {Color.green(1, 0.5)}),
			grey = c.notNil.if({Color.grey.blend(c, 0.5)}, {Color.grey(1, 0.5)});

		btn.states = [
			if(quark.isDownloaded, {
				["+", nil, grey]
			}, {
				["+", nil, nil]
			}),
			["✓", nil, green],
		];

		btn.value = quark.isInstalled.binaryValue;

		treeItem.setString(1, quark.name ? "");
		treeItem.setString(2, (quark.summary !? { quark.summary.replace("\n"," ").replace("\t","") }) ? "");
	}
}

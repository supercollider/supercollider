
QuarksGui {

	var model,
		quarks,
		window,
		treeView,
		quarkRows,
		palette,
		lblMsg,
		btnRecompile,
		detailView;

	*new { ^super.new.init }

	init {
		var bounds,
			btnUpdateDirectory,
			btnQuarksHelp,
			btnOpenFolder,
			lblCaption,
			btnClear,
			btnLoad,
			btnSave;

		model = Quarks;
		palette = GUI.current.palette;

		bounds = Window.flipY(Window.availableBounds);
		window = Window("Quarks", Rect(0, 0, min(bounds.width * 0.75, 1000), bounds.height * 0.9).center_(bounds.center));

		lblCaption = StaticText().font_(GUI.font.new(size:16, usePointSize:true)).string_("Quarks");

		btnUpdateDirectory = Button()
			.states_([["Refresh Quarks directory"]])
			.toolTip_("Download directory listing from" + Quarks.directoryUrl + "(auto-refreshes every 4 hours)")
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

		btnClear = Button().states_([["Uninstall all"]])
			.toolTip_("Uninstall all")
			.action_({ model.clear; this.update; });

		btnLoad = Button().states_([["Load Quarks file"]])
			.toolTip_("Install a set of quarks from a file")
			.action_({
				Dialog.openPanel({ |path|
					model.load(path);
					this.update;
				})
			});

		btnSave = Button().states_([["Save Quarks file"]])
			.toolTip_("Save currently installed quarks to a file")
			.action_({
				Dialog.savePanel({ |path|
					model.save(path)
				})
			});

		btnRecompile = Button().states_([
				["Recompile class library"],
				["Recompile class library", Color.black, Color.yellow]
			])
			.toolTip_("You will need to recompile the class library after making any changes")
			.action_({ thisProcess.platform.recompile })
			.enabled_(false);

		lblMsg = StaticText().font_(GUI.font.new(size:12, usePointSize:true));

		treeView = TreeView()
			.setProperty(\rootIsDecorated, false)
			.columns_(["", "", "Name", "Version", "Summary"])
			.itemPressedAction_({ |v|
				detailView.visible = true;
			})
			// open detail view
			.onItemChanged_({ |v|
				var curItem, curView;
				curItem = v.currentItem;
				if(curItem.notNil, {
					curView = quarkRows.values().detect({ |view| view.treeItem == curItem });
					if(curView.notNil, {
						detailView.model = curView.quark;
					}, {
						detailView.model = nil;
					})
				}, {
					detailView.model = nil
				})
			});

		detailView = QuarkDetailView.new;

		window.layout =
			VLayout(
				lblCaption,
				HLayout(btnUpdateDirectory, btnOpenFolder, btnQuarksHelp, btnClear, btnSave, btnLoad, btnRecompile, nil),
				lblMsg,
				[treeView, s:5],
				[detailView.makeView(this), s:2]
			);

		quarkRows = Dictionary.new;
		this.update;
		window.front;
	}
	update {
		var recompile = false;
		treeView.canSort = false;
		model.all.do({ |quark|
			var qrv, lp = quark.localPath;
			qrv = quarkRows.at(lp);
			if(qrv.isNil, {
				quarkRows[lp] = QuarkRowView(treeView, quark, this);
			}, {
				qrv.update;
				if(qrv.quark.changed, { recompile = true });
			});
		});
		treeView.canSort = true;
		treeView.sort(1, true);
		treeView.invokeMethod(\resizeColumnToContents, 0);
		treeView.invokeMethod(\resizeColumnToContents, 1);
		treeView.invokeMethod(\resizeColumnToContents, 2);
		treeView.invokeMethod(\resizeColumnToContents, 3);
		btnRecompile.enabled = recompile;
		btnRecompile.value = recompile.if(1, 0);
		detailView.update();
	}

	setMsg { |msg, color|
		lblMsg.background = palette.button.blend(Color.perform(color ? 'yellow'), 0.2);
		lblMsg.string = msg;
	}
}


QuarkDetailView {

	var <model,
		view,
		selectVersion,
		btnMethods,
		txtDescription,
		btnClose,
		btnHelp,
		btnOpenFolder,
		btnClasses,
		btnOpenWebpage,
		btnOpenGithub,
		btnCheckout;

	makeView { |quarksGui|
		var xSizeHint;
		txtDescription = TextView(bounds:10@10)
			.font_(Font(size:11, usePointSize:true))
			.tabWidth_(15)
			.autohidesScrollers_(true)
			.hasVerticalScroller_(true)
			.editable_(false)
			.minHeight_(50);

		btnHelp = Button()
			.states_([["Help"]])
			.action_({
				model.help
			});

		btnOpenFolder = Button()
			.states_([["Open Folder"]])
			.action_({
				model.localPath.openOS;
			});

		btnOpenWebpage = Button()
			.states_([["Open Webpage"]])
			.action_({
				var url = model.data['url'] ? model.url;
				if(url.notNil, {
					if(url.beginsWith("git:"), {
						url = "https:" ++ url.copyToEnd(4)
					});
					openOS(url);
				});
			});

		btnOpenGithub = Button()
			.states_([["Github"]])
			.action_({
				var url = model.url;
				if(url.notNil, {
					if(url.beginsWith("git:"), {
						url = "https:" ++ url.copyToEnd(4)
					});
					openOS(url);
				});
			});

		selectVersion = PopUpMenu();

		btnCheckout = Button()
			.states_([["Checkout"]])
			.action_({
				var refspec = selectVersion.items.at(selectVersion.value ? -1),
					ok=true;
				if(model.isInstalled, {
					// reinstall possibly with different dependencies
					model.uninstall;
					ok = Quarks.install(model.url, refspec);
				}, {
					model.refspec = refspec;
					model.checkout;
				});
				this.update;
				if(ok, {
					quarksGui.setMsg(model.name + "has checked out" + model.version, \green);
				}, {
					quarksGui.setMsg(model.name + "could not checkout" + model.version, \yellow);
				});
			});

		btnClasses = Button()
			.states_([["Classes"]])
			.toolTip_("Show classes defined by this quark")
			.enabled_(false)
			.action_({
				var cls = model.definesClasses;
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
				Window("% Classes".format(model.name)).layout_(
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

		btnMethods = Button()
			.states_([["Extension methods"]])
			.toolTip_("Show extension methods defined in this quark that overwrite methods in the common library")
			.enabled_(false)
			.action_({
				var mets = model.definesExtensionMethods;
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
				Window("% Extension Methods".format(model.name)).layout_(
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
						var x = m.ownerClass.name,
							it = if(x.isMetaClassName,
								{[x.asString[5..], "*" ++ m.name]},
								{[x.asString, "-" ++ m.name]});
						tree.addItem(it);
					};
					tree.itemPressedAction = { buts.do(_.enabled_(true)) }
				} {
					tree.addItem([nil,"No extension methods"]);
				};
				tree.invokeMethod(\resizeColumnToContents, 0);
				tree.invokeMethod(\resizeColumnToContents, 1);
			});

		btnClose = StaticText()
			.string_("X")
			.align_(\center)
			.toolTip_("Close detail view")
			.mouseDownAction_({
				this.visible = false;
			});
		xSizeHint = btnClose.sizeHint;
		xSizeHint.width = xSizeHint.width + 20;
		btnClose.fixedSize = xSizeHint;

		view = View();
		view.layout = VLayout(
			HLayout(btnOpenWebpage, btnOpenGithub, btnHelp,
				btnOpenFolder, btnClasses, btnMethods,
				selectVersion, btnCheckout, btnClose,
				nil).margins_(0),
			txtDescription
		).spacing_(0).margins_(0);
		view.visible = false;
		^view
	}
	update {
		var tags, refspec, isInstalled = false, isDownloaded = false, url;
		if(model.notNil, {
			txtDescription.string = this.descriptionForQuark(model) ? "";
			isInstalled = model.isInstalled;
			isDownloaded = model.isDownloaded;
			url = model.url;
			// if webpage is different than the github url
			btnOpenWebpage.enabled = model.data['url'] != url and: {url.notNil};
			btnOpenGithub.enabled = url.notNil;
			btnClasses.enabled = isInstalled;
			btnMethods.enabled = isInstalled;
			btnHelp.enabled = isInstalled or: {
				isDownloaded and: {model.data['helpdoc'].isString}
			};
			btnOpenFolder.enabled = isDownloaded or: isInstalled;
			btnCheckout.enabled = model.url.notNil;

			if(model.git.isNil, {
				selectVersion.items = [];
				selectVersion.enabled = false;
			}, {
				tags = model.tags.collect({ |t| "tags/" ++ t });
				refspec = model.git.refspec;
				if(tags.indexOfEqual(refspec).isNil, {
					tags = tags.add(refspec);
				});
				// if model.git.remoteLatest != current
				// show that you can update
				tags = tags.add("HEAD");
				if(model.git.isDirty, {
					selectVersion.enabled = false;
					refspec = "DIRTY";
					tags = tags.add("DIRTY");
				}, {
					selectVersion.enabled = true;
				});
				selectVersion.items = tags;
				selectVersion.value = tags.indexOfEqual(refspec);
			});
			view.visible = true;
		}, {
			view.visible = false;
		});
	}
	model_ { |quark|
		model = quark;
		this.update;
	}
	visible_ { |bool|
		view.visible = bool;
	}
	descriptionForQuark { |quark|
		var lines,
			dependencies,
			isInstalled = quark.isInstalled,
			isDownloaded = quark.isDownloaded;
		lines = [
			quark.name,
			"url:" + quark.url,
			"installed:" + isInstalled,
			if(quark.url.notNil, { "downloaded:" + isDownloaded }),
			if(isDownloaded or: isInstalled, { "path:" + quark.localPath })
		];
		if(isInstalled and: { File.exists(quark.localPath).not }, {
			lines = lines.add("ERROR: path does not exist");
		});

		quark.data.keysValuesDo({ |k, v|
			if([\name, \summary, \url, \path, \dependencies].includes(k).not) {
				lines = lines.add(k.asString ++ ":" + v.asString);
			}
		});
		dependencies = quark.dependencies;
		if(dependencies.notEmpty) {
			lines = lines ++ [Char.nl, "Dependencies:"] ++ dependencies.collect(_.asString);
		};
		if(quark.summary.notNil, {
			lines = lines ++ [
				Char.nl,
				quark.summary
			];
		});
		^lines.select(_.notNil).join(Char.nl);
	}
}


QuarkRowView {

	var <quark, <treeItem, quarksGui, btn;

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
			"",
			"",
			""
		]).setView(0, btn);

		btn.action = { |btn|
			this.install(btn.value > 0);
		};
		this.update;
	}
	install { |bool|
		var prev, ok;
		if(bool, {
			// if any other is installed by same name
			// then just uinstall that first
			prev = Quarks.installed.detect({ |q| q.name == quark.name });
			if(prev.notNil, {
				"Uninstalling other version % %".format(prev, prev.localPath).warn;
				prev.uninstall;
			});
			ok = quark.install;
			if(ok, {
				quarksGui.setMsg("Installed" + quark, \green);
			}, {
				quarksGui.setMsg("Error while installing" + quark, \red);
			});
		}, {
			quark.uninstall;
			quarksGui.setMsg("Uninstalled" + quark, \yellow);
		});
		quarksGui.update;
	}

	update {
		var palette = GUI.current.tryPerform(\palette),
			c = palette !? {palette.button},
			green = c.notNil.if({Color.green.blend(c, 0.5)}, {Color.green(1, 0.5)}),
			grey = c.notNil.if({Color.grey.blend(c, 0.5)}, {Color.grey(1, 0.5)}),
			isInstalled;

		btn.states = [
			if(quark.isDownloaded, {
				["+", nil, grey]
			}, {
				["+", nil, nil]
			}),
			["✓", nil, green],
		];

		isInstalled = quark.isInstalled;
		btn.value = isInstalled.binaryValue;

		treeItem.setString(1, isInstalled.if("Y", { quark.isDownloaded.if("N", "") }));
		treeItem.setString(2, quark.name ? "");
		treeItem.setString(3, (quark.version ? "").asString);
		treeItem.setString(4,
			if(quark.summary.isNil, {
				""
			}, {
				quark.summary.replace(Char.nl," ").replace(Char.tab, "")
			})
		);
	}
}

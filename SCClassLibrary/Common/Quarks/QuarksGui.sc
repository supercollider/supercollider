
QuarksGui {

	var model,
		quarks,
		window,
		treeView,
		quarkRows,
		palette,
		lblMsg,
		btnRecompile,
		detailView,
		<colors,
		initialized=false;

	*new { ^super.new.init }

	init {
		var bounds,
			btnUpdateDirectory,
			btnQuarksHelp,
			btnInstallFolder,
			btnOpenFolder,
			btnClear,
			btnLoad,
			btnSave;

		model = Quarks;
		palette = GUI.current.palette;

		bounds = Window.flipY(Window.availableBounds);
		window = Window("Quarks", Rect(0, 0, min(bounds.width * 0.75, 1000), bounds.height * 0.9).center_(bounds.center));

		colors = (
			error: (bg: Color.fromHexString("#fcdede"), text: Color.fromHexString("#991111")),
			info: (bg: Color.fromHexString("#e2eef9"), text: Color.fromHexString("#224466")),
			warn: (bg: Color.fromHexString("#f7ea57"), text: Color.fromHexString("#494620")),
			success: (bg: Color.fromHexString("#f5ffef"), text: Color.fromHexString("#6cc644")),
			primary: (bg: Color.fromHexString("#76c656"), text: Color.white),
			installed: (bg: Color.fromHexString("#76c656"), text: Color.white),
			default: (bg: Color.white, text: Color.black)
		);

		btnUpdateDirectory = Button()
			.states_([["Check for updates"], ["Checking..."]])
			.toolTip_("Download directory listing from" + Quarks.directoryUrl)
			.action_({
				treeView.enabled = false;
				this.checkForUpdates({
					treeView.enabled = true;
					btnUpdateDirectory.value = 0;
					this.update;
					this.setMsg("Directory has been updated.", \success);
				}, {
					treeView.enabled = true;
					btnUpdateDirectory.value = 0;
					this.update;
					this.setMsg("An error occured while updating the directory.", \warn);
				});
				nil
			});

		btnQuarksHelp = Button().states_([["Help"]])
			.toolTip_("Open Quarks documentation")
			.action_({ HelpBrowser.openBrowsePage("Quarks") });

		btnInstallFolder = Button().states_([["Install a folder"]])
			.toolTip_("Install classes from a folder on your computer")
			.action_({ this.installFolder });

		/*btnOpenFolder = Button().states_([["Open Quarks Folder"]])
			.toolTip_("Open" + model.folder)
			.action_({ model.openFolder });*/

		btnClear = Button().states_([["Uninstall all"]])
			.toolTip_("Uninstall all")
			.action_({ model.clear; this.update; });

		btnSave = Button().states_([["Save Quarks set"]])
			.toolTip_("Save currently installed quarks to a file")
			.action_({
				Dialog.savePanel({ |path|
					this.setMsg("Saving quarks to file:" + path);
					model.save(path);
					this.setMsg();
				})
			});

		btnLoad = Button().states_([["Load Quarks set"]])
			.toolTip_("Install a set of quarks from a file")
			.action_({
				Dialog.openPanel({ |path|
					this.runCancellable({
						this.setMsg("Installing from file:" + path);
						0.1.wait;
						model.load(path, {
							this.setMsg();
							0.1.wait;
							this.update;
						});
					});
				});
			});

		btnRecompile = Button().states_([
				["Recompile class library"],
				["Recompile class library", colors.primary.text, colors.primary.bg]
			])
			.toolTip_("You will need to recompile the class library after making any changes")
			.action_({ thisProcess.platform.recompile })
			.enabled_(false);

		lblMsg = StaticText()
			.font_(GUI.font.new(size:13, usePointSize:true))
			.align_(\center);

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
				HLayout(
					btnUpdateDirectory,
					btnInstallFolder,
					btnClear,
					btnSave,
					btnLoad,
					btnQuarksHelp,
					btnRecompile),
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
		if(initialized.not, {
			treeView.sort(0, true);
			initialized = true;
		});
		treeView.invokeMethod(\resizeColumnToContents, 0);
		treeView.invokeMethod(\resizeColumnToContents, 1);
		treeView.invokeMethod(\resizeColumnToContents, 2);
		treeView.invokeMethod(\resizeColumnToContents, 3);
		btnRecompile.enabled = recompile;
		btnRecompile.value = recompile.if(1, 0);
		detailView.update();
	}

	setMsg { |msg, style|
		lblMsg.background = colors[style ? \default].bg;
		lblMsg.stringColor = colors[style ? \default].text;
		lblMsg.string = msg ? "";
	}
	checkForUpdates { |onComplete, onCancel|
		this.runCancellable({
			this.setMsg("Fetching directory listing...", \info);
			0.1.wait;
			model.fetchDirectory(true);
			this.setMsg("Checking for updates to Quarks...", \info);
			0.1.wait;
			model.checkForUpdates();
		}, onComplete, onCancel)
	}
	installFolder {
		FileDialog({ |path|
				Quarks.install(path);
				this.update;
			},
			nil, // cancel
			2,  // QFileDialog::Directory
			0,  // QFileDialog::AcceptOpen
			true); // just first value in paths array
	}
	runCancellable { |fn, onComplete, onCancel|
		// call a function that can be cancelled by cmd-.
		var r, cancel;
		r = Routine.run({
			fn.protect({
				CmdPeriod.remove(cancel);
				onComplete.value();
			});
		}, clock: AppClock);
		cancel = {
			r.stop();
			CmdPeriod.remove(cancel);
			onCancel.value;
		};
		CmdPeriod.add(cancel);
	}
}


QuarkDetailView {

	var <model,
		view,
		quarksGui,
		details,
		txtTitle;

	makeView { |argQuarksGui|
		quarksGui = argQuarksGui;

		txtTitle = StaticText().font_(Font(size:16));

		details = TreeView()
			.setProperty(\rootIsDecorated, false)
			.columns_(["", ""])
			.canSort_(false);

		view = View();
		view.layout = VLayout(
			txtTitle,
			details
		).spacing_(4).margins_(0);
		view.visible = false;
		^view
	}
	update {
		var tags, refspec, isInstalled = false, isDownloaded = false, path,
			url,
			webpage,
			dependencies,
			makeBtn = { |text, fn|
				Button()
					.fixedSize_(Size(600, 20))
					.states_([[text]])
					.action_(fn);
			};

		if(model.notNil, {
			isInstalled = model.isInstalled;
			isDownloaded = model.isDownloaded;
			url = model.url;
			webpage = model.data['url'];

			txtTitle.string = model.name;

			details.clear();


			if(model.summary.notNil, {
				this.pushRow("Summary", model.summary);
			});

			// author

			// version
			if(model.data['version'].notNil, {
				this.pushRow("Version", model.data['version'].asString);
			});

			if(webpage != url and: webpage.notNil, {
				this.pushRow("Webpage", makeBtn.value(webpage, {
					this.openWebpage();
				}));
			});

			if(url.notNil, {
				this.pushRow("Repository", makeBtn.value(url, {
					this.openGithub();
				}));
			});

			if(isDownloaded or: isInstalled, {
				path =  model.localPath;
				if(path.size > 64) { path = "..." + path.keep(-64) };
				this.pushRow("Local path", makeBtn.value("Open Folder:" + path, {
					this.openLocalPath;
				}));
			});
			if(isInstalled and: { File.exists(model.localPath).not }, {
				this.pushRow("ERROR", model.localPath + "does not exist");
			});

			dependencies = model.dependencies;
			if(dependencies.notEmpty) {
				this.pushRow("Dependencies", dependencies.collect(_.asString).join(Char.nl));
			};
			if(isInstalled or: {
				isDownloaded and: {model.data['helpdoc'].isString}
			}, {
				this.pushRow("Help", makeBtn.value("Open help", { model.help }));
			});

			if(isDownloaded, {
				if(isInstalled, {
					this.pushRow("Classes",
						makeBtn.value("Show classes", { this.showClasses })
					);
					this.pushRow("Extensions",
						makeBtn.value("Show extension methods",
							{ this.showExtensionMethods })
					);
				});
			}, {
				if(model.url.notNil, {
					this.pushRow("", makeBtn.value("Fetch", {
						model.checkout();
						quarksGui.update();
						this.update();
					}));
				});
			});

			model.data.keysValuesDo({ |k, v|
				if(#[\name, \summary, \url, \path, \dependencies, \version].includes(k).not) {
					this.pushRow(k.asString, v.value().asString);
				}
			});

			// versions
			if(isDownloaded and: {model.git.notNil}, {
				this.pushRow("Versions", this.makeVersionTree());
			});

			view.visible = true;
		}, {
			view.visible = false;
		});
	}
	pushRow { |caption, obj|
		if(obj.isString, {
			details.addItem([caption, obj]);
		}, {
			details.addItem([caption, nil]).setView(1, obj);
		});
	}
	model_ { |quark|
		model = quark;
		this.update;
	}
	visible_ { |bool|
		view.visible = bool;
	}
	openWebpage {
		var url = model.data['url'];
		if(url.notNil, {
			openOS(url);
		});
	}
	openGithub {
		var url = model.url;
		if(url.notNil, {
			if(url.beginsWith("git:"), {
				url = "https:" ++ url.copyToEnd(4)
			});
			openOS(url);
		});
	}
	openLocalPath {
		model.localPath.openOS;
	}
	showClasses {
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
	}
	showExtensionMethods {
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
	}
	makeVersionTree {
		var versions, tags, remoteLatest, currentRefspec, currentSha, isDirty, tree;

		versions = List.new;

		tags =  model.tags;

		// add LATEST iff remoteLatest != sha of most recent tag
		remoteLatest = model.git.remoteLatest;
		if(tags.size == 0 or: {
			remoteLatest != model.git.shaForTag(tags.first);
		}, {
			versions.add([remoteLatest, "LATEST"]);
		});

		// working copy
		currentRefspec = model.refspec;
		if(currentRefspec.size != 40, {
			if(currentRefspec.beginsWith("tags/"), {
				currentSha = model.git.shaForTag(currentRefspec.copyToEnd(5));
			}, {
				currentSha = model.git.shaForTag(currentRefspec);
			});
		}, {
			currentSha = currentRefspec;
		});

		tags.do { |tag|
			var refspec = "tags/" ++ tag;
			versions.add([refspec, tag]);
		};

		isDirty = model.git.isDirty;
		if(isDirty, {
			currentRefspec = "DIRTY";
			versions.insert(0, ["DIRTY", "DIRTY"]);
		});

		tree = TreeView()
			.setProperty(\rootIsDecorated, false)
			.columns_(["Version", "Checkout"]);

		versions.do({ |tagRefspec|
			var tag, refspec, btn, row, isCurrent, btnText, enabled;
			# refspec, tag = tagRefspec;
			isCurrent = refspec == currentRefspec;
			row = tree.addItem([tag, nil]);
			// if installed then you can switch versions
			// else you can install any including current
			if(model.isInstalled, {
				btnText = if(isCurrent, "INSTALLED", "Install");
				enabled = isCurrent.not;
			}, {
				btnText = "Install";
				enabled = true;
			});
			if(isDirty, {
				enabled = false;
			});
			btn = Button()
				.fixedSize_(Size(100, 20))
				.states_([[btnText]]);
			btn.enabled = enabled;
			if(enabled, {
				btn.action = { this.checkout(refspec) };
			});
			row.setView(1, btn);

			if(refspec == currentRefspec, {
				row.colors = [quarksGui.colors.installed.bg, quarksGui.colors.installed.bg];
				row.setTextColor(0, quarksGui.colors.installed.text);
			});
		});

		tree.invokeMethod(\resizeColumnToContents, 0);
		^tree
	}
	checkout { |refspec|
		{
			quarksGui.setMsg("Installing" + model.name);
			if(model.isInstalled, {
				// reinstall possibly with different dependencies
				model.uninstall;
			});
			model = Quarks.install(model.url, refspec);
		}.protect({ |err|
			this.update;
			quarksGui.update;
			if(err.isNil, {
				quarksGui.setMsg(model.name + "has checked out" + refspec, \success);
			}, {
				quarksGui.setMsg(model.name + "failed to checkout" + refspec, \error);
			});
		});
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
			"",
			nil,
			"",
			"",
			""
		]).setView(1, btn);
		treeItem.setTextColor(0, Color.clear);

		btn.action = { |btn|
			this.install(btn.value > 0);
		};
		this.update;
	}
	install { |bool|
		quarksGui.runCancellable({
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
					quarksGui.setMsg("Installed" + quark, \success);
				}, {
					quarksGui.setMsg("Error while installing" + quark, \error);
				});
			}, {
				quark.uninstall;
				quarksGui.setMsg("Uninstalled" + quark, \success);
			});
		}, {
			quarksGui.update;
		}, {
			quarksGui.setMsg("Cancelled" + quark, \info);
			quarksGui.update;
		});
	}

	update {
		var isInstalled;

		btn.states = [
			["+", nil, nil],
			["✓", quarksGui.colors.primary.text, quarksGui.colors.primary.bg],
		];

		isInstalled = quark.isInstalled;
		btn.value = isInstalled.binaryValue;

		// this column has invisible text
		// its used to sort the rows so that installed quarks are at the top
		treeItem.setString(0, isInstalled.if("Y", { quark.isDownloaded.if("N", "") }));
		// 1 is the install button. its not possible to sort by this column
		treeItem.setString(2, quark.name ? "");
		treeItem.setString(3, (quark.version ? "").asString);
		treeItem.setString(4, (quark.summary ? "").replace(Char.nl.asString," ").replace(Char.tab.asString, ""));
	}
}

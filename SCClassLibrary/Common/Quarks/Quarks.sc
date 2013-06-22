/**
  *
  * Subversion based package repository and package manager
  * sk, cx, danstowell, LFSaw
  *
  *  this is the interface class for
  *   accessing the SVN repository, (eg. the sourceforge quarks project)
  *   downloading those to the local quarks folder (Platform.userAppSupportDir/quarks)
  *   installing individual quarks by symlinking from the local quark folder into the [Platform dependent]/Extensions folder
  *
  */


Quarks
{
	classvar global, <allInstances, <known;
	var <repos, <local;

	*global { ^(global ?? { global = this.new; }) }

	*new { | reposPath, localPath |
		var newQ;
		if((this.known[reposPath].isNil.not) and: (this.known[reposPath] != localPath), {
			("The repository is in the list of known repositories, but with a different local path. You are recommended to use this local path:"
					 + this.known[reposPath]).warn;
		});
		newQ = super.new.initQuarks(
			reposPath,
			localPath
		);
		allInstances.add(newQ);
		^newQ;
	}
	initQuarks{|reposPath, localPath|
		local = LocalQuarks(localPath, this);
		repos = QuarkSVNRepository(reposPath, local);
	}

	*initClass {
		Class.initClassTree( Platform );
		allInstances = List(1);
		known = Dictionary[
			"https://svn.code.sf.net/p/quarks/code" -> (Platform.userAppSupportDir +/+ "quarks"),
			"https://svn.sonenvir.at/svnroot/SonEnvir/trunk/src/quarks-sonenvir" -> (Platform.userAppSupportDir +/+ "quarks-sonenvir"),
			"https://sc3-plugins.svn.sourceforge.net/svnroot/sc3-plugins/build" -> (Platform.userAppSupportDir +/+ "SC3plugins")
			];
	}
	*forUrl { |url|
		var q;
		this.global; // ensure the global one is constructed
		q = allInstances.detect({|q| q.repos.url == url});
		if(q.isNil && this.known[url].isNil.not, {
			q = Quarks.new(url, this.known[url]);
		});
		^q;
	}

	update { |name|
		var q;
		if(name.notNil,{
			if((q = local.findQuark(name)).isNil,{
				Error("Local Quark "+name+" not found, cannot update from repository").throw;
			});
			repos.svn("update", (local.path+/+q.path).escapeChar($ ));
		},{
			//update all
			repos.update(local)
		});
	}
	/*add { |name|
		// this should create or manage the directory file too if it exists
		var q;
		if((q = local.findQuark(name)).isNil,{
			Error("Local Quark code not found, cannot add to repository").throw;
		});
		repos.svn("add",local.path++"/"++q.path);
	}*/
	commit { |name,message|
		var q;
		if((q = local.findQuark(name)).isNil,{
			Error("Local Quark code not found, cannot commit to repository").throw;
		});
		if(message.isNil,{
			Error("svn log message required to commit").throw;
		});
		repos.svn("commit","-m",message,"-F",local.path +/+ q.path);
	}
	// TODO: Deprecate "checkoutDirectory" - "updateDirectory" can be used whether or not there's an existing one
	checkoutDirectory {
		"Please wait for directory to be checked out.".postln;
		^this.updateDirectory
	}
	updateDirectory {
		repos.updateDirectory
	}

	listCheckedOut {
		this.checkedOut.do { |q| q.postDesc };
	}
	listAvailable {
		this.repos.quarks.do { |q| q.postDesc };
	}

	checkoutAll { repos.checkoutAll(local.path) }
	checkout { |name, version, sync=false|
		var q;
		if(local.findQuark(name,version).notNil,{
			inform("Quark "++name++" already checked out");
			^this
		});

		q = repos.findQuark(name,version);
		if(q.isNil,{
			Error("Quark not found in repository.").throw;
		});
		repos.checkout(q, local.path, sync);
	}
	// DEPRECATED because it has a different and confusing functionality w.r.t. QuarkSVNRepos.checkDir
	checkDir {
		var d;
		"Quarks.checkDir is deprecated".warn;
		d = (Platform.userExtensionDir +/+ local.name).escapeChar($ );
		if(d.pathMatch.isEmpty,{
			("creating: " + d).inform;
			d.mkdir;
		});
	}
	checkedOut {
		^local.quarks
	}
	status { |name|
		var q;
		if(name.notNil,{
			q = local.findQuark(name);
			repos.svn("status",(local.path +/+ q.path).escapeChar($ ));
		},{
			repos.svn("status",local.path.escapeChar($ ));
		});
	}
	installed {
		// of quarks in local, select those also present in userExtensionDir
		^local.quarks.select{|q|
			(Platform.userExtensionDir.escapeChar($ )
				+/+ local.name
				+/+ q.path
			).pathMatch.notEmpty
		}
	}
	install { | name , includeDependencies=true, checkoutIfNeeded=true |
		var q, deps, installed, dirname, quarksForDep;

		if(this.isInstalled(name),{
			(name + "already installed").inform;
			^this
		});

		q = local.findQuark(name);
		if(q.isNil,{
			if(checkoutIfNeeded) {
				(name.asString + " not found in local quarks; checking out from remote ...").postln;
				this.checkout(name, sync: true);
				q = local.reread.findQuark(name);
				if(q.isNil, {
					Error("Quark" + name + "install: checkout failed.").throw;
				});
			}
			{
				Error(name.asString + "not found in local quarks.  Not yet downloaded from the repository ?").throw;
			};
		});

		if(q.isCompatible.not,{
			(q.name + " reports that it is not compatible with your current class library.  See the help file for further information.").inform;
			^this
		});

		// create /quarks/ directory if needed
		if(this.repos.checkDir.not){this.checkoutDirectory};

		// Now ensure that the dependencies are installed (if available given the current active reposses)
		if(includeDependencies, {
			q.dependencies(true).do({ |dep|
				quarksForDep = if(dep.repos.isNil, {this}, {Quarks.forUrl(dep.repos)});
				if(quarksForDep.isNil, {
					("Quarks:install - unable to find repository for dependency '" ++ dep.name
						++ "' - you may need to satisfy this dependency manually. No repository detected locally with URL "++dep.repos).warn;
				}, {
					if(quarksForDep.isInstalled(dep.name).not, {
						try({
							quarksForDep.install(dep.name, false, checkoutIfNeeded)
						}, {
							("Unable to satisfy dependency of '"++name++"' on '"++dep.name
								++"' - you may need to install '"++dep.name++"' manually.").warn;
						});
					});
				});
			});
		});

		// Ensure the correct folder-hierarchy exists first
		dirname = (Platform.userExtensionDir +/+  local.name +/+ q.path).dirname;
		if(File.exists(dirname).not, {
			dirname.mkdir
		});

		// install via symlink to Extensions/<quarks-dir>
		("ln -s " +  (local.path +/+ q.path).escapeChar($ ) +  (Platform.userExtensionDir +/+ local.name +/+ q.path).escapeChar($ )).systemCmd;
		(q.name + "installed").inform;
	}
	listInstalled {
		this.installed.do { |q| q.postDesc };
	}
	isInstalled { arg name;
		^this.installed.detect{|quark| quark.name == name }.notNil
	}
	uninstall { | name |
		var q, deps, installed;
		name = name.asString;
		if(this.isInstalled(name).not,{
			^this
		});

		q = local.findQuark(name);
		if(q.isNil,{
			Error(
				name +
				"is not found in Local quarks in order to look up its relative path.  You may remove the symlink manually."
			).throw;
		});

		// install via symlink to Extensions/Quarks
		("rm " +  (Platform.userExtensionDir +/+ local.name +/+ q.path).escapeChar($ )).systemCmd;
		(q.name + "uninstalled").inform;
	}

	help { |name|
		var q;
		if(name.isNil){
			"Quarks".help
		}{
			q = local.findQuark(name);
			if(q.isNil,{
				Error(
					name.asString +
					"not found in local quarks.  Not yet downloaded from the repository ?"
				).throw;
			});
			q.help;
		}
	}

	name { ^local.name }

	///// convenience methods for Quarks.global

	*checkoutAll { this.global.repos.checkoutAll(this.global.local.path) }
	*checkout { | name, version, sync | this.global.checkout(name,version, sync); }

	/*
	 return Quark objects for each in App Support/SuperCollider/Quarks
	 (so actually this would list any Quarks that are in local development
	 and not yet checked in) */
	*checkedOut { ^this.global.checkedOut }
	*listCheckedOut {
		this.checkedOut.do { |q| q.postDesc };
	}
	*listAvailable {
		this.global.listAvailable
	}
	/*
	  download/update only the quark specification files from remote repos
	  and not the quarks themselves */
	*updateDirectory {
		^this.global.repos.updateDirectory
	}
	*checkoutDirectory {
		^this.global.checkoutDirectory
	}
	*update { |quarkName| this.global.update(quarkName) }

	/*
	  this symlinks from {App Support}/SuperCollider/Quarks to
	  {App Support}/SuperCollider/Extensions
	  it is then in the SC compile path */
	*install { |name, includeDependencies=true, checkoutIfNeeded=true|
		this.global.install(name, includeDependencies, checkoutIfNeeded)
	}

	/*
	  return Quark objects for each installed */
	*installed { ^this.global.installed }
	*listInstalled { ^this.global.listInstalled }
	*isInstalled { |name| ^this.global.isInstalled(name) }
	/*
	  removes the symlink */
	*uninstall { |name| ^this.global.uninstall(name) }

	/*
	  add code in {App Support}/SuperCollider/Quarks to the remote repos
	  and also adds the quarks file in DIRECTORY */
	//*add { |name| this.global.add(name) }
	/*
	  you may also use standard svn tools within {App Support}/SuperCollider/Quarks */
	*commit { |name,message| this.global.commit(name,message) }
	// post the SVN status
	*status { |name| this.global.status(name) }

	*local { ^this.global.local }
	*repos { ^this.global.repos }
	*help  {|name| ^this.global.help(name) }
	*gui { ^this.global.gui }

	gui {
		if( GUI.id === \qt ) { ^QuarksViewQt(this) } { ^QuarksView(this) }
	}
}

// a gui for Quarks. 2007 by LFSaw.de
QuarksView {
	var quarksCtrl, quarks;

	var window, caption, explanation, views, resetButton, saveButton, warning,
		scrollview, scrB, flowLayout, /* quarksflow, */ height, maxPerPage, nextButton, prevButton;

	*new { |quarksCtrl| ^super.new.init(quarksCtrl) }

	init { |q|
		var pageStart = 0, fillPage;

		quarksCtrl = q;

		fillPage = { |start|
			scrollview.visible = false;
			views.notNil.if({
				views.do({ |view| view.remove });
			});
			scrollview.decorator.reset;
			views = quarks.sort { |a, b| a.name < b.name }
				.collect { |quark|
					var qView = QuarkView.new(scrollview, 500@20, quark,
						quarksCtrl.installed.detect { |it| it == quark }.notNil
					);
					scrollview.decorator.nextLine;
					qView;
				};
			scrollview.visible = true;
			views
		};

		// note, this doesn't actually contact svn
		// it only reads the DIRECTORY entries you've already checked out
		quarks = quarksCtrl.repos.quarks.copy;

		scrB = GUI.window.screenBounds;
		height = min(quarks.size * 25 + 120, scrB.height - 60);

		window = GUI.window.new(quarksCtrl.name, Rect.aboutPoint( scrB.center, 250, height.div( 2 )));
		flowLayout = FlowLayout( window.view.bounds );
		window.view.decorator = flowLayout;

		caption = GUI.staticText.new(window, Rect(20,15,400,30));
		caption.font_( Font.sansSerif( 24 ));
		caption.string = quarksCtrl.name;
		window.view.decorator.nextLine;

		if ( quarks.size == 0 ){
			GUI.button.new(window, Rect(0, 0, 229, 20))
			.states_([["checkout Quarks DIRECTORY"]])
			.action_({ quarksCtrl.checkoutDirectory; });
		}{
			GUI.button.new(window, Rect(0, 0, 229, 20))
			.states_([["update Quarks DIRECTORY"]])
			.action_({ quarksCtrl.updateDirectory;});
		};

		GUI.button.new(window, Rect(0, 0, 200, 20))
		.states_([["refresh Quarks listing"]])
		.action_({
			window.close;
			quarksCtrl.gui;
		});

		window.view.decorator.nextLine;

		GUI.button.new(window, Rect(0, 0, 150, 20))
			.states_([["browse all help"]])
			.action_({ HelpBrowser.openBrowsePage("Quarks") });

		GUI.button.new(window, Rect(15,15,150,20))
		.states_([["open quark directory"]])
		.action_{ arg butt;
			openOS(quarksCtrl.local.path.escapeChar($ ))
		};

		resetButton = GUI.button.new(window, Rect(15,15,75,20));
		resetButton.states = [
			["reset"]
		];
		resetButton.action = { arg butt;
			views.do(_.reset);
		};

		saveButton = GUI.button.new(window, Rect(15,15,75,20));
		saveButton.states = [
			["save", nil, Color.blue(1, 0.2)]
		];
		saveButton.action = { arg butt;
			Task{
				warning.string = "Applying changes, please wait";
				warning.background_(Color(1.0, 1.0, 0.9));
				0.1.wait;
				views.do{|qView|
					qView.toBeInstalled.if({
						quarksCtrl.install(qView.quark.name);
						qView.flush
					});
					qView.toBeDeinstalled.if({
						quarksCtrl.uninstall(qView.quark.name);
						qView.flush;
					})
				};
				warning.string = "Done. You should now recompile sclang";
				warning.background_(Color(0.9, 1.0, 0.9));
			}.play(AppClock);
		};

		window.view.decorator.nextLine;
		explanation = GUI.staticText.new(window, Rect(20,15,500,20));
		explanation.string = "\"+\" -> installed, \"-\" -> not installed, \"*\" -> marked to install, \"x\" -> marked to uninstall";
		window.view.decorator.nextLine;

		warning = GUI.staticText.new(window, Rect(20,15,400,30));
		warning.font_( Font.sansSerif( 18 ));

		window.view.decorator.nextLine;
		GUI.staticText.new( window, 492 @ 1 ).background_( Color.grey );
		window.view.decorator.nextLine;

		flowLayout.margin_( 0 @0 ).gap_( 0@0 );
		scrollview = GUI.scrollView.new(window, 500 @ (height - 165))
			.resize_( 5 )
			.autohidesScrollers_(true);
		scrollview.decorator = FlowLayout( Rect( 0, 0, 500, quarks.size * 25 + 20 ));

		window.front;
		fillPage.(pageStart);
		^window;
	}
}

QuarksViewQt {
	var quarksCtrl;

	var window, lblCaption,
		btnUpdate, btnHelp, btnUpdateQuarks, btnOpenDir, btnReset, btnApply,
		lblStatus, lblExplanation, quarksView,
		infoView, btnQuarkHelp, btnQuarkOpen, btnQuarkClasses,
		btnQuarkMethods, txtDescription, btnCloseDetails;
	var quarks, views, curQuark;
	var refresh, msgWorking, msgDone;
	var screen, palette, gizmo;

	*new { |quarksCtrl| ^super.new.init(quarksCtrl) }

	init { |q|
		quarksCtrl = q;

		refresh = {
			quarksView.invokeMethod( \clear );
			quarks = quarksCtrl.repos.quarks.copy;
			quarksView.canSort = false;
			views = quarks.collect{|quark|
				var qView = QuarkViewQt.new(quarksView, quark,
					quarksCtrl.installed.detect{|it| it == quark}.notNil);
				qView;
			};
			quarksView.canSort = true;
			quarksView.sort( 1 );
			quarksView.invokeMethod( \resizeColumnToContents, 0 );
			quarksView.invokeMethod( \resizeColumnToContents, 1 );
			views
		};

		msgWorking = { arg msg;
			lblStatus.background = palette.button.blend(Color.yellow,0.2);
			lblStatus.string = msg;
		};

		msgDone =  { arg msg;
			lblStatus.background = palette.button.blend(Color.green,0.2);
			lblStatus.string = msg;
		};

		palette = GUI.current.palette;
		screen = Window.flipY(Window.availableBounds);
		window = Window( quarksCtrl.name, Rect( 0, 0, 700, screen.height * 0.9 ).center_(screen.center) );

		lblCaption = StaticText().font_( GUI.font.new(size:16,usePointSize:true) ).string_(quarksCtrl.name);

		btnUpdate = Button()
			.states_([["Update Quark Listing"]])
			.toolTip_("Download the latest information and update the Quarks listing")
			.action_({
				quarksView.enabled = false;
				msgWorking.value("Downloading the latest information...");
				AppClock.sched( 0.2, {
					protect {
						quarksCtrl.updateDirectory;
					} {
						refresh.value;
						quarksView.enabled = true;
						msgDone.value("Quarks listing has been updated with the latest information.")
					}
				});
				nil
			});

		btnUpdateQuarks = Button()
			.states_([["Update Quarks"]])
			.toolTip_("Update installed Quarks")
			.action_({
				quarksView.enabled = false;
				msgWorking.value("Updating installed Quarks...");
				AppClock.sched( 0.2, {
					protect {
						quarksCtrl.update;
					} {
						refresh.value;
						quarksView.enabled = true;
						msgDone.value("Quarks have been updated." +
							" You should recompile the class library for changes to take effect.")
					}
				});
				nil
			});

		btnHelp = Button().states_([["Help"]])
			.toolTip_("Browse help for all the Quarks")
			.action_({ HelpBrowser.openBrowsePage("Quarks") });

		btnOpenDir = Button().states_([["Directory"]])
			.toolTip_("Open the local Quarks directory")
			.action_({ openOS(quarksCtrl.local.path) });

		btnReset = Button()
			.states_([["Reset"]])
			.toolTip_("Clear the marked changes")
			.action_({ arg butt; views.do(_.reset) });

		btnApply = Button().states_([["Apply",nil,Color.blue.blend(palette.button,0.6)]])
			.toolTip_("Apply the marked changes")
			.action_({ arg butt;
				quarksView.enabled = false;
				msgWorking.value("Applying changes, please wait...");
				AppClock.sched( 0.2, {
					protect {
						views.do{|qView|
							qView.toBeInstalled.if({
								quarksCtrl.install(qView.quark.name);
								qView.flush
							});
							qView.toBeDeinstalled.if({
								quarksCtrl.uninstall(qView.quark.name);
								qView.flush;
							})
						};
					} {
						msgDone.value("Changes applied." +
							"You should recompile the class library for changes to take effect."
						);
						quarksView.enabled = true;
					}
				});
			});

		lblExplanation = StaticText().string_(
			"\"+\" installed, \"-\" not installed, \"*\" to install, \"x\" to uninstall"
		);

		lblStatus = StaticText().font_( GUI.font.new( size:12, usePointSize:true ) );

		quarksView = TreeView()
			.setProperty( \rootIsDecorated, false )
			.columns_([nil,"Name","Summary"])
			.itemPressedAction_({ |v|
				infoView.visible = true;
			})
			.onItemChanged_({ |v|
				var curItem, curView, inst;
				curItem = v.currentItem;
				curQuark = nil;
				if( curItem.notNil ) {
					curView = views.detect { |view| view.treeItem == curItem };
					if( curView.notNil ) {
						curQuark = curView.quark;
						txtDescription.string = curQuark.longDesc;
						btnQuarkOpen.enabled = curQuark.isLocal;
						inst = quarksCtrl.installed.detect{|it| it == curQuark}.notNil;
						btnQuarkClasses.enabled = inst;
						btnQuarkMethods.enabled = inst;
					}
				}{
					infoView.visible = false
				}
			});

		txtDescription = TextView(bounds:10@10)
			.font_( GUI.font.new( size:10, usePointSize:true ) )
			.tabWidth_(15)
			.autohidesScrollers_( true )
			.hasVerticalScroller_( true )
			.editable_( false )
			//.minSize_(Size(0,0));
			.minHeight_(50);

		btnQuarkHelp = Button()
			.states_([["Help"]])
			.toolTip_("Show help for this Quark")
			.action_({
				curQuark.help
			});

		btnQuarkOpen = Button()
			.states_([["Source"]])
			.toolTip_("Open the source directory of this Quark")
			.action_({
				openOS( "%/%".format(Quarks.local.path, curQuark.path) );
			});

		btnQuarkClasses = Button()
			.states_([["Classes"]])
			.toolTip_("Show list of classes defined by this Quark")
			.enabled_(false)
			.action_({
				var cls = curQuark.definesClasses;
				var tree, item, buts = [
					Button().states_([["Browse"]]).action_({
						cls[item.index].browse;
					}),
					Button().states_([["Help"]]).action_({
						cls[item.index].help;
					}),
					Button().states_([["Source"]]).action_({
						cls[item.index].openCodeFile;
					})
				];
				buts.do(_.enabled_(false));
				Window("% Classes".format(curQuark.name)).layout_(
					VLayout(
						tree = TreeView()
							.setProperty( \rootIsDecorated, false )
							.columns_(["Classes"])
							.onItemChanged_({|v| item = v.currentItem}),
						HLayout(*buts)
					)
				).front;
				if(cls.size>0) {
					cls.do {|c| tree.addItem([c.name.asString])};
					tree.itemPressedAction = { buts.do(_.enabled_(true)) }
				} {
					tree.addItem(["No classes"]);
				};
				tree.invokeMethod( \resizeColumnToContents, 0 );
			});

		btnQuarkMethods = Button()
			.states_([["Ext methods"]])
			.toolTip_("Show list of extension methods added by this Quark")
			.enabled_(false)
			.action_({
				var mets = curQuark.definesExtensionMethods;
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
				Window("% Extension Methods".format(curQuark.name)).layout_(
					VLayout(
						tree = TreeView()
							.setProperty( \rootIsDecorated, false )
							.columns_(["Class","Method"])
							.onItemChanged_({|v| item = v.currentItem}),
						HLayout(*buts)
					)
				).front;
				if(mets.size>0) {
					mets.collect{|m|
						var x = m.ownerClass.name;
						tree.addItem(if(x.isMetaClassName) {[x.asString[5..],"*"++m.name]} {[x.asString,"-"++m.name]});
					};
					tree.itemPressedAction = { buts.do(_.enabled_(true)) }
				} {
					tree.addItem([nil,"No extension methods"]);
				};
				tree.invokeMethod( \resizeColumnToContents, 0 );
				tree.invokeMethod( \resizeColumnToContents, 1 );
			});

		btnCloseDetails = StaticText()
			.string_("X")
			.align_(\center)
			.toolTip_("Hide Quark information panel")
			.mouseDownAction_({
				infoView.visible = false;
			});
		gizmo = btnCloseDetails.sizeHint;
		gizmo.width = gizmo.width + 20;
		btnCloseDetails.fixedSize = gizmo;

		infoView = View();
		infoView.layout = VLayout(
			HLayout( btnCloseDetails, btnQuarkHelp, btnQuarkOpen, btnQuarkClasses, btnQuarkMethods, nil ).margins_(0),
			txtDescription
		).spacing_(0).margins_(0);
		infoView.visible = false;

		window.layout =
			VLayout(
				lblCaption,
				HLayout( btnUpdate, btnUpdateQuarks, btnOpenDir, btnHelp, nil ),
				lblStatus,
				HLayout( btnReset, btnApply, [lblExplanation, s:1] ).margins_(0),
				[quarksView, s:5],
				[infoView, s:2]
			);

		refresh.value;
		window.front;
		^window;
	}
}

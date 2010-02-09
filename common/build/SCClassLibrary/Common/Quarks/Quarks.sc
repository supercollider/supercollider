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
			"https://quarks.svn.sourceforge.net/svnroot/quarks" -> (Platform.userAppSupportDir +/+ "quarks"),
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
			("mkdir -p" + d).systemCmd;
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
			("mkdir -p " + dirname.escapeChar($ )).systemCmd;
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
		var q, helpdoc, path;

		q = local.findQuark(name);
		if(q.isNil,{
			Error(
				name.asString +
				"not found in local quarks.  Not yet downloaded from the repository ?"
			).throw;
		});

		helpdoc = q.info.helpdoc;

		if(helpdoc.isNil, {
			("No primary helpdoc listed for Quark"+name).inform;
		}, {
			path = Quarks.local.path.select{|c| (c != $\\)}
				+/+ q.path +/+ helpdoc;
			Document.open(path);
		});
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
	*gui {
		^this.global.gui
	}

	// a gui for Quarks. 2007 by LFSaw.de
	gui {
		var	window, caption, explanation, views, resetButton, saveButton, warning,
			scrollview, scrB, flowLayout, /* quarksflow, */ height, maxPerPage, nextButton, prevButton;
		var	quarks;
		var pageStart = 0, fillPage = { |start|
			scrollview.visible = false;
			views.notNil.if({
				views.do({ |view| view.remove });
			});
			scrollview.decorator.reset;
			views = quarks.collect{|quark|
				var qView = QuarkView.new(scrollview, 500@20, quark,
					this.installed.detect{|it| it == quark}.notNil);
				scrollview.decorator.nextLine;
				qView;
			};
			scrollview.visible = true;
			views
		};

		// note, this doesn't actually contact svn
		// it only reads the DIRECTORY entries you've already checked out
		quarks = this.repos.quarks.copy
			.sort({ |a, b| a.name < b.name });

		scrB = GUI.window.screenBounds;
		height = min(quarks.size * 25 + 120, scrB.height - 60);

		window = GUI.window.new(this.name, Rect.aboutPoint( scrB.center, 250, height.div( 2 )));
		flowLayout = FlowLayout( window.view.bounds );
		window.view.decorator = flowLayout;

		caption = GUI.staticText.new(window, Rect(20,15,400,30));
		caption.font_( GUI.font.new( GUI.font.defaultSansFace, 24 ));
		caption.string = this.name;
		window.view.decorator.nextLine;

		if ( quarks.size == 0 ){
			GUI.button.new(window, Rect(0, 0, 229, 20))
			.states_([["checkout Quarks DIRECTORY", Color.black, Color.gray(0.5)]])
			.action_({ this.checkoutDirectory; });
		}{
			GUI.button.new(window, Rect(0, 0, 229, 20))
			.states_([["update Quarks DIRECTORY", Color.black, Color.gray(0.5)]])
			.action_({ this.updateDirectory;});
		};

		GUI.button.new(window, Rect(0, 0, 200, 20))
		.states_([["refresh Quarks listing", Color.black, Color.gray(0.5)]])
		.action_({
			window.close;
			this.gui;
		});

		window.view.decorator.nextLine;

		GUI.button.new(window, Rect(0, 0, 150, 20))
			.states_([["browse all help", Color.black, Color.gray(0.5)]])
			.action_({ Help(this.local.path).gui });

		// add open directory button (open is only implemented in OS X)
		(thisProcess.platform.name == \osx).if{
			GUI.button.new(window, Rect(15,15,150,20)).states_([["open quark directory", Color.black, Color.gray(0.5)]]).action_{ arg butt;
				"open %".format(this.local.path.escapeChar($ )).unixCmd;
			};
		};

		resetButton = GUI.button.new(window, Rect(15,15,75,20));
		resetButton.states = [
			["reset", Color.black, Color.gray(0.5)]
		];
		resetButton.action = { arg butt;
			views.do(_.reset);
		};

		saveButton = GUI.button.new(window, Rect(15,15,75,20));
		saveButton.states = [
			["save", Color.black, Color.blue(1, 0.5)]
		];
		saveButton.action = { arg butt;
			Task{
				warning.string = "Applying changes, please wait";
				warning.background_(Color(1.0, 1.0, 0.9));
				0.1.wait;
				views.do{|qView|
					qView.toBeInstalled.if({
						this.install(qView.quark.name);
						qView.flush
					});
					qView.toBeDeinstalled.if({
						this.uninstall(qView.quark.name);
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
		warning.font_( GUI.font.new( GUI.font.defaultSansFace, 18 ));

		window.view.decorator.nextLine;
		GUI.staticText.new( window, 492 @ 1 ).background_( Color.grey );		window.view.decorator.nextLine;

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
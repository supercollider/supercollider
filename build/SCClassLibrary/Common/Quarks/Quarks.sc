/**
  *
  * Subversion based package repository and package manager
  * a work in progress.  sk, cx, danstowell
  *
  */

// the interface class for 
//   accessing the SVN repository, (sourceforge quarks project)
//   downloading those to the local quarks folder (build/quarks)
//   installing selected quarks by symlink into the [Platform dependent]/Extensions folder

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
		local = LocalQuarks(localPath);
		repos = QuarkSVNRepository(reposPath, local);
	}
	
	*initClass {
		allInstances = List(1);
		known = Dictionary[
			"https://quarks.svn.sourceforge.net/svnroot/quarks" -> (Platform.userAppSupportDir ++ "/quarks"),
			"https://svn.sonenvir.at/svnroot/SonEnvir/trunk/src/quarks-sonenvir" -> (Platform.userAppSupportDir ++ "/quarks-sonenvir")
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
			repos.svn("update", local.path++"/"++q.path);
		},{
			//update all
			repos.update(local)
		});
	}
	add { |name|
		var q;
		// this does not support the messyness of working on multiple versions
		if((q = local.findQuark(name)).isNil,{
			Error("Local Quark code not found, cannot add to repository").throw;
		});
		repos.svn("add",local.path++"/"++q.path);
	}
	commit { |name,message|
		var q;
		if((q = local.findQuark(name)).isNil,{
			Error("Local Quark code not found, cannot commit to repository").throw;
		});
		if(message.isNil,{ 
			Error("svn log message required to commit").throw;
		});
		repos.svn("commit","-m",message,"-F",local.path++"/"++q.path);
	}
	checkoutDirectory {
		(repos.checkoutDirectory).if({
			"Directory was previously checked out, please perform updateDirectory.".postln
		}, { "Please wait for directory to be checked out.".postln });
	}
	updateDirectory {
		repos.updateDirectory
	}

	listCheckedOut {
		this.checkedOut.do { |q| q.postDesc };
	}
	checkoutAll { repos.checkoutAll(local.path) }
	checkout { |name, version|
		var q;
		if(local.findQuark(name,version).notNil,{
			inform("Quark "++name++" already checked out");
			^this
		});
		
		q = repos.findQuark(name,version);
		if(q.isNil,{
			Error("Quark not found in repository.").throw;
		});
		repos.checkout(q,local.path);
	}
	checkDir {
		var d;
		d = Platform.userExtensionDir.escapeChar($ ) ++ "/" ++ local.name;
		if(d.pathMatch.isEmpty,{
			("mkdir -p" + d).systemCmd;
			("creating: " + d).inform;
		});
	}
	checkedOut {
		^local.quarks
	}
	status { |name|
		var q;
		if(name.notNil,{
			q = local.findQuark(name);
			repos.svn("status",local.path ++ "/" ++ q.path);
		},{
			repos.svn("status",local.path);
		});
	}
	installed {
		// of quarks in local, select those also present in userExtensionDir
		^local.quarks.select{|q|
			(Platform.userExtensionDir.escapeChar($ ) 
				++ "/" ++ local.name 
				++ "/" ++ q.path
			).pathMatch.notEmpty
		}
	}
	install { | name , incdeps=true |
		var q, deps, installed, dirname, quarksForDep;

		if(this.isInstalled(name),{
			(name + "already installed").inform;
			^this
		});

		q = local.findQuark(name);
		if(q.isNil,{
			Error(name.asString + "not found in local quarks.  Not yet downloaded from the repository ?").throw;
		});
		
		// do we have to create /quarks/ directory ? If so, do it.
		this.checkDir;
		
		// Now ensure that the dependencies are installed (if available given the current active reposses)
		if(incdeps, {	
			q.dependencies.do({ |dep|
				quarksForDep = if(dep.repos.isNil, {this}, {Quarks.forUrl(dep.repos)});
				if(quarksForDep.isNil, {
					("Quarks:install - unable to find repository for dependency '" ++ dep.name 
						++ "' - you may need to satisfy this dependency manually. No repository detected locally with URL "++dep.repos).warn;
				}, {
					if(quarksForDep.isInstalled(dep.name).not, {
						try({
							quarksForDep.install(dep.name)
						}, {
							("Unable to satisfy dependency of '"++name++"' on '"++dep.name
								++"' - you may need to install '"++dep.name++"' manually.").warn;
						});
					});
				});
			});
		});
		
		// Ensure the correct folder-hierarchy exists first
		dirname = (Platform.userExtensionDir ++ "/" ++ local.name ++ "/" ++ q.path).dirname;
		if(File.exists(dirname).not, {
			("mkdir -p " + dirname.escapeChar($ )).systemCmd;
		});
		
		// install via symlink to Extensions/<quarks-dir>
		("ln -s " +  local.path ++ "/" ++ q.path +  Platform.userExtensionDir.escapeChar($ ) ++ "/" ++ local.name ++ "/" ++ q.path).systemCmd;
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
		("rm " +  Platform.userExtensionDir.escapeChar($ ) ++ "/" ++ local.name ++ "/" ++ q.path).systemCmd;
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
				++ "/" ++ q.path ++ "/" ++ helpdoc;
			Document.open(path);
		});
	}	

	name { ^local.name }

	///// convenience methods for Quarks.global
	
	*checkoutAll { this.global.repos.checkoutAll(this.global.local.path) }
	*checkout { | name, version | this.global.checkout(name,version); }
	
	/* 
	 return Quark objects for each in App Support/SuperCollider/Quarks
	 (so actually this would list any Quarks that are in local development 
	 and not yet checked in) */
	*checkedOut { ^this.global.checkedOut }
	*listCheckedOut {
		this.checkedOut.do { |q| q.postDesc };
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

	/* 
	  this symlinks from {App Support}/SuperCollider/Quarks to 
	  {App Support}/SuperCollider/Extensions
	  it is then in the SC compile path */
	*install { |name| this.global.install(name) }
	
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
	*add { |name| this.global.add(name) }
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
		var window, caption, explanation, views, resetButton, saveButton, warning;
		
		window = GUI.window.new(
			this.name, Rect(300,60,550, this.local.quarks.size * 25 + 120)).front;
		window.view.decorator =  FlowLayout( window.view.bounds );
		
		caption = GUI.staticText.new(window, Rect(20,15,400,30));
		caption.font_(GUI.font.new("Helvetica", 24));
		caption.string = this.name;
		window.view.decorator.nextLine;
		
		views = this.local.quarks.collect{|quark|
			var qView = QuarkView.new(window, 400@20, quark, this.installed.detect{|it| it == quark}.notNil);
			window.view.decorator.nextLine;
			qView;
		};
		
		resetButton = GUI.button.new(window, Rect(15,15,150,20));
		//	button.font_(GUI.font.new("Helvetica", 14));
		resetButton.states = [
			["reset", Color.black, Color.gray(0.5)]
		];
		resetButton.action = { arg butt;
			views.do(_.reset);
		};
		
		saveButton = GUI.button.new(window, Rect(15,15,150,20));
		//	button.font_(GUI.font.new("Helvetica", 14));
		saveButton.states = [
			["save", Color.black, Color.blue(1, 0.5)]
		];
		saveButton.action = { arg butt;
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
			warning.string = "You should now recompile sclang"
		};
		
		// add open directory button (open is only implemented in OS X)
		(thisProcess.platformClass == OSXPlatform).if{
			GUI.button.new(window, Rect(15,15,150,20)).states_([["open quark directory", Color.black, Color.gray(0.5)]]).action_{ arg butt;
				"open %".format(this.local.path).unixCmd;
			};
		};
		
		window.view.decorator.nextLine;
		explanation = GUI.staticText.new(window, Rect(20,15,550,20));
		explanation.string = "\"+\" -> installed, \"-\" -> not installed, \"*\" -> marked to install, \"x\" -> marked to uninstall";
		window.view.decorator.nextLine;

		warning = GUI.staticText.new(window, Rect(20,15,400,50));
		warning.font_(GUI.font.new("Helvetica", 24));
		warning.string = "";

		^window
	}
	
}




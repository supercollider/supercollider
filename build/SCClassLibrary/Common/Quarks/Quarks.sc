
/**
  *
  * Subversion based package repository and package manager
  * a work in progress.  sk & cx
  *
  */

// the interface class for 
//   accessing the SVN repository, (sourceforge quarks project)
//   downloading those to the local quarks folder (build/quarks)
//   installing selected quarks by symlink into the [Platform dependent]/Extensions folder

Quarks
{
	classvar global;
	var <repos, <local;

	*checkoutAll { this.global.repos.checkoutAll(this.global.local.path) }
	*checkout { | name, version | this.global.checkout(name,version); }
	*update { |name| this.global.update(name) }
	// return Quark objects for each in App Support/SuperCollider/Quarks
	// (so actually this would list any Quarks that are in local development and not yet checked in)
	*checkedOut { ^this.global.checkedOut }
	// download/update only the quark specification files from remote repos
	// and not the quarks themselves
	*updateDirectory { 
		^this.global.repos.updateDirectory 
	}


	// this symlinks from {App Support}/SuperCollider/Quarks to {App Support}/SuperCollider/Extensions
	// it is then in the SC compile path
	*install { |name| this.global.install(name) }
	// return Quark objects for each installed
	*installed { ^this.global.installed }
	*listInstalled { ^this.global.listInstalled }
	*isInstalled { |name| ^this.global.isInstalled(name) }
	// removes the symlink
	*uninstall { |name| ^this.global.uninstall(name) }
	
	// add code in {App Support}/SuperCollider/Quarks to the remote repos
	// and also adds the quarks file in DIRECTORY
	*add { |name| this.global.add(name) }
	// you may also use standard svn tools within {App Support}/SuperCollider/Quarks
	*commit { |name,message| this.global.commit(name,message) }
	// post the SVN status
	*status { |name| this.global.status(name) }
	
	*local { ^this.global.local }
	*repos { ^this.global.repos }

	*new { | reposPath, localPath |
		^super.newCopyArgs(
			QuarkSVNRepository(reposPath),
			LocalQuarks(localPath)
		)
	}
	*global { ^(global ?? { global = this.new; }) }
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
	update { |name|
		var q;
		if(name.notNil,{
			if((q = local.findQuark(name)).isNil,{
				Error("Local Quark "+name+" not found, cannot update from repository").throw;
			});
			repos.svn("update", local.path++"/"++q.path);
		},{
			//update all
			repos.svn("update", local.path++"/");
		});
	}
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
	install { | name |
		var q, deps, installed;

		if(this.isInstalled(name),{
			(name + "already installed").inform;
			^this
		});

		q = local.findQuark(name);
		if(q.isNil,{
			Error(name + "is not yet downloaded from the repository").throw;
		});
		
		
		// do we have to create /quarks/ directory ?
		this.checkDir;
		
		// install via symlink to Extensions/Quarks
		("ln -s " +  local.path ++ "/" ++ q.path +  Platform.userExtensionDir.escapeChar($ ) ++ "/quarks/" ++ q.path).systemCmd;
		(q.name + "installed").inform;
	}
	checkDir {
		var d;
		d = Platform.userExtensionDir.escapeChar($ ) ++ "/quarks";
		if(d.pathMatch.isEmpty,{
			("mkdir" + d).systemCmd;
			("creating: " + d).inform;
		});
	}
	
	installed {
		// of local quarks, select symlinks in userExtensionDir of same name
		var pathMatches,q;
		pathMatches = pathMatch( Platform.userExtensionDir ++ "/quarks/*/QUARK" );
		^pathMatches.collect { |p|  
			q = Quark.fromFile(p);
		};
		
	}
	listInstalled {
		this.installed.do { |q| [q.name, q.summary].postln };
	}
	isInstalled { arg name;
		^pathMatch( Platform.userExtensionDir ++ "/quarks/" ++ name  ).notEmpty
	}
	
	uninstall { | name |
		var q, deps, installed;

		if(this.isInstalled(name).not,{
			^this
		});

		q = local.findQuark(name);
		if(q.isNil,{
			Error(name + "is not found in Local quarks in order to look up its relative path.  You may remove the symlink manually.").throw;
		});

		// install via symlink to Extensions/Quarks
		("rm " +  Platform.userExtensionDir.escapeChar($ ) ++ "/quarks/" ++ q.path).systemCmd;
		(q.name + "uninstalled").inform;
	}


	
}


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


	// add code in App Support/SuperCollider/Quarks to remote repos
	*add { |name| this.global.add(name) }
	// or use standard svn tools in App Support/SuperCollider/Quarks
	*commit { |name,message| this.global.commit(name,message) }
	*update { |name| this.global.update(name) }
	*checkout { | name, version | this.global.checkout(name,version); }
	// return Quark objects for each in App Support/SuperCollider/Quarks
	// so actually it would list any Quarks not yet checked in
	*checkedOut { ^this.global.checkedOut }
	// post SVN status
	*status { |name| this.global.status(name) }
	
	// symlinks from code in App Support/SuperCollider/Quarks to Extensions
	// so that it is in the SC compile path
	*install { |name,version| this.global.install(name,version) }
	// return Quark objects for each installed
	*installed { ^this.global.installed }
	*findInstalled { |name, version| ^this.global.findInstalled(name,version) }



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
		repos.svnp("add",local.path++"/"++q.path);
	}
	commit { |name,message|
		var q;
		if((q = local.findQuark(name)).isNil,{
			Error("Local Quark code not found, cannot commit to repository").throw;
		});
		if(message.isNil,{ 
			Error("svn log message required to commit").throw;
		});
		repos.svnp("commit","-m",message,"-F",local.path++"/"++q.path);
	}
	update { |name|
		var q;
		if(name.notNil,{
			if((q = local.findQuark(name)).isNil,{
				Error("Local Quark code not found, cannot update from repository").throw;
			});
			repos.svn("update", local.path++"/"++q.path);
		},{
			//update all
			repos.svnp("update", local.path++"/");
		});
	}
	
	checkout { |name, version|
		var q;
		if(local.findQuark(name,version).notNil,{
			warn("Quark already checked out");
			^this
		});
		q = repos.findQuark(name,version);
		if(q.isNil,{
			Error("Quark not found in repository.").throw;
		});
		// might have to make local.path directory first
		repos.svnp("co",repos.url++"/" ++ q.path,local.path ++ "/" ++ q.path)
	}
	checkedOut {
		^local.quarks
	}
	status { |name|
		var q;
		if(name.notNil,{
			q = local.findQuark(name);
			repos.svnp("status",local.path ++ "/" ++ q.path);
		},{
			repos.svnp("status",local.path);
		});
	}
	install { | name, version |
		var q, deps, installed;
		// installed?
		q = this.findInstalled(name);
		if(q.notNil,{
			if(version.notNil,{
				if(q.version >= version,{
					(name + "already installed").inform;
					^this
				},{
					// can we get a higher version ?
					// get it
					// uninstall currently installed insufficient one
					"quark installed but insufficient version found".notYetImplemented
				});
			},{
				(name + "already installed").inform;
				^this
			});
		});
		
		// is it locally checked out ?
		q = local.findQuark(name,version);
		if(q.isNil,{
			// complain
			Error(name + "is not yet in your local quarks.  first you must: Quarks.checkout("++name++")").throw;
			
			// check it out from repos
		});
		
		
		// dependencies not yet handled
		/* deps = this.buildDeps(q);
		"DEPENDENCIES".postln;
		deps.do({ |dep| dep.name.postln });
		"============".postln;
		deps.do { |dep| local.install(dep) };
		this.sync; */
		
		// do we have to create /quarks/ directory ?
		
		// install via symlink to Extensions/Quarks
		("ln -s " +  local.path ++ "/" ++ q.path +  Platform.extensionsDir ++ "/quarks/" ++ q.path).debug("install not yet implemented");
	}
	
	installed {
		var pathMatches,q;
		pathMatches = pathMatch( thisProcess.platform.userExtensionDir ++ "/quarks/*/QUARK" );
		^pathMatches.collect { |p|  
			q = Quark.fromFile(p);
		};
		
	}
	listInstalled {
		this.installed.do { |q| [q.name, q.summary].postln };
	}
	findInstalled { arg name, version;
		var pathMatches, q;
		pathMatches = pathMatch( thisProcess.platform.userExtensionDir ++ "/quarks/" ++ name ++ "/QUARK" );
		if(pathMatches.size == 0,{ ^nil });
		q = loadPath( pathMatches.first );
		if(version <= q[\version],{
			// insufficient version
			inform(name + "version " + q[\version] + "is installed but version " + version + "is required");
			^nil
		});
		^q	
	}
	
	buildDeps { | q |
		var stream = CollStream([]), vis = List.new;
		this.buildDepsOn(q, stream, vis);
		^stream.contents
	}
	buildDepsOn { | q, stream, vis |
		var next;
		if (vis.includes(q).not) {
			vis.add(q);
			if (q.dependencies.notEmpty) {
				q.dependencies.do { |dep|
					if (this.findInstalled(dep.name).isNil) {
						Error(dep.name ++ " is not installed.").throw;
						next = repos.findQuark(dep.name,dep.version);
						if (next.isNil) {
							Error(dep.name ++ " is not in the remote repository").throw;
						};
						this.buildDepsOn(next, stream, vis);
					}
				};
			};
			stream.put(q);
		};
	}
}

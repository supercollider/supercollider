
// a work in progress.  sk & cx

// the interface class for 
//   accessing the SVN repository, (sourceforge quarks project)
//   downloading those to the local quarks folder (build/quarks)
//   installing selected quarks by symlink into the [Platform dependent]/Extensions folder

Quarks
{
	var <repos, <local;

	*new { | reposPath="https://svn.sourceforge.net/svnroot/quarks", localPath |
		if(localPath.isNil,{ localPath = "quarks"; });
		^super.newCopyArgs(
			QuarkSVNRepository(reposPath),
			LocalQuarks(localPath)
		)
	}
	checkout { |name, version|
		// already checked out ?
		var q;
		q = repos.findQuark(name,version);
		if(q.isNil,{
			Error("Quark not found ! " + name + version).throw;
		});
		repos.checkout(q,local.path)
	}
	install { | name, version |
		var q, deps, installed;
		// installed?
		q = this.findInstalled(name);
		if(q.notNil,{
			if(version.notNil,{
				if(q.version >= version,{
					(name + "already installed").inform;
				},{
					// can we get a higher version ?
					// get it
					// uninstall currently installed insufficient one
				});
			},{
				(name + "already installed").inform;
			});
		});
		
		// is it locally checked out ?
		q = local.findQuark(name,version);
		if(q.isNil,{
			// complain
			Error(name + "is not yet in your local quarks.  Quarks.checkout("++name++")").throw;
			
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
		("ln -s " +  local.path ++ "/" ++ q.path +  Platform.extensionsDir ++ "/quarks/" ++ q.path).debug;
	}
	update { |name|
		// svn update this dir
		var q;
		q = local.findQuark(name);
		("svn update " + local.path ++ "/" ++ q.path).debug;
	}
	

	listInstalled {
		var pathMatches;
		pathMatches = pathMatch( Platform.extensionsDir ++ "/quarks/*/QUARK" );
		pathMatches.do { |p| p.postln };
		//local.quarks.do { |q| [q.name, q.summary].postln };
	}
	findInstalled { arg name, version;
		var pathMatches, q;
		pathMatches = pathMatch( Platform.extensionsDir ++ "/quarks/" ++ name ++ "/QUARK" );
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

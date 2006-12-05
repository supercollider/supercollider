
/**
  *
  * Subversion based package repository and package manager
  * a work in progress.  sk & cx
  *
  */

// Local Quarks are those that have been checked out from the repository.
// and are stored in build/quarks prior to being actually installed

LocalQuarks
{
	var <path;

	*new { | path |
		^super.newCopyArgs((path ?? { Platform.userAppSupportDir ++ "/quarks"}).escapeChar($ ))
	}

	quarks {
		// check through each quark in repos/directory
		var paths,quarks;
		paths = (Platform.userAppSupportDir ++ "/quarks/DIRECTORY/*.quark").pathMatch;
		quarks = paths.collect({ |p| Quark.fromFile(p) });

		// check paths that do exist locally
		^quarks.select({ |q| (Platform.userAppSupportDir ++ "/quarks/" ++q.path).pathMatch.notEmpty })
	}
	findQuark { arg name, version;
		var matches;
		matches = this.quarks.select({ |q| q.name == name });
		if(version.notNil,{
			matches = matches.select({ |q| q.version >= version });
		});
		^matches.sort({ |a,b| a.version > b.version }).first
	}
	findPath { arg name, version;
		var q;
		if((q = this.findQuark(name, version)).isNil,{
			Error("Local Quark % not Found.".format(name.quote)).throw;
		});
		^path ++ "/" ++ q.path;
	}
	openFolder { arg name, version;
		if(name.isNil) { 
			unixCmd("open" + path) 
		} {
			unixCmd("open" + this.findPath(name, version))
		}
	}
}


/**
  *
  * Subversion based package repository and package manager
  * a work in progress.  sk, cx, lfsaw
  *
  */

// Local Quarks are those that have been checked out from the repository.
// and are stored in build/quarks prior to being actually installed

LocalQuarks
{
	classvar >globalPath; // where the "Quarks.global" checkout is stored

	var <path;
	var <parent; // the Quarks
	var all; // contains nil, or all local quarks

	*new { | path, parent |
		^super.newCopyArgs((path ?? {this.globalPath}), parent)
	}

	*globalPath {
		^ globalPath ?? { globalPath = Platform.userAppSupportDir ++ "/quarks"}
	}

	name {
		^PathName(path).fileName;
	}
	quarks {
		var paths, quarks;
		all.isNil.if{
			// check through each quark in repos/directory
			paths = (path ++ "/DIRECTORY/*.quark").pathMatch;
			quarks = Array(paths.size);
			paths.do { |p|
				try
				{ var q = Quark.fromFile(p, parent); quarks add: q }
				{ |e| e.errorString.postln }
			};
			// check paths that do exist locally
			all = quarks.select({ |q| (path ++ "/" ++ q.path).pathMatch.notEmpty })
		};
		^all
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
			openOS(path)
		} {
			openOS(this.findPath(name, version))
		}
	}

	/// reread local quarks directory
	reread {
		all = nil;
		this.quarks;
	}

		// stupid path has to be escaped above???
		// well sometimes you need the raw path...
	simplePath {
		var	out = path.copy;
		path.findAll("\\").reverseDo({ |i|
			path.removeAt(i);
		});
		^path
	}
}

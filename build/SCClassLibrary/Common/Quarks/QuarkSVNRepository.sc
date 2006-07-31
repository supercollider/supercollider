
/**
  *
  * Subversion based package repository and package manager
  * a work in progress.  sk & cx
  *
  */

QuarkSVNRepository
{
	var <url;

	*new { | url |
		^this.newCopyArgs(url ? "https://svn.sourceforge.net/svnroot/quarks")
	}
	// easiest to just check out all
	checkoutAll { |localRoot|
		this.svn("co", this.url ++ "/", localRoot ++  "/")
	}
	// checkout a specific quark
	checkout { | q, localRoot |
		this.svn("co", this.url ++ q.path, localRoot ++  q.path)
	}
	// check if the quarks directory is checked out yet
	checkDir {
		var dir;
		dir = Platform.userAppSupportDir ++ "/quarks/";
		if(File.exists(dir).not, {
			"Quarks dir is not yet checked out.  Execute:".debug;
			this.svn("co","-N",this.url,Quarks.local.path);
			this.svn("co",this.url++"/DIRECTORY",Quarks.local.path);
			^false;
		});
		^true;
	}
	// DIRECTORY contains a quark spec file for each quark regardless if checked out / downloaded or not
	updateDirectory {
		if(this.checkDir,{
			"To update the DIRECTORY:".debug;
			this.svn("update",Quarks.local.path ++ "/DIRECTORY/");
		});
	}
	update {
		"To update your local quarks working copies:".debug;
		this.svn("update",Quarks.local.path);
	}
	// load all specification quark objects from DIRECTORY
	// they may or may not be locally checked out
	quarks {
		var paths;
		paths = (Platform.userAppSupportDir ++ "/quarks/DIRECTORY/*.quark").pathMatch;
		^paths.collect({ |p| Quark.fromFile(p) });
	}
	// search DIRECTORY quark objects to see if quark is in repository
	findQuark { arg name, version;
		var matches;
		matches = this.quarks.select({ |q| q.name == name });
		if(version.notNil,{
			matches = matches.select({ |q| q.version >= version });
		});
		^matches.sort({ |a,b| a.version > b.version }).first
	}
	// OSX is not running svn commands ?
	svn { | cmd ... args |
		cmd = ("svn" + cmd + args.join(" "));
		"".debug;
		cmd.debug;
		"".debug;
		//cmd.unixCmd.debug("result:");
	}
	// just post
	svnp { |cmd ... args|
		cmd = ("svn" + cmd + args.join(" "));
		"".debug;
		cmd.debug;
		"".debug;
	}
}

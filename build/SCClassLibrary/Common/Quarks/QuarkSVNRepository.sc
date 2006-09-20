
/**
  *
  * Subversion based package repository and package manager
  * a work in progress.  sk & cx
  *
  */

QuarkSVNRepository
{
	classvar <>svnpath="/usr/local/bin/svn";
	var <url;

	*new { | url |
		if(File.exists(QuarkSVNRepository.svnpath).not,{
			Error("Correct SVN path is not set.  Set QuarkSVNRespository = /full/path/to/svn in your startup").throw;
		});
		^this.newCopyArgs(url ? "https://svn.sourceforge.net/svnroot/quarks")
	}
	// easiest to just check out all
	checkoutAll { |localRoot|
		this.svn("co", this.url ++ "/", localRoot ++  "/")
	}
	// checkout a specific quark
	checkout { | q, localRoot |
		this.svn("co", this.url ++ "/" ++ q.path, localRoot ++ "/" ++  q.path)
	}
	// check if the quarks directory is checked out yet
	checkDir {
		var dir;
//		dir = Platform.userAppSupportDir ++ "/quarks/";
		dir = Quarks.local.path.select{|c| (c != $\\)};
		if(File.exists(dir).not, {
			//"Quarks dir is not yet checked out.  Execute:".debug;
			this.svn("co","-N",this.url,Quarks.local.path);
			this.svn("co",this.url++"/DIRECTORY",Quarks.local.path);
			^false;
		});
		^true;
	}
	// DIRECTORY contains a quark spec file for each quark regardless if checked out / downloaded or not
	updateDirectory {|lQuark|
		lQuark = lQuark ? Quarks.local;
		this.svn("update",lQuark.path ++ "/DIRECTORY/");
	}
	update {|lQuark|
		//"To update your local quarks working copies:".debug;
		lQuark = lQuark ? Quarks.local;
		this.svn("update",lQuark.path);
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
	svn { | cmd ... args |
		cmd = ("export LANG='' ; " + svnpath + cmd + args.join(" ") + "2>&1");
		"".debug;
		cmd.debug;
		"".debug;
		cmd.unixCmd;
	}
	// just post
	svnp { |cmd ... args|
		cmd = ("svn" + cmd + args.join(" "));
		"".debug;
		cmd.debug;
		"".debug;
	}
}

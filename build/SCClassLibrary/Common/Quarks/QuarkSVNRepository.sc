/**
  *
  * Subversion based package repository and package manager
  * a work in progress.  sk & cx & ds & LFSaw
  *
  */

QuarkSVNRepository
{
	classvar <>svnpath="/usr/local/bin/svn";
	var <url, <local;
	
	*initClass {
		svnpath = [svnpath, "/usr/local/bin/svn", "/usr/bin/svn", "/opt/local/bin/svn", "/sw/bin/svn"].detect({ |path|
			File.exists(path);
		});
	}
	
	*new { | url, local |
		if(svnpath.isNil || File.exists(svnpath).not,{
			Error("Path to SVN executable is not correct.  Set \n\tQuarkSVNRepository.svnpath = \"/full/path/to/svn\"\n in your startup ").throw;
		});
		^this.newCopyArgs(url ? "https://quarks.svn.sourceforge.net/svnroot/quarks", local ?? {Quarks.local})
	}
	// easiest to just check out all
	checkoutAll { |localRoot|
		this.svn("co", this.url ++ "/", localRoot.escapeChar($ ) ++  "/")
	}
	// checkout a specific quark
	checkout { | q, localRoot, sync = false |
		var args = [(this.url ++ "/" ++ q.path).escapeChar($ ), (localRoot ++ "/" ++  q.path).escapeChar($ )];
		if(sync)
			{this.svnSync("co", *args)}
			{this.svn(    "co", *args)};
	}
	checkoutDirectory {
		var dir;
		dir = (local.path.select{|c| (c != $\\)}) ++ "/DIRECTORY" ;
		if(File.exists(dir).not, {
			this.svn("co", (this.url++"/DIRECTORY").escapeChar($ ), (local.path ++ "/DIRECTORY").escapeChar($ ));
			^false
		});
		^true
	}
	// check if the quarks directory is checked out yet
	checkDir {
		var dir;
		dir = local.path.select{|c| (c != $\\)};
		if(File.exists(dir).not, {
			//"Quarks dir is not yet checked out.  Execute:".debug;
			this.svn("co","-N",this.url, local.path.escapeChar($ ));
			this.svn("co",this.url++"/DIRECTORY", local.path.escapeChar($ ));
			^false;
		});
		^true;
	}
	// DIRECTORY contains a quark spec file for each quark regardless if checked out / downloaded or not
	updateDirectory {
		this.svn("update",(local.path ++ "/DIRECTORY/").escapeChar($ ));
	}
	update {
		this.svn("update",local.path.escapeChar($ ));
	}
	// load all specification quark objects from DIRECTORY
	// they may or may not be locally checked out
	quarks {
		var paths;
		paths = (local.path ++ "/DIRECTORY/*.quark").pathMatch;
		^paths.collect({ |p| Quark.fromFile(p, this.local.parent)});
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
		cmd = ("export LANG='' ; " + svnpath.escapeChar($ ) + cmd + args.join(" ") + "2>&1");
		"".debug;
		cmd.debug;
		"".debug;
		Platform.case(
			// On OSX we run it in a terminal window to minimise the risk of people getting stuck without a certificate
			\osx, {
				("echo \"
--------------------------------------------------------------

  SuperCollider Quarks: accessing remote repository.

  If this is the first time, you may be asked to accept a
  security certificate. Please do so!

  The command being run is:
"++cmd.escapeChar($")++"

--------------------------------------------------------------
\"
"++cmd).runInTerminal
			},
			// Non-OSX platforms run it internally
			{cmd.unixCmd}
		);
	}
	// Allows to wait for command to complete
	svnSync { | cmd ... args |
		cmd = ("export LANG='' ; " + svnpath.escapeChar($ ) + cmd + args.join(" ") + "2>&1");
		"".debug;
		cmd.debug;
		"".debug;
		cmd.systemCmd;
	}
	// just post
	svnp { |cmd ... args|
		cmd = ("svn" + cmd + args.join(" "));
		"".debug;
		cmd.debug;
		"".debug;
	}
}

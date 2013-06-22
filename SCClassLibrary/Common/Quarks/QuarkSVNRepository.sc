/**
  *
  * Subversion based package repository and package manager.
  * sk & cx & ds & LFSaw
  *
  */

QuarkSVNRepository
{
	classvar <>svnpath="/usr/local/bin/svn";
	var <url, <local;

	*initSVNDetails {
		var res;
		svnpath = [svnpath, "/usr/local/bin/svn", "/usr/bin/svn", "/opt/local/bin/svn", "/sw/bin/svn"].detect({ |path|
			try { File.exists(path) } { false }
		});
		if(svnpath.isNil and:{thisProcess.platform.hasFeature(\unixPipes)}){
			// Try and detect whether svn is in the path, and could be called just via "svn"
			res = "svn --version --quiet".unixCmdGetStdOut;
			if(res.size != 0 and: {res[0].asString.asInteger > 0}){
				svnpath = "svn";
			};
		};
	}

	*new { | url, local |
		QuarkSVNRepository.initSVNDetails;
		if(svnpath.isNil) {
			if(thisProcess.platform.name==\windows){
				Post
				<< 	"\tSVN not yet possible on windows. Quarks placed in the directory"
				<<	"\n\n\t\t" << Platform.userAppSupportDir << "\\quarks"
				<< 	"\n\n\t" << "will be available.\n"
			}{
				Post
				<< 	"\tSVN not found! Quarks placed in the directory"
				<<	"\n\n\t\t" << Platform.userAppSupportDir << "/quarks"
				<< 	"\n\n\t" << "will be available, but you need svn to checkout updated versions."
				<<	"\n\n\t" << "svn can be downloaded from:"
				<< 	"\n\n\t\t" << "http://subversion.tigris.org/project_packages.html\n"
			}
		};
		^this.newCopyArgs(url ? "https://svn.code.sf.net/p/quarks/code", local ?? {Quarks.local})
	}

	// returns true if some change was performed
	checkSVNandBackupIfNeeded{
		var res,files;
		files = (Quarks.local.path ++ "/*").pathMatch;
		if ( files.size != 0 ) {
			// there are files in the quarks dir
			if (  (Quarks.local.path ++ "/.svn").pathMatch.size == 0 ) {
				// but quarks dir itself is not under version control
				files.do{ |it|
					res = ("svn st -q " ++ it ).unixCmdGetStdOut;
					if ( res == "" ){
						// no local modifications, so delete the folder
						("rm -r " ++ it ).unixCmd;
					}{
						// local modifications, so copy the folder for backup
						("mv" + it + it.drop(-1) ++ "_modified" ).unixCmd;
						("You had local modifications in quark folder" + it + "a copy of the folder has been made, so please review your modifications there").inform;
					};
				};
				^true
			}
		};
		^false
	}

	// easiest to just check out all - BUT may waste your space since the global repos is becoming bigger and bigger!
	checkoutAll { |localRoot|
		this.checkSVNandBackupIfNeeded;
		this.svn("co", this.url ++ "/", localRoot.escapeChar($ ) ++  "/")
	}
	// checkout a specific quark (or multiple quarks - first arg can be an array).
	// NOTE: despite the method name, this actually uses "svn up" rather than "svn co", to ensure the base checkout is the base for this subfolder.
	// Therefore it can be used to update, equally well as to checkout, a quark.
	checkout { | q, localRoot, sync = false |

		var subfolders, fullCheckout, pathSoFar, skeletonCheckout, args;

		skeletonCheckout = [];
		fullCheckout     = [];

		q.asArray.do{ |oneq|

			subfolders = oneq.path.split($/);

			fullCheckout = fullCheckout ++ [oneq.path.escapeChar($ )];
			subfolders.pop; // The final entry is the folder whose entire contents we want

			pathSoFar = ".";
			skeletonCheckout = skeletonCheckout ++ subfolders.collect{ |element, index|
				pathSoFar = pathSoFar ++ "/" ++ element
			}.collect{|el| el.escapeChar($ )};
		};

		// Now construct a svn command for the skels, and then a svn command for the fulls
		args = if(skeletonCheckout.isEmpty){
			[]
		}{
			["update", ["--non-recursive"] ++ skeletonCheckout.collect(_.asSymbol).as(OrderedIdentitySet).collectAs((_.asString), Array)]
		} ++ ["update", fullCheckout.collect(_.asSymbol).as(OrderedIdentitySet).collectAs((_.asString), Array)];
		this.svnMulti(localRoot, sync, *args);
	}

	// check if the quarks directory is checked out yet
	checkDir {
		var dir;
		dir = local.path.select{|c| (c != $\\)};
		if(File.exists(dir).not, {
			// This method SHOULD NOT check the dir out on your behalf! That's not what it's for! Use .checkoutDirectory for that.
			//"Quarks dir is not yet checked out.  Execute:".debug;
			//this.svn("co","--non-recursive",this.url, local.path.escapeChar($ ));
			//this.svn("up", local.path.escapeChar($ ) +/+ "DIRECTORY");
			^false;
		});
		^true;
	}

	// updateDirectory and checkoutDirectory can be handled by the same function, simplifying the user experience, hopefully.
	// TODO: deprecate checkoutDirectory methods, simply use updateDirectory whether or not it's the first time.
	//        Then update the help docs to the simpler instructions.
	checkoutDirectory {|forceSync=false|
		^this.updateDirectory(forceSync);
	}

	// DIRECTORY contains a quark spec file for each quark regardless if checked out / downloaded or not.
	updateDirectory { |forceSync=false|
		if (svnpath.isNil) {
			"\n\tSince SVN not installed, you cannot checkout Quarks. ".postln.halt;
		};

		// If there's no svn metadata then either there's nothing there at all or there's a non-svn thing in the way
		if (  File.exists(local.path ++ "/.svn").not) {
			if( File.exists(local.path).not ) {
				// Main folder doesn't exist at all, simply check it out
				this.svnMulti(".", forceSync,
					"checkout", ["--non-recursive", this.url, local.path.select{|c| (c != $\\)}.escapeChar($ )],
					// and then do the directory update:
					"update", [local.path.select{|c| (c != $\\)}.escapeChar($ ) +/+ "DIRECTORY"]
					);
			}{
				Post
				<< "\n\tCurrent Quarks are not SVN. Delete the directories \n\t\t "
				<< local.path << "\n\tand\n\t\t"
				<< Platform.userExtensionDir << "/quarks\n"
				<< "\tand recompile before checking out quarks";
				nil.halt;
			};
		}{
			this.svnMulti(local.path, forceSync, "update", ["DIRECTORY"]);
		};
	}
	update {
		this.checkSVNandBackupIfNeeded;
		if(this.checkDir.not){
			this.checkoutDirectory; // ensures that the main folder exists
			this.svn("update",local.path.escapeChar($ ));
		}{
			// The "checkout" method can do the updating of individual quarks for us
			this.checkout(local.quarks, local.path);
		};
	}
	// load all specification quark objects from DIRECTORY
	// they may or may not be locally checked out
	quarks {
		var paths, quarks;
		paths = (local.path ++ "/*/*.quark").pathMatch;
		quarks = Dictionary.new;
		paths.do { |p|
			try
			{ var q=Quark.fromFile(p, this.local.parent); quarks[q.name] = q; }
			{ |e| e.errorString.postln }
		};
		^quarks.values;
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

	// Can perform multiple svn commands in one call.
	// Call it with [cmd, args, cmd, args] pairs - e.g. svnMulti(....   "co", ["--quiet", "/some/repo"], "up", ["~/my/repo"]).
	// "forceSync" is whether or not to force to run in sync (on OSX we like to do it async to avoid certificate-pain)
	svnMulti { | baseDir, forceSync=(false) ... pairs |
		var cmd, svnpath = this.class.svnpath.escapeChar($ );
		if (svnpath.isNil) {
			Error("SVN is not installed! Quarks cannot be updated.").throw;
		};
		cmd = "export LANG='' && cd" + baseDir.select{|c| (c != $\\)}.escapeChar($ );
		pairs.pairsDo{|onecmd, args|
			cmd = cmd + "&&" + svnpath + onecmd + args.join(" ")
		};
		cmd = cmd + "2>&1";
		"".debug;
		cmd.debug;
		"".debug;

		if(forceSync.not and: {thisProcess.platform.name == \osx}){
			// asynchronous but user-friendly execution - on OSX we
			// run it in a terminal window to minimise the risk of people getting stuck without a certificate
			("echo " ++ $" ++ "
--------------------------------------------------------------

 SuperCollider Quarks: accessing remote repository.

 If this is the first time, you may be asked to accept a
 security certificate. If you can trust it, please do so!

 The command being run is:
" ++ cmd.escapeChar($") ++ "

--------------------------------------------------------------
" ++ $" ++ cmd).runInTerminal
		}{
			// synchronous execution:
			cmd.unixCmdGetStdOut.postln;
		};
	}
	svn { | cmd ... args |
		^this.svnMulti(".", false, cmd, args);
	}
	// Allows to wait for command to complete
	svnSync { | cmd ... args |
		^this.svnMulti(".", true, cmd, args);
	}

	// just post
	svnp { |cmd ... args|
		cmd = ("svn" + cmd + args.join(" "));
		"".debug;
		cmd.debug;
		"".debug;
	}
}

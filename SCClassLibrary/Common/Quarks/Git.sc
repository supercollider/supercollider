
Git {
	var <>localPath;

	*isGit { |localPath|
		^File.exists(localPath +/+ ".git")
	}
	*new { |localPath|
		^super.new.localPath_(localPath)
	}
	clone { |url|
		this.git([
			"clone",
			url,
			localPath.escapeChar($ )
		], false);
	}
	pull {
		this.git(["pull"])
	}
	checkout { |refspec|
		this.git(["checkout", refspec])
	}
	fetch {
		this.git(["fetch"])
	}
	isDirty {
		var out = this.git(["--no-pager diff HEAD --"]);
		if(out.size != 0, {
			out.debug;
			^true
		});
		^false
	}
	remote {
		// detect origin of repo or nil
		// origin	git://github.com/supercollider-quarks/MathLib (fetch)
		// origin	git://github.com/supercollider-quarks/MathLib (push)
		var out = this.git(["remote -v"]),
			match = out.findRegexp("^origin\t([^ ]+) \\(fetch\\)");
		if(match.size > 0, {
			^match[1][1]
		});
		^nil
	}
	refspec {
		var
			out = this.git(["log --pretty=format:'%d' --abbrev-commit --date=short -1 | cat"]),
			match = out.findRegexp("tag: ([a-zA-Z0-9\.\-_]+)");
		if(match.size > 0, {
			^"tags/" ++ match[1][1]
		});
		out = this.git(["rev-parse HEAD"]);
		^out.copyRange(0, out.size - 2)
	}
	remoteLatest {
		var out = this.git(["rev-parse origin/master"]);
		^out.copyRange(0, out.size - 2)
	}
	tags {
		^this.git(["tag"]).split(Char.nl).select({ |t| t.size != 0 })
	}
	git { |args, cd=true|
		var cmd;
		if(cd, {
			cmd = ["cd", localPath.escapeChar($ ), "&&", "git"];
		},{
			cmd = ["git"];
		});
		cmd = (cmd ++ args).join(" ");
		^cmd.debug.unixCmdGetStdOut;
	}
}

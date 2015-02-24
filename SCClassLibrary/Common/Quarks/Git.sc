
Git {
	var <>localPath, >url, tag, sha, remoteLatest, tags;

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
		this.url = url;
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
		^this.git(["--no-pager diff HEAD --"]).size != 0
	}
	url {
		^url ?? {
			url = this.remote;
		}
	}
	remote {
		// detect origin of repo or nil
		// origin	git://github.com/supercollider-quarks/MathLib (fetch)
		// origin	git://github.com/supercollider-quarks/MathLib (push)
		// problem: if more than one remote then this will just detect the first
		// should favor 'origin' if more than one
		var out = this.git(["remote -v"]),
			match = out.findRegexp("^[a-zA-Z0-9]+\t([^ ]+) \\(fetch\\)");
		if(match.size > 0, {
			^match[1][1]
		});
		^nil
	}
	refspec {
		^this.tag ?? { this.sha }
	}
	tag {
		var out, match;
		^tag ?? {
			out = this.git(["--no-pager log --pretty=format:'%d' --abbrev-commit --date=short -1"]);
			match = out.findRegexp("tag: ([a-zA-Z0-9\.\-_]+)");
			if(match.size > 0, {
				tag = "tags/" ++ match[1][1]
			});
			tag
		}
	}
	sha {
		var out;
		^sha ?? {
			out = this.git(["rev-parse HEAD"]);
			sha = out.copyRange(0, out.size - 2)
		}
	}
	remoteLatest {
		var out;
		remoteLatest ?? {
			out = this.git(["rev-parse origin/master"]);
			remoteLatest = out.copyRange(0, out.size - 2)
		}
	}
	tags {
		^tags ?? {
			tags = this.git(["tag"]).split(Char.nl).select({ |t| t.size != 0 })
		}
	}
	git { |args, cd=true|
		var cmd;
		if(cd, {
			cmd = ["cd", localPath.escapeChar($ ), "&&", "git"];
		},{
			cmd = ["git"];
		});
		cmd = (cmd ++ args).join(" ");
		^cmd.unixCmdGetStdOut;
	}
}

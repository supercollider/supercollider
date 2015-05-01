
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
		tags = remoteLatest = nil;
		this.git(["fetch"]);
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
		// find what tag is currently checked out
		var out, match;
		^tag ?? {
			out = this.git(["--no-pager log --pretty=format:'%d' --abbrev-commit --date=short -1"]);
			match = out.findRegexp("tag: ([^ ,\)]+)");
			if(match.size > 0, {
				tag = "tags/" ++ match[1][1]
			});
			tag
		}
	}
	sha {
		// find what hash is currently checked out
		var out;
		^sha ?? {
			out = this.git(["rev-parse HEAD"]);
			sha = out.copyRange(0, out.size - 2)
		}
	}
	remoteLatest {
		// find what the latest commit on the remote is
		var out;
		^remoteLatest ?? {
			out = this.git(["rev-parse origin/master"]);
			remoteLatest = out.copyRange(0, out.size - 2)
		}
	}
	tags {
		var raw;
		// all tags
		// only includes ones that have been fetched from remote
		^tags ?? {
			raw = this.git(["for-each-ref --format='%(refname)' --sort=taggerdate refs/tags"]);
			tags = raw.split(Char.nl)
				.select({ |t| t.size != 0 })
				.reverse()
				.collect({ |t| t.copyToEnd(10)});
		}
	}
	shaForTag { |tag|
		^this.git(["rev-list", tag]).copyFromStart(39)
	}
	git { |args, cd=true|
		var cmd;
		if(cd, {
			cmd = ["cd", localPath.escapeChar($ ), "&&", "git"];
		},{
			cmd = ["git"];
		});
		cmd = (cmd ++ args).join(" ");
		// this blocks the app thread
		^cmd.unixCmdGetStdOut;
	}
}

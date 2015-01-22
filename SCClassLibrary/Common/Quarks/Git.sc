
Git {

	*clone { |url, localPath|
		this.git([
			"clone",
			url,
			localPath.escapeChar($ )
		]);
	}
	*pull { |localPath|
		this.git(["pull"], localPath)
	}
	*checkout { |refspec, localPath|
		this.git(["checkout", refspec], localPath)
	}
	*fetch { |localPath|
		this.git(["fetch"], localPath)
	}
	*isGit { |localPath|
		^File.exists(localPath +/+ ".git")
	}
	*isDirty { |localPath|
		var out = this.git(["--no-pager diff HEAD --"], localPath);
		if(out.size != 0, {
			out.debug;
			^true
		});
		^false
	}
	*remote { |localPath|
		// detect origin of repo or nil
		// origin	git://github.com/supercollider-quarks/MathLib (fetch)
		// origin	git://github.com/supercollider-quarks/MathLib (push)
		var out = this.git(["remote -v"], localPath),
			match = out.findRegexp("^origin\t([^ ]+) \\(fetch\\)");
		if(match.size > 0, {
			^match[1][1]
		});
		^nil
	}
	*refspec { |localPath|
		var
			out = this.git(
				["log --pretty=format:'%d' --abbrev-commit --date=short -1 | cat"],
				localPath,
				true),
			match = out.findRegexp("tag: ([a-zA-Z0-9\.\-_]+)");
		if(match.size > 0, {
			^"tags/" ++ match[1][1]
		});
		out = this.git(["rev-parse HEAD"], localPath);
		^out.copyRange(0, out.size - 2)
	}
	*remoteLatest { |localPath|
		var out = this.git(["rev-parse origin/master"], localPath);
		^out.copyRange(0, out.size - 2)
	}
	*tags { |localPath|
		^this.git(["tag"], localPath).split(Char.nl).select({ |t| t.size != 0 })
	}
	*git { |args, cd|
		var cmd;
		if(cd.notNil, {
			cmd = ["cd", cd.escapeChar($ ), "&&", "git"];
		},{
			cmd = ["git"];
		});
		cmd = (cmd ++ args).join(" ");
		^cmd.debug.unixCmdGetStdOut;
	}
}

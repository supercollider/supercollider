
Quark {
	var <name, url, >refspec, data, <localPath;
	var <changed = false, <git;

	*new { |name, refspec, url, localPath|
		var args = Quark.parseQuarkName(name, refspec, url, localPath);
		if(args.isNil, {
			Error("Quark '%' not found".format(name)).throw;
		});
		^super.new.init(*args)
	}
	*fromLocalPath { |path|
		var name, url, refspec;
		path = Quarks.quarkNameAsLocalPath(path);
		name = path.basename;
		^super.new.init(name, url, refspec, path)
	}
	*fromDirectoryEntry { |name, directoryEntry|
		var url, refspec;
		# url, refspec = directoryEntry.split($@);
		^super.new.init(name, url, refspec)
	}
	init { |argName, argUrl, argRefspec, argLocalPath|
		name = argName;
		url = argUrl;
		refspec = argRefspec;
		localPath = argLocalPath ?? {Quarks.folder +/+ name};
		if(Git.isGit(localPath), {
			git = Git(localPath);
		});
	}
	data {
		^data ?? {
			data = this.parseQuarkFile() ?? { IdentityDictionary.new }
		}
	}
	version {
		^this.data['version']
	}
	summary {
		^this.data['summary']
	}
	url {
		^url ?? {
			url = git !? { git.url }
		}
	}
	refspec {
		^refspec ?? {
			git !? { git.refspec }
		}
	}
	tags {
		^git !? { git.tags }
	}
	isDownloaded {
		^File.exists(this.localPath)
	}
	isGit {
		^git.notNil
	}
	isInstalled {
		^Quarks.pathIsInstalled(this.localPath)
	}
	isCompatible {
		var isCompatible = true;
		if(this.data.includesKey('isCompatible'), {
			{
				isCompatible = this.data['isCompatible'].value !== false
			}.try({ |error|
				("Failed to evalute quarkfile data field: isCompatible" + this).error;
				error.reportError;
			});
		});
		^isCompatible
	}

	install {
		var success = Quarks.installQuark(this);
		changed = true;
		data = nil;
		^success
	}
	update {
		if(Git.isGit(localPath), {
			data = git = refspec = nil;
			changed = true;
			git = Git(localPath);
			git.pull();
			git.checkout("master");
			("Quark '%' updated to version: % tag: % refspec: %".format(name, this.version, this.git.tag, this.refspec)).postln;
		}, {
			("Quark" + name + "was not installed using git, cannot update.").warn;
		});
	}
	uninstall {
		Quarks.uninstallQuark(this);
		changed = true;
	}

	checkout {
		var rs;
		if(git.isNil, {
			git = Git(localPath);
		});
		if(this.isDownloaded.not, {
			if(this.url.isNil, {
				Error("No git url, cannot checkout quark" + this).throw;
			});
			git.clone(url);
			// get tags etc
			git.fetch();
			changed = true;
			data = nil;
		});
		if(refspec.notNil, {
			if(this.isDownloaded
				and: {git.notNil}
				and: {git.isDirty},
			{
				("% has uncommited changes, cannot checkout %".format(name, refspec ? "")).warn;
			}, {
				if(refspec == "HEAD", {
					git.pull()
				}, {
					rs = this.validateRefspec(refspec);
					if(rs.notNil, {
						git.checkout(rs)
					});
				});
			});
			changed = true;
			data = nil;
		});
	}
	validateRefspec { |refspec|
		var tag;
		if(refspec == "HEAD"
			or: {refspec.findRegexp("^[a-zA-Z0-9]{40}$").size != 0}, {
			^refspec
		});
		if(refspec.beginsWith("tags/").not, {
			tag = refspec;
			refspec = "tags/" ++ tag;
		}, {
			tag = refspec.copyToEnd(5);
		});
		if(this.tags.includesEqual(tag).not, {
			("Tag not found:" + this + tag + Char.nl + "Possible tags:" + this.tags).warn;
			^nil
		});
		^refspec
	}
	checkForUpdates {
		var tags;
		if(this.isGit, {
			tags = git.tags().sort;
			git.fetch();
			// if not already marked as changed
			if(changed.not, {
				// changed if there are new tags
				changed = git.tags().sort != tags;
			})
		});
	}

	dependencies {
		var deps = this.data['dependencies'] ?? {^[]};
		if(deps.isSequenceableCollection.not, {
			("Invalid dependencies " + this + deps).warn;
			^[]
		});
		^deps.collect({ |dep|
			var q = Quark.parseDependency(dep, this);
			if(q.isNil, {
				"% not found".format(dep).warn;
			});
			q
		}).select({ |it| it.notNil });
	}
	deepDependencies {
		// warning: everything must be checked out just to get the dependency list
		^this.prCollectDependencies(Dictionary.new).values
	}
	prCollectDependencies { |collector|
		this.dependencies.do({ |q|
			var prev = collector[q.name];
			if(prev.notNil, {
				if(prev.refspec != q.refspec, {
					("% requires % but % is already registered as a dependency".format(this, q, prev)).warn;
				});
			}, {
				q.checkout();
				collector[q.name] = q;
				q.prCollectDependencies(collector);
			});
		});
		^collector
	}
	*parseDependency { |dep, forQuark|
		// parse a dependency specifier and return a Quark
		// (1) string
		if(dep.isString, {
			^Quark.prMakeDep(*dep.split($@))
		});
		// (2) name -> version
		if(dep.isKindOf(Association), {
			^Quark.prMakeDep(dep.key.asString)
		});
		// (3) [name, version, url]
		// url is svn repo and should be ignored now
		if(dep.isSequenceableCollection, {
			^Quark.prMakeDep(dep.first)
		});
		("Cannot parse dependency:" + dep + (forQuark ? "")).error;
		^nil
	}
	*prMakeDep { |name, refspec|
		// protected make dependency
		// if not found then posts error and returns nil
		var args = Quark.parseQuarkName(name, refspec);
		if(args.isNil, {
			^nil
		});
		^super.new.init(*args)
	}
	*parseQuarkName { |name, refspec, url, localPath|
		// determine which quark the string 'name' refers to
		// name is one of: quarkname, url, localPath
		// returns: [name, url, refspec, localPath]
		var directoryEntry;
		if(name.contains("://"), {
			^[PathName(name).fileNameWithoutExtension(), name, refspec, localPath]
		});
		if(Quarks.isPath(name), {
			// if not provided then url can be determined later by the Quark
			^[name.basename, url, refspec, name]
		});
		// search Quarks folders
		(Quarks.additionalFolders ++ [Quarks.folder]).do({ |f|
			var lp = localPath ?? {f +/+ name};
			if(File.existsCaseSensitive(lp), {
				^[name, url, refspec, lp]
			});
		});
		// lookup url in directory
		directoryEntry = Quarks.findQuarkURL(name);
		if(directoryEntry.notNil, {
			directoryEntry = directoryEntry.split($@);
			^[name, url ? directoryEntry[0], refspec ? directoryEntry[1], localPath]
		});
		// not found
		^nil
	}
	parseQuarkFile {
		var qfp = this.localPath +/+ name ++ ".quark",
			result;
		if(File.exists(qfp), {
			result = thisProcess.interpreter.compileFile(qfp).value;
			if(result.isNil, {
				("Failed to parse %".format(qfp)).warn;
			});
		});
		^result
	}

	printOn { arg stream;
		var v = this.version ? this.refspec;
		stream << "Quark: " << name;
		if(v.notNil,{ stream << "[" << v << "]" });
	}
	help {
		var p = this.data['schelp'];
		// explicit pointer to a help file in quark data
		if(p.notNil, {
			^HelpBrowser.openHelpFor(p);
		});
		// old html help doc
		p = this.data['helpdoc'];
		if(p.notNil, {
			(this.localPath +/+ p).openOS;
			// ^HelpBrowser.goTo(URI.fromLocalPath(this.localPath +/+ p).asString);
		});
		HelpBrowser.openBrowsePage("Quarks>" ++ name);
	}
}

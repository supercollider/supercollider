
Quark {
	var <name, url, >refspec, data, <localPath;
	var <changed = false, <git;

	*new { |name, refspec|
		var args = Quark.parseQuarkName(name, refspec);
		if(args.isNil, {
			Error("% not found".format(name)).throw;
		});
		^super.new.init(*args)
	}
	*fromLocalPath { |path|
		var name, url, refspec;
		path= Quarks.quarkNameAsLocalPath(path);
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
		^this.data['isCompatible'].value !== false
	}

	install {
		var success = Quarks.installQuark(this);
		changed = true;
		data = nil;
		^success
	}
	uninstall {
		Quarks.uninstallQuark(this);
		changed = true;
	}

	checkout {
		if(this.isDownloaded.not, {
			if(this.url.isNil, {
				Error("No git url, cannot checkout quark" + this).throw;
			});
			git = Git(localPath);
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
					// when do you have to fetch ?
					// if offline then you do not want to fetch
					// just checkout. fast switching
					git.checkout(refspec)
				});
			});
			changed = true;
			data = nil;
		});
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
	*parseQuarkName { |name, refspec|
		// determine which quark the string 'name' refers to
		// name is one of: quarkname, url, localPath
		// returns: [name, url, refspec, localPath]
		var directoryEntry;
		if(name.contains("://"), {
			^[PathName(name).fileNameWithoutExtension(), name, refspec, nil]
		});
		if(Quarks.isPath(name), {
			// url can be determined later by the Quark
			^[name.basename, nil, refspec, name]
		});
		// search Quarks folders
		(Quarks.additionalFolders ++ [Quarks.folder]).do({ |f|
			var localPath = f +/+ name, url;
			if(File.existsCaseSensitive(localPath), {
				^[name, nil, refspec, localPath]
			});
		});
		// lookup url in directory
		directoryEntry = Quarks.findQuarkURL(name);
		if(directoryEntry.notNil, {
			directoryEntry = directoryEntry.split($@);
			^[name, directoryEntry[0], refspec ? directoryEntry[1], nil]
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

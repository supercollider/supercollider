
Quark {
	var <name, url, >refspec, data, <localPath;
	var <changed = false, <git;

	*new { |name, refspec|
		^super.new.init(*Quark.parseQuarkName(name, refspec))
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
			git.refspec
		}
	}
	tags {
		^git.tags
	}
	isDownloaded {
		^File.exists(this.localPath)
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

	dependencies {
		var deps = this.data['dependencies'] ?? {^[]};
		if(deps.isSequenceableCollection.not, {
			("Invalid dependencies " + this + deps).warn;
			^[]
		});
		^deps.collect({ |dep| this.parseDependency(dep) }).select(_.notNil);
	}
	deepDependencies {
		var deps = Dictionary.new;
		this.dependencies.do({ |q|
			q.checkout();
			q.deepDependencies.debug(q).do({ |qb|
				deps[qb.name] = qb;
			});
			deps[q.name] = q;
		});
		^deps.values
	}
	parseDependency { arg dep;
		// (1) string
		var name, version, url, q;
		if(dep.isString, {
			{
				q = Quarks.at(dep)
			}.try({ |err|
				// bad name, or quark not found
				err.errorString.postln;
				(this.asString + "failed to find dependency" + dep.asCompileString).warn;
			});
			^q
		});
		// support older styles:
		// (2) name->version
		if(dep.isKindOf(Association), {
			name = dep.key.asString;
			version = dep.value;
			q = Quarks.at(name);
			// and what if version is different ?
			^Quark(name)
		});
		// (3) [name, version, url]
		// url is likely to be an svn repo
		if(dep.isSequenceableCollection, {
			# name, version, url = dep;
			^Quark(url + name, version);
		});
		Error("Cannot parse dependency:" + this + dep).throw;
	}

	*parseQuarkName { |name, refspec|
		// determine which quark the string 'name' refers to
		// and return name, url, refspec, localPath
		// name is one of: quarkname, url, localPath
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
			if(File.exists(localPath), {
				^[name, nil, refspec, localPath]
			});
		});
		// lookup url in directory
		directoryEntry = Quarks.findQuarkURL(name);
		if(directoryEntry.notNil, {
			directoryEntry = directoryEntry.split($@);
			^[name, directoryEntry[0], refspec ? directoryEntry[1], nil]
		});
		Error("% not found".format(name)).throw;
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
		stream << "Quark: " << name;
		if(this.version.notNil,{ stream << " [" << this.version << "]"; });
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

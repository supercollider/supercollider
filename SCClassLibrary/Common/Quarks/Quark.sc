
Quark {
	var <name, <url, >refspec, data, <localPath;
	var <changed = false;

	*new { |name, refspec|
		var url;
		# name, url, refspec = Quark.parseQuarkName(name, refspec);
		^super.new.init(name, url, refspec)
	}
	*fromLocalPath { |path|
		var name, url, refspec;
		name = path.basename;
		if(Git.isGit(path), {
			url = Git.remote(path);
		});
		// refspec if git
		^super.new.init(name, url, refspec, path)
	}
	*fromDirectoryEntry { |name, quarkUrl|
		var url, refspec;
		# url, refspec = quarkUrl.split($@);
		^super.new.init(name, url, refspec)
	}
	init { |argName, argUrl, argRefspec, argLocalPath|
		name = argName;
		url = argUrl;
		refspec = argRefspec;
		localPath = argLocalPath ?? {Quarks.folder +/+ name};
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
	refspec {
		^refspec ?? {
			Git.refspec(this.localPath)
		}
	}
	tags {
		^Git.tags(this.localPath)
	}
	isDownloaded {
		^File.exists(this.localPath)
	}
	isInstalled {
		^Quarks.isInstalled(name)
	}
	isCompatible {
		^this.data['isCompatible'].value !== false
	}

	install {
		Quarks.installQuark(this);
		changed = true;
		data = nil;
	}
	uninstall {
		Quarks.uninstall(this.name);
		changed = true;
	}

	checkout {
		var localPath = this.localPath;
		if(this.isDownloaded
			and: {Git.isGit(localPath)}
			and: {Git.isDirty(localPath)},
		{
			Error("% has uncommited changes, cannot checkout %".format(name, refspec ? "")).throw;
		});
		if(url.isNil and: {Git.isGit(localPath)}, {
			url = Git.remote(localPath);
			if(url.isNil, {
				Error("No git url, cannot checkout quark" + this).throw;
			});
		});
		if(this.isDownloaded.not, {
			Git.clone(url, localPath);
			// get tags etc
			Git.fetch(localPath);
			changed = true;
			data = nil;
		});
		if(refspec.notNil, {
			Git.checkout(refspec, localPath);
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
		^deps.collect({ |dep| this.parseDependency(dep) });
	}
	deepDependencies {
		var deps = this.dependencies, all = List.new;
		all.addAll(deps);
		deps.do { |q|
			q.checkout();
			all.addAll(q.deepDependencies);
		};
		^all
	}
	parseDependency { arg dep;
		// (1) string
		// support older styles:
		// (2) name->version
		// (3) [name, version, url]
		var name, version, url;
		if(dep.isString, {
			^Quark(dep)
		});
		if(dep.isKindOf(Association), {
			^Quark(dep.key.asString, dep.value)
		});
		if(dep.isSequenceableCollection, {
			# name, version, url = dep;
			^Quark(url + name, version);
		});
		Error("Cannot parse dependency:" + this + dep).throw;
	}

	*parseQuarkName { |name, refspec|
		// parse any URLS and @tags
		var quarkUrl, url, localPath;
		if(name.contains("://"), {
			url = name;
			name = PathName(url).fileNameWithoutExtension();
		}, {
			// lookup url in directory
			quarkUrl = Quarks.findQuarkURL(name);
			if(quarkUrl.isNil, {
				localPath = Quarks.folder +/+ name;
				if(File.exists(localPath), {
					// a quark not in the index
					// referred to by just its name
					// so assumed to be downloaded or previously cloned
					// or we are offline and there is no cached directory.txt
					if(Git.isGit(localPath), {
						url = Git.remote(localPath);
					});
					^[name, url, refspec]
				}, {
					Error("% not found".format(name)).throw;
				});
			});
			quarkUrl = quarkUrl.split($@);
			url = quarkUrl[0];
			refspec = refspec ? quarkUrl[1];
		});
		^[name, url, refspec]
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
			^HelpBrowser.goTo(URI.fromLocalPath(this.localPath +/+ p).asString);
		});
		HelpBrowser.openBrowsePage("Quarks>" ++ name);
	}
}

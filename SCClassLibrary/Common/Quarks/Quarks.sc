
Quarks {

	classvar
		<folder,
		directory,
		<>directoryUrl="https://raw.githubusercontent.com/supercollider-quarks/quarks/master/directory.txt";

	*install { |name, refspec|
		var path;
		if(name.findRegexp("^[~\.]?/").size == 0, {
			this.installQuark(Quark(name, refspec));
		}, {
			// local path / ~/ ./
			path = this.asAbsolutePath(name);
			if(File.exists(path).not, {
				("Path does not exist" + path).warn;
			});
			this.link(path);
		});
	}
	*uninstall { |name|
		// by quark name or by supplying a local path
		// resolving / ~/ ./
		this.unlink(this.quarkNameAsLocalPath(name));
	}
	*clear {
		this.installed.do({ |quark|
			LanguageConfig.removeIncludePath(quark.localPath);
		});
		LanguageConfig.store();
	}
	*load { |path|
		// install a list of quarks from a text file
		var file, line, dir;
		this.clear();
		path = this.asAbsolutePath(path);
		dir = path.dirname;
		file = File.open(path, "r");
		while({
			line = file.getLine();
			line.notNil;
		}, {
			var name, refspec;
			// resolve any paths relative to the quark file
			# name, refspec = line.split($=);
			if(name.findRegexp("^[~\.]?/").size != 0, {
				this.install(this.asAbsolutePath(name, dir), refspec);
			}, {
				this.install(name, refspec);
			});
		});
	}
	*save { |path|
		// save currently installed quarks to a text file
		var file, dir;
		path = this.asAbsolutePath(path);
		file = File.open(path, "w");
		dir = path.dirname;
		this.installed.do({ |quark|
			var qpath, d, refspec;
			if(Git.isGit(quark.localPath), {
				// quark name if in directory and is using same repo
				// else git url
				qpath = quark.url;
				if(Git.isDirty(quark.localPath), {
					("Working copy is dirty" + quark.localPath).warn;
				}, {
					refspec = quark.refspec;
				});
			}, {
				// ./path if quark is inside the folder where the quark file is
				if(quark.localPath.beginsWith(dir), {
					qpath = "." ++ quark.localPath.copyToEnd(dir.size);
				}, {
					d = Platform.userHomeDir;
					// ~/path if in home
					if(quark.localPath.beginsWith(d), {
						qpath = "~" ++ quark.localPath.copyToEnd(d.size);
					}, {
						// absolute path
						qpath = quark.localPath;
					});
				});
			});
			if(refspec.notNil, {
				file.write("%=%\n".format(qpath, refspec));
			}, {
				file.write("%\n".format(qpath));
			});
		});
		file.close();
	}
	*update { |name|
		// by quark name or by supplying a local path
		// resolving / ~/ ./
		Git.update(this.quarkNameAsLocalPath(name));
	}
	*installed {
		^LanguageConfig.includePaths
			.collect(Quark.fromLocalPath(_))
	}
	*isInstalled { |name|
		^LanguageConfig.includePaths.any({ |path|
			path.withoutTrailingSlash.endsWith(name)
		})
	}
	*openFolder {
		this.folder.openOS;
	}
	*gui {
		^QuarksGui.new
	}

	*installQuark { |quark|
		var
			deps,
			incompatible = { arg name;
				(quark.name
					+ "reports an incompatibility with this Super Collider version"
					+ "or with other already installed quarks."
				).inform;
			};

		quark.checkout();
		if(quark.isCompatible().not, {
			^incompatible.value(quark.name);
		});
		deps = quark.deepDependencies;
		deps.do({ |dep|
			if(dep.isCompatible().not, {
				^incompatible.value(dep.name);
			});
			dep.checkout();
		});
		deps.do({ |dep|
			this.link(dep.localPath);
		});
		this.link(quark.localPath);
		(quark.name + "installed").inform;
	}

	*link { |path|
		path = path.withoutTrailingSlash;
		if(LanguageConfig.includePaths.includesEqual(path).not, {
			path.debug("Adding path");
			LanguageConfig.addIncludePath(path);
			LanguageConfig.store();
			^true
		});
		^false
	}
	*unlink { |path|
		path = path.withoutTrailingSlash;
		if(LanguageConfig.includePaths.includesEqual(path), {
			path.debug("Removing path");
			LanguageConfig.removeIncludePath(path);
			LanguageConfig.store();
			^true
		});
		^false
	}

	*initClass {
		folder = Platform.userAppSupportDir +/+ "downloaded-quarks";
		if(File.exists(folder).not, {
			folder.mkdir();
		});
	}
	*findQuarkURL { arg name;
		^this.directory[name]
	}
	*directory {
		// just urls
		if(directory.isNil, {
			this.fetchDirectory(false);
		});
		^directory
	}
	*all {
		// all known quarks
		var all = Dictionary.new, f;
		// those in index
		this.directory.keysValuesDo({ |name, quarkURL|
			all[name] = Quark.fromDirectoryEntry(name, quarkURL);
		});
		f = { |path|
			var quark;
			if(File.type(path) == \directory, {
				quark = Quark.fromLocalPath(path);
				all[quark.name] = quark;
			});
		};

		// this means alternate versions are hidden
		// those checked out in folder or linked in language config
		(Quarks.folder +/+ "*").pathMatch.do(f);
		// running many duplicates, but do not want to miss any that were linked
		LanguageConfig.includePaths.do(f);
		^all.values
	}
	*fetchDirectory { |force=true|
		// TODO: needs a cross platform download
		var dirCachePath = Quarks.folder +/+ "directory.txt",
			fetch = true;
		// what if you are offline ?
		if(File.exists(dirCachePath), {
			fetch = force or: {
				(Date.getDate().rawSeconds) - File.mtime(dirCachePath) > (60 * 60 * 4)
			};
		});
		{
			this.prReadFile(fetch, dirCachePath);
		}.try({ arg err;
			("Failed to fetch quarks directory listing: % %".format(if(fetch, directoryUrl, dirCachePath), err)).error;
			// if fetch failed, try read from cache
			// if read from cache failed, try fetching
			if(fetch, {
				 if(File.exists(dirCachePath), {
					this.prReadFile(false, dirCachePath);
				});
			}, {
				this.prReadFile(true, dirCachePath);
			});
		});
	}
	*prReadFile { |fetch, dirCachePath|
		var line, kv, file, cached=List.new;
		directory = Dictionary.new;
		if(fetch, {
			("Fetching" + directoryUrl).debug;
			file = Pipe("curl" + Quarks.directoryUrl, "r");
		}, {
			("Reading" + dirCachePath).debug;
			file = File.open(dirCachePath, "r");
		});
		while({
			line = file.getLine();
			line.notNil;
		}, {
			kv = line.split($=);
			directory[kv[0]] = kv[1];
			cached.add(line);
		});
		file.close;
		if(fetch and: {cached.size > 0}, {
			dirCachePath.debug("writing");
			File.open(dirCachePath, "w").put(cached.join(Char.nl)).close();
		});
	}
	*quarkNameAsLocalPath { |name|
		^if(name.findRegexp("^[~\.]?/").size == 0, {
			Quark(name).localPath
		}, {
			this.asAbsolutePath(name)
		});
	}
	*asAbsolutePath { |path, relativeTo|
		^if(path.at(0).isPathSeparator, {
			path
		}, {
			if(path.at(0) == $~, {
				^path.standardizePath
			});
			// scroot is the cwd at startup
			if(path.beginsWith("./"), {
				^(relativeTo ?? {PathName.scroot}) +/+ path.copyToEnd(2)
			});
			(relativeTo ?? {PathName.scroot}) +/+ path
		})
	}
	// quarks fetch all available quark specs
	// directory.json
}

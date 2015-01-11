
Quarks {

	classvar
		<folder,
		directory,
		<>directoryUrl="https://raw.githubusercontent.com/supercollider-quarks/quarks/master/directory.txt";

	*install { |name, refspec|
		this.installQuark(Quark(name, refspec));
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
		});
		deps.do({ |dep|
			this.link(dep.name);
		});
		this.link(quark.name);
		(quark.name + "installed").inform;
	}
	*uninstall { |name|
		this.unlink(name);
		// TODO and uninstall all dependencies not in use by others
	}
	*update { |name|
		GitQuarks.update(name);
	}
	*installed {
		^LanguageConfig.includePaths
			.select(_.beginsWith(Quarks.folder))
			.collect(Quark(_))
	}
	*isInstalled { |name|
		^LanguageConfig.includePaths.any({ |path|
			path.endsWith(name) and: {
				path.beginsWith(Quarks.folder)
			}
		})
	}
	/*
	*purge { |name, force=false|
		var localPath = Quarks.folder +/+ name;
		if(File.exists(localPath), {
			if(force.not, {
				if(Git.isGit(localPath) and: {Git.isDirty(localPath)}, {
					"Quark has changes. Use Quarks.purge(name, force=true) to force deletion".error;
					^this
				});
			});
			// 	File.delete does not remove directories
			File.delete(localPath);
			("Deleted" + localPath).inform;
		});
	}
	*/

	*link { |name|
		var path = Quarks.folder +/+ name.asString;
		if(LanguageConfig.includePaths.includesEqual(path).not, {
			path.debug("adding");
			LanguageConfig.addIncludePath(path);
			LanguageConfig.store();
			^true
		});
		^false
	}
	*unlink { |name|
		var path = Quarks.folder +/+ name.asString;
		if(LanguageConfig.includePaths.includesEqual(path), {
			path.debug("removing");
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
	*openFolder {
		this.folder.openOS;
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
		var list = List.new;
		this.directory.keysValuesDo({ |name, quarkURL|
			list.add(Quark.fromDirectoryEntry(name, quarkURL))
		});
		// TODO: make a union of directory and any others found in quarks folder
		^list
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
			// fall back to the other method
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
	// quarks fetch all available quark specs
	// directory.json
	*gui {
		^QuarksGui.new
	}
}


+ Main {
	*packages {
		// a package is either a quark or a folder in SCClassLibrary or extensions folders

		// a class is in a 'package' determined by where its Class file is
		// all folders in class library:
			// Common
			// JITlib
			// crucial
			// your own
		// all folders in system extension
		// all folders in user extension
		// all quarks
		// any loose files are placed in a packaged calle SCClassLibrary or Extensions
			// possiblity for error: if you have loose files in user extensions and system extensions
			// they will both be placed in the same package: Extensions

		// [ name -> folderPath, name -> folderPath ... ]

		var platform;
		var packages, f;

		// cache
		packages = Library.at(Quarks, \packages);
		if(packages.notNil,{ ^packages });

		platform = thisProcess.platform;

		f = { arg dir;
			var folders, packages, paths, files, slash = platform.pathSeparator;
			dir = dir.withTrailingSlash;
			paths = (dir ++ "*").pathMatch;
			folders = paths.reject({ |p| p.last != slash or: {p.basename == "quarks"} });
			files = paths.select({ |p| p.last != slash });
			packages = folders.collect({ |p| p.basename.asSymbol -> p });
			if(files.notEmpty, {
				// collect any loose files in a package called dir
				packages = packages.add(dir.basename.asSymbol -> dir)
			});
			packages
		};

		packages = (
			f.value(platform.classLibraryDir)
			++ f.value(platform.systemExtensionDir)
			++ Quarks.installed.collect({ |q|
					q.name.asSymbol -> q.localPath.withTrailingSlash
				})
		).sort({ |a, b| a.value.size > b.value.size }) // sort longer paths first
		++ f.value(platform.userExtensionDir);
		Library.put(Quarks, \packages, packages);
		^packages
	}
}


+ Class {
	package {
		var path = this.filenameSymbol.asString;
		Main.packages.do({ |namepath|
			if(path.copyRange(0, namepath.value.size - 1) == namepath.value, {
				^namepath.key
			})
		});
		Error("Discrepancy: Package not found for class !").throw;
	}
}


+ Method {
	package {
		var path;
		path = this.filenameSymbol.asString;
		Main.packages.do({ |namepath|
			if(path.copyRange(0,namepath.value.size-1) == namepath.value,{
				^namepath.key
			})
		});
		Error("Discrepancy: Package not found for method !").throw;
	}
}


+ Quark {
	definesClasses {
		var localPath = this.localPath;
		^Class.allClasses.select({ |class|
			class.isMetaClass.not and: {
				class.filenameSymbol.asString.beginsWith(localPath)
			}
		})
	}
	definesExtensionMethods {
		// all methods whose path is in this quark folder
		// where the class is not in this quark
		var localPath = this.localPath;
		^Class.allClasses.collect({ |c| c.methods }).reject(_.isNil).flat
			.select({ |method|
				method.filenameSymbol.asString.beginsWith(localPath) and: {
					method.ownerClass.filenameSymbol.asString.beginsWith(localPath).not
				}
			})
	}
}


+ Quarks {
	*classesInPackage { |packageName|
		// the equivalent to Quark-definesClasses
		// but works for non-quarks like Quarks.classesInPackage("JITlib")
		var myPath, end, package;
		package = Main.packages.detect({ |pk| pk.key == packageName });
		if(package.isNil,{ Error("Package not found:"+packageName).throw });

		myPath = package.value;
		end = myPath.size - 1;
		^Class.allClasses.select({ |class|
			class.isMetaClass.not and: {
				class.filenameSymbol.asString.copyRange(0, end) == myPath
			}
		})
	}
}

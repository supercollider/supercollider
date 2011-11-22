+ Main {
	// a package is either a quark or a folder in SCClassLibrary or extensions folders

	// a class is in a 'package' determined by where its Class file is
	// all folders in class library :
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
	*packages {

		var platform,scClassLibrary,looseFiles;
		var packages,f;

		// cache
		packages = Library.at(Quarks,\packages);
		if(packages.notNil,{ ^packages });

		platform = thisProcess.platform;

		f = { arg dir;
			var folders,packages,paths,files;
			dir = dir.withTrailingSlash;
			paths = (dir++"*").pathMatch;
			folders = paths.reject({ |p| p.last != $/ or: {p.basename == "quarks"} });
			files = paths.select({ |p| p.last != $/ });
			packages = folders.collect({ |p| p.basename.asSymbol -> p });
			if(files.notEmpty,{
				// if there are any loose files then create a package called dir
				packages = packages.add( dir.basename.asSymbol -> dir )
			});
			packages
		};

		packages =
			(f.value(platform.classLibraryDir)

			++

			f.value(platform.systemExtensionDir)

			++

			Quarks.installed.collect({ |q|
				q.name.asSymbol -> (Platform.userExtensionDir +/+ "quarks" +/+ q.path.withTrailingSlash)
			}))
			.sort({ |a,b| a.value.size > b.value.size }) // sort longer paths first

			++

			f.value(platform.userExtensionDir);
		Library.put(Quarks,\packages,packages);
		^packages
	}
}

+ Class {
	package {
		var path;
		path = this.filenameSymbol.asString;
		Main.packages.do({ |namepath|
			if(path.copyRange(0,namepath.value.size-1) == namepath.value,{
				^namepath.key
			})
		});
		Error("Discrepancy: Package not found for class !").throw;
	}
}

/*
	method
		extensions:
			may not be the package that its class is in
*/
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
		var myPath,end;
		myPath = Platform.userExtensionDir +/+ "quarks" +/+ this.path;
		end = myPath.size-1;
		^Class.allClasses.reject(_.isMetaClass).select({ |class|
			class.filenameSymbol.asString.copyRange(0,end) == myPath
		})
	}
	definesExtensionMethods {
		// all methods whose path is in this quark folder
		// where the class is not in this quark
		var myPath,end;
		myPath = Platform.userExtensionDir +/+ "quarks" +/+  this.path;
		end = myPath.size-1;
		^Class.allClasses.collect({ |c| c.methods }).reject(_.isNil).flat
			.select({ |method|
				method.filenameSymbol.asString.copyRange(0,end) == myPath
				and: {
					method.ownerClass.filenameSymbol.asString.copyRange(0,end) != myPath
				}
			})
	}
	// of the classes you defined, what packages are the superclasses in ?
	// of the extension methods you defined, what packages are the super classes in ?
	/*	checkDependencies {

	}*/
}

+ Quarks {
	// the equivalent to Quark-definesClasses
	// but works for non-quarks like Quarks.classesInPackage("JITlib")
	*classesInPackage { |packageName|
		var myPath,end,package;
		package = Main.packages.detect({ |pk| pk.key == packageName });
		if(package.isNil,{ Error("Package not found:"+packageName).throw });

		myPath = package.value;
		end = myPath.size-1;
		^Class.allClasses.select({ |class|
			class.isMetaClass.not and: {
				class.filenameSymbol.asString.copyRange(0,end) == myPath
			}
		})
	}
}

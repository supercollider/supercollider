Platform
{
	var <classLibraryDir, <helpDir, <>recordingsDir, features;

	var <>devpath;

	initPlatform {
		classLibraryDir = thisMethod.filenameSymbol.asString.dirname.dirname;
		helpDir = thisMethod.filenameSymbol.asString.dirname.dirname.dirname ++ "/Help";
		features = IdentityDictionary.new;
		recordingsDir = this.userAppSupportDir +/+ "Recordings";
	}

	name { ^this.subclassResponsibility }

	recompile { ^this.subclassResponsibility }
	*case { | ... cases |
		^thisProcess.platform.name.switch(*cases)
	}

	// directories
	*classLibraryDir { ^thisProcess.platform.classLibraryDir }
	*helpDir { ^thisProcess.platform.helpDir }

	systemAppSupportDir { _Platform_systemAppSupportDir }
	*systemAppSupportDir { ^thisProcess.platform.systemAppSupportDir }

	userAppSupportDir { _Platform_userAppSupportDir }
	*userAppSupportDir { ^thisProcess.platform.userAppSupportDir }

	systemExtensionDir { _Platform_systemExtensionDir }
	*systemExtensionDir { ^thisProcess.platform.systemExtensionDir }

	userExtensionDir { _Platform_userExtensionDir }
	*userExtensionDir { ^thisProcess.platform.userExtensionDir }

	// The "ideName" is for ide-dependent compilation.
	// From SC.app, the value is "scapp" meaning "scide_scapp" folders will be compiled and other "scide_*" ignored.
	ideName { _Platform_ideName }
	*ideName { ^thisProcess.platform.ideName }

	platformDir { ^this.name.asString }
	*platformDir { ^thisProcess.platform.platformDir }

	pathSeparator { ^this.subclassResponsibility }
	*pathSeparator { ^thisProcess.platform.pathSeparator }

	isPathSeparator { |char| ^this.subclassResponsibility }
	*isPathSeparator { |char| ^thisProcess.platform.isPathSeparator(char) }

	clearMetadata { |path| ^this.subclassResponsibility }
	*clearMetadata { |path| ^thisProcess.platform.clearMetadata(path) }
	
	getMouseCoords { ^this.subclassResponsibility }
	*getMouseCoords { ^thisProcess.platform.getMouseCoords }

	// startup/shutdown hooks
	startup { }
	shutdown { }

	startupFiles { ^#[] }
	loadStartupFiles { this.startupFiles.do{|afile|
		afile = afile.standardizePath;
		if(File.exists(afile), {afile.load})
		}
	}

	// features
	declareFeature { | aFeature |
		var str = aFeature.asString;
		if (str.first.isUpper) {
			Error("cannot declare class name features").throw;
		};
		if (str.first == $_) {
			Error("cannot declare primitive name features").throw;
		};
		features.put(aFeature, true);
	}
	hasFeature { | symbol |
		if (features.includesKey(symbol).not) {
			features.put(
				symbol,
				symbol.asSymbol.asClass.notNil or: { symbol.isPrimitive }
			)
		};
		^features.at(symbol)
	}
	when { | features, ifFunction, elseFunction |
		^features.asArray.inject(true, { |v,x|
			v and: { this.hasFeature(x) }
		}).if(ifFunction, elseFunction)
	}
	*when {  | features, ifFunction, elseFunction |
		^thisProcess.platform.when(features, ifFunction, elseFunction)
	}

	// swing is compatible with all platforms; so declare it as global default
	defaultGUIScheme { ^\swing }
	defaultHIDScheme { ^\none }

	isSleeping { ^false } // unless defined otherwise

	// used on systems to deduce a svn directory path, if system wide location is used for installed version. (tested on Linux).
	devLoc{ |inpath|
		var outpath;
		if ( devpath.isNil ){ ^inpath };
		outpath = inpath.copyToEnd( inpath.find( "SuperCollider") );
		outpath = outpath.replace( "SuperCollider", devpath +/+ "build");
		^outpath;
	}

}

UnixPlatform : Platform
{
	pathSeparator { ^$/ }

	isPathSeparator { |char|
		^(char === this.pathSeparator)
	}

	clearMetadata { |path|
		"rm -f %\.*meta".format(path.splitext[0].escapeChar($ )).systemCmd;
	}

	arch {
		var pipe, arch;
		pipe = Pipe("arch", "r");
		arch = pipe.getLine;
		pipe.close;
		^arch.asSymbol;
	}
}

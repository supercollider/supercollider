Platform {
	classvar defaultStartupFile;

	// IDE actions
	classvar <>makeServerWindowAction, <>makeSynthDescWindowAction, <>openHelpFileAction, <>openHTMLFileAction;

	var <classLibraryDir, <helpDir, <>recordingsDir, features;
	var <>devpath;

	*initClass {
		defaultStartupFile = this.userConfigDir +/+ "startup.scd"
	}

	initPlatform {
		classLibraryDir = thisMethod.filenameSymbol.asString.dirname.dirname;
		helpDir = thisMethod.filenameSymbol.asString.dirname.dirname.dirname ++ "/Help";
		features = IdentityDictionary.new;
		recordingsDir = this.userAppSupportDir +/+ "Recordings";
	}

	name { ^this.subclassResponsibility }

	recompile {
		_Recompile
		^this.primitiveFailed
	}
	*case { | ... cases |
		^thisProcess.platform.name.switch(*cases)
	}

	// directories
	*classLibraryDir { ^thisProcess.platform.classLibraryDir }
	*helpDir { ^thisProcess.platform.helpDir }

	userHomeDir {
		_Platform_userHomeDir
		^this.primitiveFailed
	}
	*userHomeDir { ^thisProcess.platform.userHomeDir }

	systemAppSupportDir {
		_Platform_systemAppSupportDir
		^this.primitiveFailed
	}
	*systemAppSupportDir { ^thisProcess.platform.systemAppSupportDir }

	userAppSupportDir {
		_Platform_userAppSupportDir
		^this.primitiveFailed
	}
	*userAppSupportDir { ^thisProcess.platform.userAppSupportDir }

	systemExtensionDir {
		_Platform_systemExtensionDir
		^this.primitiveFailed
	}
	*systemExtensionDir { ^thisProcess.platform.systemExtensionDir }

	userExtensionDir {
		_Platform_userExtensionDir
		^this.primitiveFailed
	}
	*userExtensionDir { ^thisProcess.platform.userExtensionDir }

	userConfigDir {
		_Platform_userConfigDir
		^this.primitiveFailed
	}
	*userConfigDir { ^thisProcess.platform.userConfigDir }

	resourceDir {
		_Platform_resourceDir
		^this.primitiveFailed
	}
	*resourceDir { ^thisProcess.platform.resourceDir }

	*recordingsDir { ^thisProcess.platform.recordingsDir }

	defaultTempDir { ^this.subclassResponsibility() }
	*defaultTempDir { ^thisProcess.platform.defaultTempDir }

	// The "ideName" is for ide-dependent compilation.
	// From SC.app, the value is "scapp" meaning "scide_scapp" folders will be compiled and other "scide_*" ignored.
	ideName {
		_Platform_ideName
		^this.primitiveFailed
	}
	*ideName { ^thisProcess.platform.ideName }

	platformDir { ^this.name.asString }
	*platformDir { ^thisProcess.platform.platformDir }

	pathSeparator { ^this.subclassResponsibility }
	*pathSeparator { ^thisProcess.platform.pathSeparator }

	pathDelimiter{ ^this.subclassResponsibility }
	*pathDelimiter { ^thisProcess.platform.pathDelimiter }

	isPathSeparator { |char| ^this.subclassResponsibility }
	*isPathSeparator { |char| ^thisProcess.platform.isPathSeparator(char) }

	clearMetadata { |path| ^this.subclassResponsibility }
	*clearMetadata { |path| ^thisProcess.platform.clearMetadata(path) }

	// startup/shutdown hooks
	startup { }
	shutdown { }

	startupFiles {
		^[defaultStartupFile];
	}

	*deprecatedStartupFiles {|paths|
		var postWarning = false;
		paths.do {|path|
			if (File.exists(path.standardizePath)) {
				warn("Deprecated startup file found: %\n".format(path));
				postWarning = true;
			}
		};
		if (postWarning) {
			postln("Please use % as startup file.\nDeprecated startup files will be ignored in future versions.\n".format(defaultStartupFile));
		}
	}

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

	// Prefer qt but fall back to swing if qt not installed.
	defaultGUIScheme { if (GUI.get(\qt).notNil) {^\qt} {^\swing} }

	// Predicate to check if SuperCollider was built with Qt.
	*hasQt {
		_Platform_hasQt
		^this.primitiveFailed
	}

	// Predicate to check if SuperCollider was built with QtWebEngine
	*hasQtWebEngine {
		_Platform_hasQtWebEngine
		^this.primitiveFailed
	}

	// Architecture for which this version of SuperCollider was built.
	*architecture {
		_Platform_architecture
		^this.primitiveFailed
	}

	isSleeping { ^false } // unless defined otherwise

	// used on systems to deduce a svn directory path, if system wide location is used for installed version. (tested on Linux).
	devLoc{ |inpath|
		var outpath;
		if ( devpath.isNil ){ ^inpath };
		outpath = inpath.copyToEnd( inpath.find( "SuperCollider") );
		outpath = outpath.replace( "SuperCollider", devpath );
		^outpath;
	}

	// hook for clients to write frontend.css
	writeClientCSS {}

	killProcessByID { |pid|
		^this.subclassResponsibility(\killProcessByID)
	}

	killAll { |cmdLineArgs|
		^this.subclassResponsibility(\killAll)
	}

	// used to format paths correctly for command-line calls
	// On Windows, encloses with quotes; on Unix systems, escapes spaces.
	formatPathForCmdLine { |path|
		^this.subclassResponsibility
	}

}

UnixPlatform : Platform {
	pathSeparator { ^$/ }
    pathDelimiter { ^$: }

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

	killProcessByID { |pid|
		("kill -9 " ++ pid).unixCmd;
	}

	killAll { |cmdLineArgs|
		("killall -9 " ++ cmdLineArgs).unixCmd;
	}

	defaultTempDir {
		// +/+ "" looks funny but ensures trailing slash
		^["/tmp/", this.userAppSupportDir +/+ ""].detect({ |path|
			File.exists(path);
		});
	}

	formatPathForCmdLine { |path|
		^path.escapeChar($ );
	}

}

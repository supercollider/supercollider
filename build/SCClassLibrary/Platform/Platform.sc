Platform
{
	var <classLibraryDir, features;

	*new {
		^super.new.initPlatform
	}
	initPlatform {
		classLibraryDir = thisMethod.filenameSymbol.asString.dirname.dirname;
		features = IdentityDictionary.new;
	}

	name { ^this.subclassResponsibility }
	switch { | ... cases |
		^this.name.switch(*cases)
	}
	*switch { | ... cases |
		^thisProcess.platform.switch(*cases)
	}

	// directories
	systemAppSupportDir { _Platform_systemAppSupportDir }
	userAppSupportDir { _Platform_userAppSupportDir }
	systemExtensionDir { _Platform_systemExtensionDir }
	userExtensionDir { _Platform_userExtensionDir }
	platformDir { ^this.name.asString }
	pathSeparator { ^this.subclassResponsibility }

	// startup/shutdown hooks
	startup { }
	shutdown { }

	startupFiles { ^#[] }
	loadStartupFiles { this.startupFiles.do(_.loadPaths) }

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

	// convenience accessors
	*classLibraryDir { ^thisProcess.platform.classLibraryDir }
	*systemAppSupportDir { ^thisProcess.platform.systemAppSupportDir }
	*userAppSupportDir { ^thisProcess.platform.userAppSupportDir }
	*systemExtensionDir { ^thisProcess.platform.systemExtensionDir }
	*userExtensionDir { ^thisProcess.platform.userExtensionDir }
}

UnixPlatform : Platform
{
	pathSeparator { ^$/ }
}

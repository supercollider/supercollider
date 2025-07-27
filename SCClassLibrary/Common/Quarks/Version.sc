Version : Object {

	var majorVersion = 0;
	var minorVersion = 1;
	var patchVersion = 0;
	var preReleaseVersion = nil;
	var buildVersion = nil;

	*new { |semverString = "0.1.0"|
		^super.new.init(semverString);
	}

	init { |semverString|
		// Convert the given string into an event
		var semverAsEvent
		= Version.prSemverStringToEvent(semverString);
		// Assign version specifications
		// to the instance
		majorVersion = semverAsEvent[\major];
		minorVersion = semverAsEvent[\minor];
		patchVersion = semverAsEvent[\patch];
		preReleaseVersion = semverAsEvent[\preRelease];
		buildVersion = semverAsEvent[\build];
	}

	// Converts a semver string
	// into an event for further manipulations
	// Expects a correct semver string
	*prSemverStringToEvent { |semverString|
		// This is an event and not a list because
		// 4th element could be either
		// a pre-release or a build information
		var semverAsEvent = (
			major: nil,
			minor: nil,
			patch: nil,
			preRelease: nil,
			build: nil
		);
		var semver = semverString;
		var split;

		// Checking if semver string
		// contains build information
		if("[[.+.]]".matchRegexp(semver)) {
			var split = semver.split($+);
			semverAsEvent[\build] = split[1];
			semver = split[0];
		};

		// Checking if semver string
		// contains pre-release information
		// This needs a particular algorithm
		// because several hyphens are tolerated,
		// with only the first one delimiting
		// version numbers from pre-release
		// identifier
		if("[[.-.]]".matchRegexp(semver)) {
			var position = 0;
			var delimiterReached = false;
			var semverString = "";
			var releaseString = "";
			while { position < semver.size } {
				if(delimiterReached.not) {
					if(semver[position] != $-) {
						semverString =
						semverString ++ semver[position];
					} {
						delimiterReached = true;
					};
				} {
					releaseString =
					releaseString ++ semver[position];
				};
				position = position + 1;
			};
			semverAsEvent[\preRelease] = releaseString;
			semver = semverString;
		};

		// Separating Major / Minor / Patch
		split = semver.split($.);
		semverAsEvent[\major] = split[0].asInteger;
		semverAsEvent[\minor] = split[1].asInteger;
		semverAsEvent[\patch] = split[2].asInteger;

		// Return the event
		^semverAsEvent
	}

	// Version specification getters
	major {
		^majorVersion
	}

	minor {
		^minorVersion
	}

	patch {
		^patchVersion
	}

	preRelease {
		^preReleaseVersion
	}

	build {
		^buildVersion
	}

	// Special getters
	*helloWorld {
		^this.new
	}

	*beta {
		^this.new.bumpMajor.preRelease_("beta")
	}

	*alpha {
		^this.new.bumpMajor.preRelease_("alpha")
	}

	*v1 {
		^this.new.bumpMajor
	}

	// Version specification setters
	bumpMajor {
		majorVersion = majorVersion + 1;
		minorVersion = 0;
		patchVersion = 0;
	}

	bumpMinor {
		minorVersion = minorVersion + 1;
		patchVersion = 0;
	}

	bumpPatch {
		patchVersion = patchVersion + 1;
	}

	// Should it remove "-"
	// if it is the first character ?
	preRelease_ { |aString|
		if(aString != "")
		{ preReleaseVersion = aString; }
		{ preReleaseVersion = nil; };
	}

	// Should it remove "+"
	// if it is the first character ?
	build_ { |aString|
		if(aString != "")
		{ buildVersion = aString; }
		{ buildVersion = nil; };
	}

	// Special setter
	clear {
		preReleaseVersion = nil;
		buildVersion = nil;
	}

	// Semver String getter
	string {
		var string = "";
		string = string ++ majorVersion.asString;
		string = string ++ ".";
		string = string ++ minorVersion.asString;
		string = string ++ ".";
		string = string ++ patchVersion.asString;
		// Append pre-release if needed
		if(preReleaseVersion.notNil) {
			string = string ++ "-";
			string = string ++ preReleaseVersion;
		};
		// Append build if needed
		if(buildVersion.notNil) {
			string = string ++ "+";
			string = string ++ buildVersion;
		};
		// Return
		^string
	}

	// Comparisons tools

	// Equality symbol checks equality
	// WITHOUT comparing builds
	== { |aVersion|
		var equality = (majorVersion == aVersion.major);
		if(equality)
		{ equality = (minorVersion == aVersion.minor); };
		if(equality)
		{ equality = (patchVersion == aVersion.patch); };
		if(equality) {
			equality =
			(preReleaseVersion == aVersion.preRelease);
		};
		^equality
	}

	!= { |aVersion|
		^(this == aVersion).not
	}

	// Identity symbol also checks builds equality
	=== { |aVersion|
		^(this.string == aVersion.string);
	}

	!== { |aVersion|
		^(this.string == aVersion.string).not
	}

	isPatchEqual { |aVersion|
		var identity = (majorVersion == aVersion.major);
		if(identity)
		{ identity = (minorVersion == aVersion.minor); };
		if(identity)
		{ identity = (patchVersion == aVersion.patch); };
		^identity
	}

	isMinorEqual { |aVersion|
		var identity = (majorVersion == aVersion.major);
		if(identity)
		{ identity = (minorVersion == aVersion.minor); };
		^identity
	}

	isMajorEqual { |aVersion|
		var identity = (majorVersion == aVersion.major);
		^identity
	}

	// Precedency && Posteriority don't check
	// pre-release content itself,
	// only if it's nil or not
	// "1.0.0" > "1.0.0-beta" equals True
	// "1.0.0-alpha" > "1.0.0-beta" equals False

	< { |aVersion|
		if(majorVersion != aVersion.major) {
			if(majorVersion < aVersion.major)
			{ ^true }
			{ ^false };
		} { // if majors are equal
			if(minorVersion != aVersion.minor) {
				if(minorVersion < aVersion.minor)
				{ ^true }
				{ ^false };
			} { // if minors are also equal
				if(patchVersion != aVersion.patch) {
					if(patchVersion < aVersion.patch)
					{ ^true }
					{ ^false };
				} { // if patches are also equal
					// Only thing we can distinguish for now
					// is either 'equality'
					// or the presence of pre-release information
					// pre-release strings are not comparable
					// without a collective 'hard-coded'
					// decision procedure
					if(this == aVersion)
					{ ^false }
					{ ^(preReleaseVersion.notNil && aVersion.preRelease.isNil) };
				};
			};
		};
	}

	<= { |aVersion|
		if(majorVersion != aVersion.major) {
			if(majorVersion < aVersion.major)
			{ ^true }
			{ ^false };
		} { // if majors are equal
			if(minorVersion != aVersion.minor) {
				if(minorVersion < aVersion.minor)
				{ ^true }
				{ ^false };
			} { // if minors are also equal
				if(patchVersion != aVersion.patch) {
					if(patchVersion < aVersion.patch)
					{ ^true }
					{ ^false };
				} { // if patches are also equal
					if(this == aVersion)
					{ ^true }
					{ if(preReleaseVersion.notNil && aVersion.preRelease.isNil)
						{ ^true }
						// Yield false if both versions have
						// different pre-release informations
						// because we can't compare their precedence
						// though precedence could technically be true
						{ ^false };
					};
				};
			};
		};
	}

	> { |aVersion|
		if(majorVersion != aVersion.major) {
			if(majorVersion > aVersion.major)
			{ ^true }
			{ ^false };
		} { // if majors are equal
			if(minorVersion != aVersion.minor) {
				if(minorVersion > aVersion.minor)
				{ ^true }
				{ ^false };
			} { // if minors are also equal
				if(patchVersion != aVersion.patch) {
					if(patchVersion > aVersion.patch)
					{ ^true }
					{ ^false };
				} { // if patches are also equal
					// Only thing we can distinguish for now
					// is either 'equality'
					// or the presence of pre-release information
					// pre-release strings are not comparable
					// without a collective 'hard-coded'
					// decision procedure
					if(this == aVersion)
					{ ^false }
					{ ^(preReleaseVersion.isNil && aVersion.preRelease.notNil) };
				};
			};
		};
	}

	>= { |aVersion|
		if(majorVersion != aVersion.major) {
			if(majorVersion > aVersion.major)
			{ ^true }
			{ ^false };
		} { // if majors are equal
			if(minorVersion != aVersion.minor) {
				if(minorVersion > aVersion.minor)
				{ ^true }
				{ ^false };
			} { // if minors are also equal
				if(patchVersion != aVersion.patch) {
					if(patchVersion > aVersion.patch)
					{ ^true }
					{ ^false };
				} { // if patches are also equal
					if(this == aVersion)
					{ ^true }
					{ if(preReleaseVersion.isNil && aVersion.preRelease.notNil)
						{ ^true }
						// Yield false if both versions have
						// different pre-release informations
						// because we can't compare their posteriority
						// though posteriority could technically be true
						{ ^false };
					};
				};
			};
		};
	}

}

VersionConstraint : Object {

	var versionRequirements = nil;
	var noPreRelease = false;

	*new { |requirements = nil|
		^super.new.init(requirements);
	}

	init { |requirements|
		this.setRequirements(requirements);
	}

	setRequirements { |requirements = nil|
		if(requirements.notNil) {
			versionRequirements = requirements.split(Char.space);
			if(versionRequirements.includesEqual("noPreRelease")) {
				noPreRelease = true;
				versionRequirements =
				versionRequirements.select({ |item|
					item != "noPreRelease"; });
			};
		} {
			versionRequirements = nil;
		};
	}

	compatibleWith { |aSemverString|
		if(versionRequirements.isNil) {
			^true
		} {
			var version = Version(aSemverString);
			var operators = "=!<>";
			// Check noPreRelease condition
			if(noPreRelease) {
				if(version.preRelease.notNil) {
					^false
				};
			};
			// Iterate through conditions
			versionRequirements.do({ |requirement|
				var operand = "";
				var semverString = "";
				var versionComparator;
				// Parse operand and semver String
				requirement.do({ |char|
					if(operators.includes(char))
					{ operand = operand ++ char; }
					{ semverString = semverString ++ char; };
				});
				versionComparator = Version(semverString);
				switch(operand)
				{ "=" } {
					if(version != versionComparator)
					{ ^false };
				}
				{ "!" } {
					if(version == versionComparator)
					{ ^false };
				}
				{ "===" } {
					if(version !== versionComparator)
					{ ^false };
				}
				{ "!==" } {
					if(version === versionComparator)
					{ ^false };
				}
				{ "<" } {
					if((version < versionComparator).not)
					{ ^false };
				}
				{ "<=" } {
					if((version <= versionComparator).not)
					{ ^false };
				}
				{ ">" } {
					if((version > versionComparator).not)
					{ ^false };
				}
				{ ">=" } {
					if((version >= versionComparator).not)
					{ ^false };
				}
			});
			// If no condition returned false, then true
			^true
		};
	}

}

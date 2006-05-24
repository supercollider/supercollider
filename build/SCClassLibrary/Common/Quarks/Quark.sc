QuarkDependency
{
	var <name, <version;
	*new { | name, version |
		^this.newCopyArgs(name, version)
	}
}

// a Package
Quark
{
	var <name, <summary, <version, <dependencies, <tags,<>path;

	*fromFile { | path |
		var string = { File.use(path, "r", _.readAllString) }.try;
		if (string.isNil) {
			Error("invalid quark file").throw;
		};
		^this.fromString(string)
	}
	*fromString { | string |
		var blob = { string.interpret }.try;
		if (blob.isNil or: { blob.isKindOf(IdentityDictionary).not }) {
			Error("invalid quark").throw;
		};
		^this.new(blob)
	}
	*new { | blob |
		^super.new.init(blob)
	}
	init { | blob |
		name = this.getName(blob[\name]);
		summary = this.getString(blob[\summary]);
 		version = this.getVersion(blob[\version]);
		dependencies = this.getDependencies(blob[\dependencies]);
		tags = ();
	}

	getName { | obj |
		var name = obj.asString;
		if (name.isEmpty) {
			error("invalid name")
		};
		^name.split(Char.nl).first/*.strip*/
	}
	getString { | obj |
		^obj !? { obj.asString }
	}
	getVersion { | obj |
		if (obj.notNil) {
			if (obj.repondsTo(\asFloat).not) {
				Error("invalid version type").throw;
			};
			^obj.asFloat
		};
		^nil
	}
	getDependencies { | obj |
		var name, version;
		if (obj.isNil) {
			^[]
		};
		if (obj.isSequenceableCollection.not) {
			error("invalid deps");
		};
		^obj.collectAs({ | spec |
			if (spec.isString) {
				QuarkDependency(spec, nil)
			} {
				if (spec.isKindOf(Association)) {
					QuarkDependency(
						spec.key.asString,
						this.getVersion(spec.value));
				}
			}
		}, Array)
	}
	printOn { arg stream;
		stream << "Quark: " << name;
		if(version.notNil,{ stream << " [" << version << "]"; });
	}
}


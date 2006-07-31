
/**
  *
  * Subversion based package repository and package manager
  * a work in progress.  sk & cx
  *
  */

// quarks are much worse than opiates, you should have been more careful
QuarkDependency
{
	var <name, <version;
	*new { | name, version |
		^this.newCopyArgs(name, version)
	}
}


// a Quark is a single package of classes, helpfiles etc.
// the QUARK file should contain an IdentityDictionary
// path is a relative path — relative to the repos root or the local root

Quark
{
	var <name, <summary, <version, <dependencies, <tags,<>path;

	*fromFile { | path |
		var string = { File.use(path, "r", _.readAllString) }.try;
		if (string.isNil) {
			Error(path + "is an invalid quark file").throw;
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
		path = this.getString(blob[\path]);
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


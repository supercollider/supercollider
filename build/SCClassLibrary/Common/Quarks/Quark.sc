
/**
  *
  * Subversion based package repository and package manager
  * a work in progress.  sk, cx, LFSaw, danstowell
  *
  */

// quarks are much worse than opiates, you should have been more careful
QuarkDependency
{
	// Note: "repos" should be nil if dependency is in same repos; otherwise the base URL of the repos.
	var <name, <version, <repos;
	*new { | name, version, repos |
		^this.newCopyArgs(name, version, repos)
	}
	== { arg that; 
		^that respondsTo: #[name, version] 
			and: {(that.isKindOf(QuarkDependency))
			and: {this.name  == that.name}
			and: {this.version  == that.version}
		}
	}
}


// a Quark is a single package of classes, helpfiles etc.
// the QUARK file should contain an IdentityDictionary
// path is a relative path  relative to the repos root or the local root

Quark
{
	var <name, <summary, <version, <author, <dependencies, <tags,<>path;
	var <info;
	
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
		author = this.getString(blob[\author]);
		info = blob;
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
			if (obj.respondsTo(\asFloat).not) {
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
				if (spec.isKindOf(ArrayedCollection)) {
						QuarkDependency(
							spec[0].asString,
							this.getVersion(spec[1]),
							spec[2].asString
							);
				} {
					if (spec.isKindOf(Association)) {
						QuarkDependency(
							spec.key.asString,
							this.getVersion(spec.value));
					}
				}
			}
		}, Array)
	}
	printOn { arg stream;
		stream << "Quark: " << name;
		if(version.notNil,{ stream << " [" << version << "]"; });
	}
	
	postDesc {
		var string;
		string = name;
		if(version.notNil,{ string = string + "[" ++ version ++ "]"; });
		string = string 
			++ "\n(by " ++ (author ? "anonymous") ++ ")"
			++ "\n" ++ summary;
		dependencies.notEmpty.if{
			string = string ++ "\nDepends on";
			dependencies.do{|dep|
				string = string ++ "\n - " ++ dep.name;
			};
		};
		string = string ++ "\n";
		string.postln;
	}
	== { arg that; 
		^that respondsTo: #[name, summary, version, author, dependencies, tags, path] 
			and: {(this.name  == that.name)
			and: {this.summary == that.summary}
			and: {this.version  == that.version}
			and: {this.author  == that.author}
			and: {this.dependencies  == that.dependencies}
			and: {this.tags  == that.tags}
			and: {this.path  == that.path}
		}
	}
}

QuarkView {
	var <view, <quark, <isInstalled, <changeState;
	*new{|parent, extent, quark|
		super.new.init(parent, extent, quark)
	}
	init {|parent, extent, aQuark|
		var installBounds, descrBounds, infoBounds, pad = 5;
		
		installBounds = Rect(0,0, extent.y, extent.y);
		infoBounds = Rect(0,0, 60, extent.y);
		descrBounds = Rect(
			0,0, 
			extent.x - (infoBounds.width - infoBounds.width - (2*pad)), extent.y
		);
		
		quark = aQuark;
		
		// the install / remove Button
		GUI.button.new(parent, installBounds).states_([
				["-", Color.black, Color.clear],		// never installed
				["+", Color.black, Color.red],		// selected to install
				["-", Color.black, Color.red(0.5, 0.5)], // selected to deinstall
				["y", Color.black, Color.green],		// already installed
			]);
		// the name with author
		GUI.staticText.new(parent, descrBounds).string_("% by %".format(quark.name, quark.author));
		// the name
		GUI.button.new(parent, infoBounds).states_([["info"]]);
	}
}
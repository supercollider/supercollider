
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
	asQuark { |parentQuarks|
		parentQuarks = parentQuarks ? Quarks.global;
		^if(repos.isNil, {parentQuarks}, {Quarks.forUrl(repos)}).repos.findQuark(name, version);
	}
}


// a Quark is a single package of classes, helpfiles etc.
// the QUARK file should contain an IdentityDictionary
// path is a relative path  relative to the repos root or the local root

Quark
{
	var <name, <summary, <version, <author, dependencies, <tags, <>path; 
	var <parent; // the Quarks, if available 
	var <info;
	
	*fromFile { | path, parent |
		var string = { File.use(path, "r", _.readAllString) }.try;
		if (string.isNil) {
			Error(path + "is an invalid quark file").throw;
		};
		^this.fromString(string, parent)
	}
	*fromString { | string , parent|
		var blob = { string.interpret }.try;
		if (blob.isNil or: { blob.isKindOf(IdentityDictionary).not }) {
			Error("invalid quark").throw;
		};
		^this.new(blob, parent)
	}
	*new { | blob, parent |
		^super.new.init(blob, parent)
	}
	init { | blob , argParent |
		parent = argParent;

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
							spec[2] !? {spec[2].asString}
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
	
	longDesc {
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
		^string;
	}
	postDesc {
		this.longDesc.postln;
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
	dependencies { |recursive = false, knownList|
		var deps, quark, selfasdep;
		deps = dependencies;
		if(recursive, {
			if(knownList.isNil.not, {
				deps = dependencies.select({|d| knownList.indexOfEqual(d).isNil}); // To avoid infinite recursion traps
			});
			deps.do({|dep|
				quark = dep.asQuark(parent);
				deps = deps ++ quark.dependencies(recursive: true, knownList: ([QuarkDependency(name, version)] ++ knownList));
			});
		});
		^deps
	}
}

QuarkView {
	var	<quark, <isInstalled, <toBeInstalled = false, <toBeDeinstalled = false, installButton,
		nameView, infoButton, srcButton;
	*new{|parent, extent, quark, isInstalled|
		^super.new.init(parent, extent, quark, isInstalled)
	}
	init {|parent, extent, aQuark, argIsInstalled|
		var installBounds, descrBounds, infoBounds, sourceBounds, pad = 5;
		
		installBounds = Rect(0,0, extent.y, extent.y);
		infoBounds = Rect(0,0, 40, extent.y);
		sourceBounds = Rect(0, 0, 20, extent.y);
		descrBounds = Rect(
			0,0, 
			extent.x - (infoBounds.width  - sourceBounds.width - (2*pad)),
			extent.y
		);
		quark = aQuark;
		isInstalled = argIsInstalled;
		
		installButton = GUI.button.new(parent, Rect(15,15,17,17));
		this.updateButtonStates;

		// the name with author
		nameView = GUI.staticText.new(parent, descrBounds).string_("% by %".format(quark.name, quark.author));
		// the name
		infoButton = GUI.button.new(parent, infoBounds).states_([["info"]]).action_{this.fullDescription};
		
		if(thisProcess.platformClass == OSXPlatform) {
			srcButton = GUI.button.new(parent, sourceBounds).states_([["src"]]).action_{
				"open %/%".format(Quarks.local.path, quark.path).unixCmd;
			};
		};


	}
	updateButtonStates {
		isInstalled.if({
			// Quark is currently installed
			installButton.states = [
				["+", Color.black, Color.green],		// installed
				["x", Color.black, Color.red],		// selected to deinstall
			];
			installButton.action = { arg butt;
				toBeDeinstalled = butt.value>0;
			};
		
		},{
			// Quark is currently not installed
			installButton.states = [
				["-", Color.black, Color.clear],		// never installed
				["*", Color.black, Color.blue(alpha: 0.5)],		// selected to install
			];
			installButton.action = { arg butt;
				toBeInstalled = butt.value>0;
			};
		});
		this.reset;
	}
	reset {
		installButton.valueAction = 0;
	}
	flush {
		toBeInstalled.if  {isInstalled = true;  toBeInstalled   = false};
		toBeDeinstalled.if{isInstalled = false; toBeDeinstalled = false};
		this.updateButtonStates;
	}
	fullDescription {
		var window;
		window = GUI.window.new(quark.name, Rect(100, 100, 400, 200)).front;
		window.view.decorator = FlowLayout(window.view.bounds);
//		GUI.staticText.new(window, window.view.bounds).string_(quark.longDesc);
		GUI.textView.new( window, window.view.bounds.insetBy( 4, 4 ))
			.font_( GUI.font.new( GUI.font.defaultSansFace, 12 ))
			.background_( Color.grey( 0.9 ))	// Color.clear background doesn't work
			.resize_( 5 )
			.autohidesScrollers_( true )
			.hasVerticalScroller_( true )
			.string_( quark.longDesc )
			.editable_( false );
	}
	remove {
		[installButton, nameView, infoButton, srcButton].do(_.remove);
	}
}
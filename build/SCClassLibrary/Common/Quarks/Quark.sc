
/**
  *
  * Subversion based package repository and package manager
  * sk, cx, LFSaw, danstowell
  *
  */


// quarks are much worse than opiates, you should have been more careful !
// now you have a quark dependency
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
// the QUARK file is sc code and should be an IdentityDictionary
// path is a relative path  relative to the repos root or the local root

Quark
{
	var <name, <summary, <version, <author, dependencies, <tags, <>path;
	var <isLocal;// meaning that a local copy exists either because checked out or is local only
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
	*find { |name|
		^Quarks.local.quarks.detect({ |q| q.name == name })
	}
	init { | blob , argParent |
		parent = argParent;

		name = this.getName(blob[\name]);
		path = this.getString(blob[\path]);
		summary = this.getString(blob[\summary]);
 		version = this.getVersion(blob[\version]);
		dependencies = this.getDependencies(blob[\dependencies]);
		author = this.getString(blob[\author]);
		isLocal = (Quarks.local.simplePath ++ "/" ++ path).pathMatch.notEmpty;
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
			++ "\n" ++ summary
			++ "\n" ++ "Checked out: " ++ if(isLocal, "yes", "no");
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
		^this.compareObject(that,[\name, \summary, \version, \author, \dependencies, \tags, \path]);
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
				if(quark.notNil) {
					deps = deps ++ quark.dependencies(recursive: true,
						knownList: ([QuarkDependency(name, version)] ++ knownList));
				};
			});
		});
		^deps
	}
	isCompatible { ^info['isCompatible'].value !== false }
}

QuarkView {
	var	<quark, <isInstalled, <toBeInstalled = false, <toBeDeinstalled = false, installButton,
		nameView, authorView, infoButton, srcButton, browseHelpButton;
	*new { |parent, extent, quark, isInstalled|
		^super.new.init(parent, extent, quark, isInstalled)
	}
	init { |parent, extent, aQuark, argIsInstalled|
		var installBounds, descrBounds, authorBounds, infoBounds, sourceBounds,
			pad = 5,checkoutBounds, remainder;

		//installBounds = Rect(0,0, extent.y, extent.y);
		infoBounds = Rect(0,0, 25, extent.y);
		sourceBounds = Rect(0, 0, 20, extent.y);
		checkoutBounds = Rect(0,0,50,extent.y);
		remainder = extent.x - (infoBounds.width*2)  - sourceBounds.width -
			checkoutBounds.width - (4*pad);
		descrBounds = Rect(0, 0, (remainder * 0.60).asInteger, extent.y);
		authorBounds = Rect(0, 0, (remainder * 0.40).asInteger, extent.y);
		quark = aQuark;
		isInstalled = argIsInstalled;

		installButton = GUI.button.new(parent, Rect(15,15,17,17));
		this.updateButtonStates;

		// the name with author
		nameView = GUI.staticText.new(parent, descrBounds).string_(quark.name);
		authorView = GUI.staticText.new(parent, authorBounds).string_(quark.author);
		infoButton = GUI.button.new(parent, infoBounds)
			.font_( GUI.font.new( GUI.font.defaultSansFace, 10 ))
			.states_([["info"]]).action_{this.fullDescription};

		browseHelpButton = GUI.button.new(parent, infoBounds)
			.font_( GUI.font.new( GUI.font.defaultSansFace, 10 ))
			.states_([["help"]])
			.action_({ Help(quark.parent.local.path +/+ quark.path).gui });

		if(quark.isLocal and: {thisProcess.platform.name == \osx}) {
			srcButton = GUI.button.new(parent, sourceBounds)
				.font_( GUI.font.new( GUI.font.defaultSansFace, 10 ))
				.states_([["src"]]).action_{
					("open " ++ ("%/%".format(Quarks.local.path, quark.path).escapeChar($ ))).unixCmd;
				};
		};
		/*if(quark.isLocal.not,{
			GUI.button.new(parent, checkoutBounds)
				.font_( GUI.font.new( GUI.font.defaultSansFace, 10 ))
				.states_([["checkout"]]).action_{
					Quarks.checkout(quark.name);
				};
		});*/
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
				["*", Color.black, Color.blue(alpha: 0.5)],				// selected to install
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
		var helpdoc = quark.info.helpdoc;
		if(helpdoc.notNil) {
				// get full path
			helpdoc = "%/%/%".format(Quarks.local.simplePath, quark.path, helpdoc);
			if(File.exists(helpdoc).not) { helpdoc = nil };
		};
		window = GUI.window.new(quark.name, Rect(100, 100, 400, 200)).front;
		GUI.textView.new( window, Rect(4, 4, 392, 170 + (helpdoc.isNil.binaryValue * 22)))
			.font_( GUI.font.new( GUI.font.defaultSansFace, 12 ) )
			.background_( Color.grey( 0.9 ) )
			.resize_( 5 )
			.autohidesScrollers_( true )
			.hasVerticalScroller_( true )
			.string_( quark.longDesc )
			.editable_( false );
		if(helpdoc.notNil) {
			GUI.button.new(window, Rect(125, 176, 150, 20))
				.resize_(8)
				.states_([["Open quark help"]])
				.action_({
					helpdoc.openHTMLFile
				});
		};
	}
	remove {
		[installButton, nameView, infoButton, srcButton].do(_.remove);
	}
}

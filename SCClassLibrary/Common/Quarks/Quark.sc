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
		^this.compareObject(that, [\name, \version])
	}
	hash { arg that;
		^this.instVarHash([\name, \version])
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
	var <name, <summary, <version, <author, dependencies, <tags, <>path, <>status;
	var <isLocal;// meaning that a local copy exists either because checked out or is local only
	var <parent; // the Quarks, if available
	var <info;

	*fromFile { | path, parent |
		var string = { File.use(path, "r", _.readAllString) }.try;
		if (string.isNil) {
			Error(path + "is an invalid quark file.").throw;
		};
		^this.fromString(string, parent)
	}
	*fromString { | string , parent|
		var blob = { string.interpret }.try;
		if (blob.isNil or: { blob.isKindOf(IdentityDictionary).not }) {
			Error("Can not interpret quark definition.").throw;
		};
		^this.new(blob, parent)
	}
	*fromPath { |path, parent| // if no blob exists, make a generic one
		var blob = (
			name: path.basename.splitext,
			path: path
		);
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
		status = this.getString(blob[\status]);
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
			Error("Can not parse a quark: invalid name.").throw;
		};
		^name.split(Char.nl).first/*.strip*/
	}
	getString { | obj |
		^obj !? { obj.asString }
	}
	getVersion { | obj |
		if (obj.notNil) {
			if (obj.respondsTo(\asFloat).not) {
				Error("Can not parse quark '" ++ this.name ++ "': invalid version type.").throw;
			};
			^obj.asFloat
		};
		^nil
	}
	getDependencies { | obj |
		var deps;
		if (obj.isNil) {
			^[]
		};
		if (obj.isSequenceableCollection.not) {
			Error("Can not parse quark '" ++ this.name ++ "': invalid dependencies.").throw;
		};
		deps = Array(obj.size);
		obj.do { | spec |
			var dep;
			case
			{spec.isString} {
				dep = QuarkDependency(spec, nil)
			}
			{spec.isKindOf(ArrayedCollection)} {
				dep = QuarkDependency(
					spec[0].asString,
					this.getVersion(spec[1]),
					spec[2] !? {spec[2].asString})
			}
			{spec.isKindOf(Association)} {
				dep = QuarkDependency(
					spec.key.asString,
					this.getVersion(spec.value))
			}
			{Error("Can not parse quark '" ++ this.name ++ "': an invalid dependency.").throw};
			deps add: dep
		};
		^deps;
	}
	help {
		var p = info.schelp;
		if(p.notNil) {
			HelpBrowser.openHelpFor(p);
			^this
		};
		if(File.exists(parent.local.path +/+ path +/+ "HelpSource")) {
			HelpBrowser.openBrowsePage("Quarks>"++name);
			^this;
		};
		p = info.helpdoc;
		if(p.notNil) {
			HelpBrowser.goTo(URI.fromLocalPath(parent.local.path +/+ path +/+ p).asString)
			^this
		};
		HelpBrowser.openBrowsePage("Quarks>"++name);
	}
	printOn { arg stream;
		stream << "Quark: " << name;
		if(version.notNil,{ stream << " [" << version << "]"; });
	}

	longDesc {
		var string;
		string = name;
		if(version.notNil) { string = string + "[" ++ version ++ "]" };
		string = string
			++ "\n(by " ++ (author ? "anonymous") ++ ")"
			++ "\n\n" ++ summary
			++ "\n\n" ++ "Status: " ++ (status ? "unknown")
			++ "\n" ++ "Checked out: " ++ if(isLocal, "yes", "no");
		if(dependencies.notEmpty) {
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
		^this.compareObject(that,
			[\name, \summary, \version, \author, \dependencies, \tags, \path, \status]);
	}
	hash { arg that;
		^this.instVarHash([\name, \summary, \version, \author, \dependencies, \tags, \path, \status]);
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

		quark = aQuark;
		isInstalled = argIsInstalled;

		//installBounds = Rect(0,0, extent.y, extent.y);
		infoBounds = Rect(0,0, 25, extent.y);
		sourceBounds = Rect(0, 0, 20, extent.y);
		checkoutBounds = Rect(0,0,50,extent.y);
		remainder = extent.x - (infoBounds.width*2)  - sourceBounds.width -
			checkoutBounds.width - (4*pad);
		descrBounds = Rect(0, 0, (remainder * 0.60).asInteger, extent.y);
		authorBounds = Rect(0, 0, (remainder * 0.40).asInteger, extent.y);

		installButton = GUI.button.new(parent, Rect(15,15,17,17));

		// the name with author
		nameView = GUI.staticText.new(parent, descrBounds).string_(quark.name);
		authorView = GUI.staticText.new(parent, authorBounds).string_(quark.author);
		infoButton = GUI.button.new(parent, infoBounds)
			.font_( Font.sansSerif( 10 ))
			.states_([["info"]]).action_{this.fullDescription};

		browseHelpButton = GUI.button.new(parent, infoBounds)
			.font_( Font.sansSerif( 10 ))
			.states_([["help"]])
			.action_({ quark.help });

		if(quark.isLocal) {
			srcButton = GUI.button.new(parent, sourceBounds)
				.font_( Font.sansSerif( 10 ))
				.states_([["src"]]).action_{
					openOS( "%/%".format(Quarks.local.path, quark.path) );
				};
		};

		/*if(quark.isLocal.not,{
			GUI.button.new(parent, checkoutBounds)
				.font_( Font.sansSerif( 10 ))
				.states_([["checkout"]]).action_{
					Quarks.checkout(quark.name);
				};
		});*/

		this.updateButtonStates;
	}
	updateButtonStates {
		var palette = GUI.current.tryPerform(\palette);
		var c = palette !? {palette.button};

		isInstalled.if({
			// Quark is currently installed
			installButton.states = [
				// installed
				["+", nil, if(c.notNil){Color.green.blend(c,0.5)}{Color.green(1, 0.5)}],
				// selected to deinstall
				["x", nil, if(c.notNil){Color.red.blend(c,0.5)}{Color.red(1, 0.5)}]
			];
			installButton.action = { arg butt;
				toBeDeinstalled = butt.value>0;
			};

		},{
			// Quark is currently not installed
			installButton.states = [
				// never installed
				["-", nil, ],
				// selected to install
				["*", nil, if(c.notNil){Color.blue.blend(c,0.5)}{Color.blue(1, 0.5)}]
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
		GUI.textView.new( window, Rect(4, 4, 392, 192))
			.font_( Font.sansSerif( 12 ) )
			.resize_( 5 )
			.autohidesScrollers_( true )
			.hasVerticalScroller_( true )
			.string_( quark.longDesc )
			.editable_( false );
		GUI.button.new(window, Rect(125, 176, 150, 20))
			.resize_(8)
			.states_([["Open quark help"]])
			.action_({ quark.help });
	}
	remove {
		[installButton, nameView, infoButton, srcButton].do(_.remove);
	}
}

QuarkViewQt {
	var <quark, <isInstalled, <toBeInstalled = false, <toBeDeinstalled = false, <treeItem;
	var installButton;

	*new { |parent, quark, isInstalled|
		^super.new.init(parent, quark, isInstalled)
	}

	init { |parent, aQuark, argIsInstalled|

		quark = aQuark;
		isInstalled = argIsInstalled;

		installButton = Button().fixedSize_(Size(20,20));
		treeItem = parent.addItem([
			nil, quark.name,
			quark.summary !? { quark.summary.replace("\n"," ").replace("\t","") }
		]).setView( 0, installButton );

		this.updateButtonStates;
	}

	updateButtonStates {
		var palette = GUI.current.tryPerform(\palette);
		var c = palette !? {palette.button};

		isInstalled.if({
			// Quark is currently installed
			installButton.states = [
				// installed
				["+", nil, if(c.notNil){Color.green.blend(c,0.5)}{Color.green(1, 0.5)}],
				// selected to deinstall
				["x", nil, if(c.notNil){Color.red.blend(c,0.5)}{Color.red(1, 0.5)}]
			];
			installButton.action = { arg butt;
				toBeDeinstalled = butt.value>0;
			};

		},{
			// Quark is currently not installed
			installButton.states = [
				// never installed
				["-", nil, ],
				// selected to install
				["*", nil, if(c.notNil){Color.blue.blend(c,0.5)}{Color.blue(1, 0.5)}]
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
}

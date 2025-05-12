// All methods from PathName moved to String class,
// so PathName methods only redirect here.

+ String {

	// new final conversion:
	asPath { ^this.standardizePath }

	// backwards compat to PathName - needed just in case?
	fullPath { ^this.asPath }

	lastSeparatorIndex {
		this.reverseDo { |char, revIndex|
			if (Platform.isPathSeparator(char)) {
				^this.size - revIndex - 1
			};
		};
		^-1
	}

	separatorIndices {
		var res = List[];
		this.do { |ch, i| if (ch.isPathSeparator) { res = res.add(i) } };
		^res
	}

	// ok to leave this here is String?
	fileName { ^this.basename }
	fileNameWithoutExtension { ^this.basename.splitext[0] }
	fileNameWithoutDoubleExtension {
		^this.basename.splitext[0].splitext[0]
	}
	folderName { ^this.dirname.basename }

	extension {
		^this.splitext.last ? ""
	}

	isAbsolutePath { ^this.at(0).isPathSeparator }

	nextName {
		var name, ext;
		if (this.last.isDecDigit) {
			^this.noEndNumbers ++ (this.endNumber + 1)
		};
		#name, ext = this.splitext;
		if (ext.isNil) {
			^name ++ "1"
		};
		^name.nextName ++ "." ++ ext
	}

	noEndNumbers {
		^this[..this.endNumberIndex]
	}

	endNumber {	// turn consecutive digits at the end of this into a number.
		^this[this.endNumberIndex + 1..].asInteger
	}

	endNumberIndex {
		var index = this.lastIndex;
		while({
			index > 0 and: { this.at(index).isDecDigit }
		}, {
			index = index - 1
		});
		^index
	}

	entries { ^pathMatch(this +/+ "*") }
	isFolder { ^this.last.isPathSeparator }

	isFile {
		var path = this.pathMatch;
		^if(path.notEmpty, {
			path.at(0).last.isPathSeparator.not
		}, { false })
	}

	files {
		^this.entries.select({ | item | item.isFile })
	}

	folders {
		^this.entries.select({ | item | item.isFolder })
	}

	deepFiles {
		^this.entries.collect({ | item |
			if(item.isFile, {
				item
			},{
				item.deepFiles
			})
		}).flatIf { |item| item.isKindOf(String).not }
	}

	parentPath {
		var ci = this.separatorIndices;

		^if((this.last.isPathSeparator) and: { ci.size > 1 }, {
			this.copyRange(0, ci[ci.size - 2])
		}, {
			this.copyRange(0, this.lastSeparatorIndex)
		})
	}

	filesDo { | func |
		this.files.do(func);
		this.folders.do { | pathname |
			pathname.filesDo(func)
		};
	}

	streamTree { | str, tabs = 0 |
		str << this << Char.nl;
		this.files.do({ | item |
			tabs.do({ str << Char.tab });
			str << item.fileNameWithoutExtension  << Char.nl
		});
		this.folders.do({ | item |
			item.streamTree(str, tabs + 1);
		});
	}

	dumpTree {
		this.streamTree(Post)
	}

	dumpToDoc { | title="Untitled" |
		var str, doc;
		doc = Document.new(title);
		str = CollStream.new;
		this.streamTree(str);
		doc.string = str.collection;
		^doc
	}
}

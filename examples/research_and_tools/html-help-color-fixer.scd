// html help color correction -- James Harkins

/*
Purpose:
OSX applies a color correction profile to HTML files as they are opened in SC. Then, when the document is resaved to disk, the colors are slightly altered. Over time, they fade. This utility scans the HTML and replaces colors with the standard OSX syntax-colorize scheme.

Usage:
This code loads all the functions into an environment saved in the global library. In OSX, a menu item is added to fix the colors in the current open document -- just make sure the document is the frontmost window and go to the Library menu > HTML Color Fix to process that document. It must be an HTML document.

To use the commandline utility, you need to enter the environment first.

1. Push the environment.
2. Set parameters using the environment variable names at the top of the code block.
3. Execute "~go.value" to scan all .html files in the root directory and subdirectories (recursively).
4. Pop the environment.

e.g.,

Library.at(\colorFix).push;

~root = "... path to source file directory...";

	// outRoot is for non-destructive writing (the default)
	// should be different from root
	// if using destructive writing, outRoot is ignored
~outRoot = "... path to output file directory...";

	// either \writeFileNonDestructive or \writeFileDestructive
~operation = \writeFileNonDestructive;

	// if using destructive writing, this could be false
	// but it should be true for non-destructive
	// otherwise your target directory will be missing unchanged files
~alwaysWrite = true;

~go.value;		// run it!

Environment.pop;
*/


(
Library.put(\colorFix, Environment.make {

// these are the real colors, from SuperCollider.app syntax colorize
~red = "bf0000";
~green = "007300";
~blue = "0000bf";
~gray = "606060";
~signatures = [~red, ~green, ~blue];

~reportErrors = false;
~haltOnError = false;
~alwaysWrite = true;	// recommended for non-destructive
~operation = \writeFileNonDestructive;
~root = "";
~outRoot = "";

~go = {
	if(~root.size > 0) {
		~scanDirectoryRecursive.(~root, ~root, ~outRoot, ~operation)
	} {
		"Set ~root and ~outRoot before running.".postln;
	};
};

~scanDirectoryRecursive = { |path, root, outRoot, op|
	var	dirs = (path ++ "/*/").pathMatch, success = true;
	dirs.reject({ |dir| dir.basename[0] == $. }).do({ |dir|
		if(~scanDirectoryRecursive.value(dir[ .. dir.size-2], root, outRoot, op).not) { success = false };
	});
	if(~scanDirectory.value(path, root, outRoot, op).not) { success = false };
	success
};

~scanDirectory = { |path, root, outRoot, op|
	var	files = (path ++ "/*.html").pathMatch, success = true;
	files.reject({ |file| file.basename[0] == $. }).do({ |filepath|
		filepath.debug("processing");
		if(~scanFile.value(filepath, root, outRoot, op).not) { success = false };
	});
	success
};

~scanFile = { |path, root, outRoot, op|
	var file = File(path, "r"), outFile, contents, stream, success = true,
		outPath, outDir;
	var	numFixes;
	if(file.isOpen) {
		protect {
			contents = file.readAllString;
		} { file.close };
		stream = CollStream(contents);
		numFixes = Ref(0);	// pass by reference
		if(~scanStream.(stream, numFixes)) {
				// write the file only if anything changed
			if(~alwaysWrite or: { numFixes.value > 0 }) {
				op.envirGet.value(stream, path, root, outRoot);
			};
		} {
			"Error occurred while scanning %. Output file was not written.".format(path).warn;
			success = false;
		};
	} {
		"Could not open file %.".format(path).warn;
		success = false;
	};
	success
};

~writeFileNonDestructive = { |stream, path, root, outRoot|
	~writeFile.(stream, outRoot +/+ path[root.size ..], true);
};

~writeFileDestructive = { |stream, path|
	~writeFile.(stream, path, false);
};

~writeFile = { |stream, outPath, checkDir = false|
	var	outDir, outFile, success = true;
	if(checkDir) {
		outDir = outPath.dirname;
		if(File.exists(outDir).not) {
				// if it fails, you'll get "Could not open output file" warnings
				// need to use the blocking systemCmd method
			"mkdir %".format(outDir.escapeChar($ )).systemCmd;
		};
	};
	if((outFile = File(outPath, "w")).isOpen) {
		protect {
			outFile.putAll(stream.contents)
		} { outFile.close };
	} {
		"Could not open output file %.".format(outPath).warn;
		success = false;
	};
	success
};

~scanStream = { |stream, numFixes|
	var	keepGoing = true;

	try {
		while { keepGoing and: { ~scanToColorKey.(stream) } } {
			keepGoing = ~fixColor.(stream, numFixes);
		};
		true	// == success
	} { |error|
		if(~haltOnError) { error.throw };
		if(~reportErrors) { error.reportError };
		false	// == fail
	};
};

// leaves stream at the position of the first hex digit of the 24-bit color
~scanToColorKey = { |stream|
	var	ch, lastCh, temp, stillSearching = true, notEOF = true;
	~fseek.(stream, -1, 1);
	lastCh = stream.getChar;
	while {
		if(stillSearching) {
			lastCh = ch;
			(ch = stream.getChar).notNil
		} { false }
	} {
		if(lastCh != $- and: { ch == $c or: { ch == $C } }) {
			temp = try { stream.nextN(5) }
					// on error, need to stop searching AND return some string (anything)
				{ stillSearching = notEOF = false; "fail" };
			lastCh = temp.last;
			if(temp.collect(_.toLower) == "olor:") {
					// skip spaces after :
				while { lastCh = ch; (ch = stream.getChar).isSpace };
					// is this a #? if yes, we're done
				if(ch == $#) {
					stillSearching = false;
				} {
						// otherwise, back up one and resume from the non-space char
					~fseek.(stream, -2, 1);
					lastCh = stream.getChar;
				};
			} {
					// olor: not matched, go back to the "o"
				~fseek.(stream, -6, 1);
				lastCh = stream.getChar;
			};
		};	// else: keep scanning with next char
	};
	notEOF and: { ch.notNil }	// anything left?
};

// assumes stream is at the position of a 6-digit color
~fixColor = { |stream, numFixes|
	var	colorStr, color, signature;
	colorStr = try { stream.nextN(6) } { "" };
	if(colorStr.size == 6) {
			// if any of the 6 chars is not a digit, an error gets thrown here
			//  and caught by ~scanStream
		color = colorStr.collectAs(_.digit, Array);
			// they all must be hex digits
		if(color.every(_ < 16)) {
			color = color.clump(2)
				.collect({ |row| (row[0] << 4) | row[1] });
			signature = color.maxIndex;
			if(colorStr != ~signatures[signature] and:
					{ signature != 0 or: { color.differentiate[1..].abs.sum != 0 } }) {
				~fseek.(stream, -6, 1);
				~signatures[signature].do({ |ch| stream.put(ch) });
				numFixes.value = numFixes.value + 1;
			};
		};
			// we must return true even if non-hex digits found
			// because in this func, 'false' means we hit the end of the file
		true
	} { false };	// if the color string was too short, we're at EOF
};


// ghastly workaround because interface of IOStream is not good enough
~fseek = { |stream, offset = 0, origin = 1|
	if(stream.isKindOf(File)) {
		stream.seek(offset, origin)
	} {
		switch(origin)
			{ 0 } { stream.pos = offset }
			{ 1 } { stream.pos = stream.pos + offset }
			{ 2 } { stream.pos = stream.collection.size + offset }
			{ Error("Seek on CollStream failed, origin % not valid.".format(origin)).throw }
	};
	stream
};


// menu item for OSX
~cocoaMenu = 'CocoaMenuItem'.asClass;
if(~cocoaMenu.notNil) {
	~checkSaved = {
		var	w, continue, sb = Window.screenBounds;
		if(Document.current.path.splitext.last.collect(_.toLower)[0..2] == "htm") {
			if(Document.current.isEdited) {
				~confirmWrite.(
					"This document is not yet saved.\nPlease save first, then confirm action.\nYES will overwrite the existing disk file!",
					{	if(Document.current.isEdited) {
							"Naughty! You clicked YES without saving! Color fix not done.".postln;
							false
						} { true }
					}
				);
			} {
				~confirmWrite.("Confirm whether or not to execute the fix.\nYES will overwrite the existing disk file!", true);
			};
		} {
			"Not an HTML file. Cannot proceed.".postln;
		};
	};

	~confirmWrite = { |prompt, checkFunc = true|
		var	w, c,
			bigFont = Font(GUI.skin.fontSpecs[0], 28);
		var	docPath;
		var	continue, sb = Window.screenBounds;

		w = Window("Confirm HTML color fix",
			Rect.aboutPoint(sb.extent * 0.5, 200, 150));
		c = CompositeView(w, w.view.bounds.insetBy(25, 25));
		c.decorator = FlowLayout(c.bounds);
		prompt.split($\n).do { |line|
			StaticText(c, (c.bounds.width - 5)@17)
				.align_(\center)
				.string_(line);
		};
		StaticText(c, 50@25);
		c.decorator.nextLine;
		Button(c, ((c.bounds.width - 50) * 0.5)@50)
			.font_(bigFont)
			.states_([["YES"]])
			.action_(inEnvir {
				if(checkFunc.value) {
					w.close;
					fork({
						var	saveAlwaysWrite = ~alwaysWrite;
						docPath = Document.current.path;
						Document.current.close;
						0.01.wait;
							// writing in place, don't need to write if no changes
						~alwaysWrite = false;
						~scanFile.(docPath, "", "", \writeFileDestructive);
						~alwaysWrite = saveAlwaysWrite;
						0.1.wait;
						Document.open(docPath);
					}, AppClock);
				} { w.close }; // checkFunc should have posted a message
			});
		StaticText(c, 30@50);	// spacer
		Button(c, ((c.bounds.width - 50) * 0.5)@50)
			.font_(bigFont)
			.states_([["NO"]])
			.action_({ w.close; "Color fix canceled.".postln });
		w.front;
	};

	~cocoaMenu.add(["HTML Color Fix"], {
		Library.at(\colorFix).use {
			~checkSaved.()
		};
	})
};

});
"HTML color fixer loaded into Library.at(\\colorFix).".postln; ""
)

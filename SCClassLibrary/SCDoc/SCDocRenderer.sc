/*
 ________________________________________
/ HEY! If you make changes to SCDoc,     \
| remember to increment SCDoc.version in |
| SCDoc.sc so you force the docs to      |
\ regenerate.                            /
 ----------------------------------------
    \                                  ___-------___
     \                             _-~~             ~~-_
      \                         _-~                    /~-_
             /^\__/^\         /~  \                   /    \
           /|  O|| O|        /      \_______________/        \
          | |___||__|      /       /                \          \
          |          \    /      /                    \          \
          |   (_______) /______/                        \_________ \
          |         / /         \                      /            \
           \         \^\\         \                  /               \     /
             \         ||           \______________/      _-_       //\__//
               \       ||------_-~~-_ ------------- \ --/~   ~\    || __/
                 ~-----||====/~     |==================|       |/~~~~~
                  (_(__/  ./     /                    \_\      \.
                         (_(___/                         \_____)_)
*/
SCDocHTMLRenderer {
	classvar <binaryOperatorCharacters = "!@%&*-+=|<>?/";
	classvar currentClass, currentImplClass, currentMethod, currArg;
	classvar currentNArgs;
	classvar footNotes;
	classvar noParBreak;
	classvar currDoc;
	classvar minArgs;
	classvar baseDir;

	*escapeSpecialChars {|str|
		var x = "";
		var beg = -1, end = 0;
		str.do {|chr, i|
			switch(chr,
				$&, { x = x ++ str.copyRange(beg, i-1) ++ "&amp;"; beg = i+1; },
				$<, { x = x ++ str.copyRange(beg, i-1) ++ "&lt;"; beg = i+1; },
				$>, { x = x ++ str.copyRange(beg, i-1) ++ "&gt;"; beg = i+1; },
				{ end = i }
			);
		};
		if(beg<=end) {
			x = x ++ str[beg..end];
		};
		^x;
	}
	*escapeSpacesInAnchor { |str|
		^str.replace(" ", "%20")
	}

	// Find the target (what goes after href=) for a link that stays inside the hlp system
	*prLinkTargetForInternalLink { |linkBase, linkAnchor, originalLink|
		var doc, result;

		if(linkBase.isEmpty) {
			result = "";
		} {
			doc = SCDoc.documents[linkBase];
			result = baseDir +/+ linkBase;

			// If this is an existing document, just add .html to get the target
			if(doc.notNil) {
				result = result ++ ".html"
			} {
				// If the document doesn't exist according to SCDoc, check the filesystem
				// to see if the link target is present
				if(File.exists(SCDoc.helpTargetDir +/+ linkBase ++ ".html")) {
					result = result ++ ".html"
				} {
					// If the link target doesn't exist as an HTML file, check to see if the
					// raw filepath exists. If it does, do nothing with it -- we're done. If
					// it doesn't, then consider this a broken link.
					if(File.exists(SCDoc.helpTargetDir +/+ linkBase).not) {
						"SCDoc: In %\n"
						"  Broken link: '%'"
						.format(currDoc.fullPath, originalLink).warn;
					};
				};
			};
		};

		if(linkAnchor.isEmpty) {
			^result
		} {
			^result ++ "#" ++ this.escapeSpacesInAnchor(linkAnchor);
		}
	}

	// Creates a link target for a link that points outside of the help system
	*prLinkTargetForExternalLink { |linkBase, linkAnchor|
		if(linkAnchor.isEmpty) {
			^linkBase
		} {
			^linkBase ++ "#" ++ this.escapeSpacesInAnchor(linkAnchor);
		}
	}

	// Find the text label for the given link, which points inside the help system.
	*prLinkTextForInternalLink { |linkBase, linkAnchor, linkText|
		var doc, result;
		// Immediately return link text if available
		if(linkText.isEmpty.not) {
			^linkText
		};

		// If the base was non-empty, generate it by combining the filename and the anchor.
		// Otherwise, if there was an anchor, use that. Otherwise, use "(empty link)"
		if(linkBase.isEmpty) {
			if(linkAnchor.isEmpty) {
				^"(empty link)"
			} {
				^linkAnchor
			}
		} {
			doc = SCDoc.documents[linkBase];
			result = doc !? _.title ? linkBase.basename;
			if(linkAnchor.isEmpty) {
				^result
			} {
				^result ++ ": " ++ linkAnchor
			}
		}
	}

	// argument link: the raw link text from the schelp document
	// argument escape: whether or not to escape special characters in the link text itself
	// returns: the <a> tag HTML representation of the original `link`
	// Possible, non-exhaustive input types for `link`:
	//   "#-decorator#decorator"
	//   "#-addAction"
	//   "Classes/View#-front#shown"
	//   "Guides/GUI-Introduction#view"
	//   "Classes/FlowLayout"
	//   "#*currentDrag#drag&drop data"
	//   "#Key actions"
	//   "http://qt-project.org/doc/qt-4.8/qt.html#Key-enum"
	*htmlForLink { |link, escape = true|
		var linkBase, linkAnchor, linkText, linkTarget;
		// FIXME: how slow is this? can we optimize

		// Get the link base, anchor, and text from the original string
		// Replace them with empty strings if any are nil
		#linkBase, linkAnchor, linkText = link.split($#);
		linkBase = linkBase ? "";
		linkAnchor = linkAnchor ? "";
		linkText = linkText ? "";

		// Check whether the link is a URL or a relative path (starts with a `/`),
		// NOTE: the second condition is not triggered by anything in the core library's
		// help system. I am not sure if it is safe to remove. - Brian H
		if("^[a-zA-Z]+://.+".matchRegexp(link) or: (link.first == $/)) {
			// Process a link that goes to a URL outside the help system

			// If there was no link text, set it to be the same as the original link
			linkText = if(linkText.isEmpty) { link } { linkText };
			linkTarget = this.prLinkTargetForExternalLink(linkBase, linkAnchor);
		} {
		    // Process a link that goes to a URL within the help system
			linkText = this.prLinkTextForInternalLink(linkBase, linkAnchor, linkText);
			linkTarget = this.prLinkTargetForInternalLink(linkBase, linkAnchor, link);
		};

		// Escape special characters in the link text if requested
		if(escape) { linkText = this.escapeSpecialChars(linkText) };

		// Return a well-formatted <a> tag using the target and link text
		^"<a href=\"" ++ linkTarget ++ "\">" ++ linkText ++ "</a>";
	}

	*makeArgString {|m, par=true|
		var res = "";
		var value;
		var l = m.argNames;
		var last = l.size-1;
		l.do {|a,i|
			if (i>0) { //skip 'this' (first arg)
				if(i==last and: {m.varArgs}) {
					res = res ++ " <span class='argstr'>" ++ "... " ++ a;
				} {
					if (i>1) { res = res ++ ", " };
					res = res ++ "<span class='argstr'>" ++ a;
					(value = m.prototypeFrame[i]) !? {
						value = if(value.class===Float) { value.asString } { value.cs };
						res = res ++ ": " ++ value;
					};
				};
				res = res ++ "</span>";
			};
		};
		if (res.notEmpty and: par) {
			^("("++res++")");
		};
		^res;
	}

	*renderHeader {|stream, doc, body|
		var x, cats, m, z;
		var thisIsTheMainHelpFile;
		var folder = doc.path.dirname;
		var undocumented = false;
		var displayedTitle;
		if(folder==".",{folder=""});

		// FIXME: use SCDoc.helpTargetDir relative to baseDir
		baseDir = ".";
		doc.path.occurrencesOf($/).do {
			baseDir = baseDir ++ "/..";
		};

		thisIsTheMainHelpFile = (doc.title == "Help") and: {
			(folder == "") or:
			{ (thisProcess.platform.name === \windows) and: { folder == "Help" } }
		};

		stream
		<< "<!doctype html>"
		<< "<html lang='en'>"
		<< "<head><title>";

		if(thisIsTheMainHelpFile) {
			stream << "SuperCollider " << Main.version << " Help";
		} {
			stream << doc.title << " | SuperCollider " << Main.version << " Help";
		};

		stream
		<< "</title>\n"
		<< "<link rel='stylesheet' href='" << baseDir << "/scdoc.css' type='text/css' />\n"
		<< "<link rel='stylesheet' href='" << baseDir << "/frontend.css' type='text/css' />\n"
		<< "<link rel='stylesheet' href='" << baseDir << "/custom.css' type='text/css' />\n"
		<< "<meta http-equiv='Content-Type' content='text/html; charset=UTF-8' />\n"
		<< "<script>\n"
		<< "var helpRoot = '" << baseDir << "';\n"
		<< "var scdoc_title = '" << doc.title.escapeChar($') << "';\n"
		<< "var scdoc_sc_version = '" << Main.version << "';\n"
		<< "</script>\n"
		<< "<script src='" << baseDir << "/scdoc.js' type='text/javascript'></script>\n"
		<< "<script src='" << baseDir << "/docmap.js' type='text/javascript'></script>\n" // FIXME: remove?
		<< "<script src='" << baseDir << "/prettify.js' type='text/javascript'></script>\n"
		<< "<script src='" << baseDir << "/lang-sc.js' type='text/javascript'></script>\n"
		<< "</head>\n";

		stream
		<< "<body onload='fixTOC();prettyPrint()'>\n";


		displayedTitle = if(
			thisIsTheMainHelpFile,
			{ "SuperCollider " ++ Main.version },
			{ doc.title }
		);

		stream
		<< "<div id='toc'>\n"
		<< "<div id='toctitle'>" << displayedTitle << ":</div>\n"
		<< "<span class='toc_search'>Filter: <input id='toc_search'></span>";
		this.renderTOC(stream, body);
		stream << "</div>";

		stream
		<< "<div class='contents'>\n"
		<< "<div id='menubar'></div>\n"
		<< "<div class='header'>\n";

		if(thisIsTheMainHelpFile.not) {
			stream
			<< "<div id='label'>\n"
			<< "<span id='folder'>" << folder.asString;
			if(doc.isExtension) {
				stream << " (extension)";
			};
			stream << "</span>\n";

			doc.categories !? {
				// Prevent the label from starting with "|".
				if(folder.asString.size > 0) {
					stream << " | "
				};

				stream << "<span id='categories'>"

				<< (doc.categories.collect { | path |
					// get all the components of a category path ("UGens>Generators>Deterministic")
					// we link each crumb of the breadcrumbs separately.
					var pathElems = path.split($>);

					// the href for "UGens" will be "UGens", for "Generators" "UGens>Generators", etc.
					pathElems.collect { | elem, i |
						var atag = "<a href='" ++ baseDir +/+ "Browse.html#";
						atag ++ pathElems[0..i].join(">") ++ "'>"++ elem ++"</a>"
					}.join("&#8201;&gt;&#8201;"); // &#8201; is a thin space

				}.join(" | "))

				<< "</span>\n";
			};

			stream << "</div>";
		};

		stream << "<h1>" << displayedTitle;
		if(thisIsTheMainHelpFile) {
			stream << "<span class='headerimage'><img src='" << baseDir << "/images/SC_icon.png'/></span>";
		};
		if(doc.isClassDoc and: { currentClass.notNil } and: { currentClass != Object }) {
			stream << "<span id='superclasses'>"
			<< " : "
			<< (currentClass.superclasses.collect {|c|
				"<a href=\"../Classes/"++c.name++".html\">"++c.name++"</a>"
			}.join(" : "))
			<< "</span>\n";
		};
		if(doc.isExtension) {
			stream
			<< "<div class='extension-indicator-ctr' title='This help file originates from a third-party quark or plugin for SuperCollider.'>"
			<< "<img class='extension-indicator-icon' alt='Extension' src='" << baseDir << "/images/plugin.png'>"
			<< "<span class='extension-indicator-text'>Extension</span>"
			<< "</div>";
		};
		stream
		<< "</h1>\n"
		<< "<div id='summary'>" << this.escapeSpecialChars(doc.summary) << "</div>\n"
		<< "</div>\n"
		<< "<div class='subheader'>\n";

		if(doc.isClassDoc) {
			if(currentClass.notNil) {
				m = currentClass.filenameSymbol.asString;
				stream << "<div id='filename'>Source: "
				<< "<a href='%' title='%'>".format(URI.fromLocalPath(m).asString, m)
				<< m.basename << "</a></div>";
				if(currentClass.subclasses.notNil) {
					z = false;
					stream << "<div id='subclasses'>"
					<< "Subclasses: "
					<< (currentClass.subclasses.collect(_.name).sort.collect {|c,i|
						if(i==4,{z=true;"<span id='hiddensubclasses' style='display:none;'>"},{""})
						++"<a href=\"../Classes/"++c++".html\">"++c++"</a>"
					}.join(", "));
					if(z) {
						stream << "</span><a class='subclass_toggle' href='#' onclick='javascript:showAllSubclasses(this); return false'>&hellip;&nbsp;see&nbsp;all</a>";
					};
					stream << "</div>\n";
				};
				if(currentImplClass.notNil) {
					stream << "<div class='inheritance'>Implementing class: "
					<< "<a href=\"../Classes/" << currentImplClass.name << ".html\">"
					<< currentImplClass.name << "</a></div>\n";
				};
			} {
				stream << "<div id='filename'>Location: <b>NOT INSTALLED!</b></div>\n";
			};
		};

		doc.related !? {
			stream << "<div id='related'>See also: "
			<< (doc.related.collect {|r| this.htmlForLink(r)}.join(", "))
			<< "</div>\n";
		};

		// FIXME: Remove this when conversion to new help system is done!
		if(doc.isUndocumentedClass and: {Help.respondsTo('findHelpFile')}) {
			x = Help.findHelpFile(name);
			x !? {
				stream << ("[ <a href='" ++ baseDir ++ "/OldHelpWrapper.html#"
				++x++"?"++SCDoc.helpTargetDir +/+ doc.path ++ ".html"
				++"'>old help</a> ]")
			};
		};

		stream << "</div>\n";
	}

	*renderChildren {|stream, node|
		node.children.do {|child| this.renderSubTree(stream, child) };
	}

	*renderMethod {|stream, node, methodType, cls, icls|
		var methodTypeIndicator;
		var methodCodePrefix;
		var args = node.text ?? ""; // only outside class/instance methods
		var names = node.children[0].children.collect(_.text);
		var mstat, sym, m, m2, mname2;
		var lastargs, args2;
		var x, maxargs = -1;
		var methArgsMismatch = false;

		methodTypeIndicator = switch(
			methodType,
			\classMethod, { "*" },
			\instanceMethod, { "-" },
			\genericMethod, { "" }
		);

		minArgs = inf;
		currentMethod = nil;
		names.do {|mname|
			methodCodePrefix = switch(
				methodType,
				\classMethod, { if(cls.notNil) { cls.name.asString[5..] } { "" } ++ "." },
				\instanceMethod, {
					// If the method name contains any valid binary operator character, remove the
					// "." to reduce confusion.
					if(mname.asString.any(this.binaryOperatorCharacters.contains(_)), { "" }, { "." })
				},
				\genericMethod, { "" }
			);

			mname2 = this.escapeSpecialChars(mname);
			if(cls.notNil) {
				mstat = 0;
				sym = mname.asSymbol;
				//check for normal method or getter
				m = icls !? {icls.findRespondingMethodFor(sym.asGetter)};
				m = m ?? {cls.findRespondingMethodFor(sym.asGetter)};
				m !? {
					mstat = mstat | 1;
					args = this.makeArgString(m);
					args2 = m.argNames !? {m.argNames[1..]};
				};
				//check for setter
				m2 = icls !? {icls.findRespondingMethodFor(sym.asSetter)};
				m2 = m2 ?? {cls.findRespondingMethodFor(sym.asSetter)};
				m2 !? {
					mstat = mstat | 2;
					args = m2.argNames !? {this.makeArgString(m2,false)} ?? {"value"};
					args2 = m2.argNames !? {m2.argNames[1..]};
				};
				maxargs.do {|i|
					var a = args2[i];
					var b = lastargs[i];
					if(a!=b and: {a!=nil} and: {b!=nil}) {
						methArgsMismatch = true;
					}
				};
				lastargs = args2;
				case
					{args2.size>maxargs} {
						maxargs = args2.size;
						currentMethod = m2 ?? m;
					}
					{args2.size<minArgs} {
						minArgs = args2.size;
					};
			} {
				m = nil;
				m2 = nil;
				mstat = 1;
			};

			x = {
				stream << "<h3 class='method-code'>"
				<< "<span class='method-prefix'>" << methodCodePrefix << "</span>"
				<< "<a class='method-name' name='" << methodTypeIndicator << mname << "' href='"
				<< baseDir << "/Overviews/Methods.html#"
				<< mname2 << "'>" << mname2 << "</a>"
			};

			switch (mstat,
				// getter only
				1, { x.value; stream << args; },
				// getter and setter
				3, { x.value; },
				// method not found
				0, {
					"SCDoc: In %\n"
					"  Method %% not found.".format(currDoc.fullPath, methodTypeIndicator, mname2).warn;
					x.value;
					stream << ": METHOD NOT FOUND!";
				}
			);

			stream << "</h3>\n";

			// has setter
			if(mstat & 2 > 0) {
				x.value;
				if(args2.size<2) {
					stream << " = " << args << "</h3>\n";
				} {
					stream << "_(" << args << ")</h3>\n";
				}
			};

			m = m ?? m2;
			m !? {
				if(m.isExtensionOf(cls) and: {icls.isNil or: {m.isExtensionOf(icls)}}) {
					stream << "<div class='extmethod'>From extension in <a href='"
					<< URI.fromLocalPath(m.filenameSymbol.asString).asString << "'>"
					<< m.filenameSymbol << "</a></div>\n";
				} {
					if(m.ownerClass == icls) {
						stream << "<div class='supmethod'>From implementing class</div>\n";
					} {
						if(m.ownerClass != cls) {
							m = m.ownerClass.name;
							m = if(m.isMetaClassName) {m.asString.drop(5)} {m};
							stream << "<div class='supmethod'>From superclass: <a href='"
							<< baseDir << "/Classes/" << m << ".html'>" << m << "</a></div>\n";
						}
					}
				};
			};
		};

		if(methArgsMismatch) {
			"SCDoc: In %\n"
			"  Grouped methods % do not have the same argument signature."
			.format(currDoc.fullPath, names).warn;
		};

		// ignore trailing mul add arguments
		if(currentMethod.notNil) {
			currentNArgs = currentMethod.argNames.size;
			if(currentNArgs > 2
			and: {currentMethod.argNames[currentNArgs-1] == \add}
			and: {currentMethod.argNames[currentNArgs-2] == \mul}) {
				currentNArgs = currentNArgs - 2;
			}
		} {
			currentNArgs = 0;
		};

		if(node.children.size > 1) {
			stream << "<div class='method'>";
			this.renderChildren(stream, node.children[1]);
			stream << "</div>";
		};
		currentMethod = nil;
	}

	*renderSubTree {|stream, node|
		var f, z, img;
		switch(node.id,
			\PROSE, {
				if(noParBreak) {
					noParBreak = false;
				} {
					stream << "\n<p>";
				};
				this.renderChildren(stream, node);
			},
			\NL, { }, // these shouldn't be here..
// Plain text and modal tags
			\TEXT, {
				stream << this.escapeSpecialChars(node.text);
			},
			\LINK, {
				stream << this.htmlForLink(node.text);
			},
			\CODEBLOCK, {
				stream << "<pre class='code prettyprint lang-sc'>"
				<< this.escapeSpecialChars(node.text)
				<< "</pre>\n";
			},
			\CODE, {
				stream << "<code class='code prettyprint lang-sc'>"
				<< this.escapeSpecialChars(node.text)
				<< "</code>";
			},
			\EMPHASIS, {
				stream << "<em>" << this.escapeSpecialChars(node.text) << "</em>";
			},
			\TELETYPEBLOCK, {
				stream << "<pre>" << this.escapeSpecialChars(node.text) << "</pre>";
			},
			\TELETYPE, {
				stream << "<code>" << this.escapeSpecialChars(node.text) << "</code>";
			},
			\STRONG, {
				stream << "<strong>" << this.escapeSpecialChars(node.text) << "</strong>";
			},
			\SOFT, {
				stream << "<span class='soft'>" << this.escapeSpecialChars(node.text) << "</span>";
			},
			\ANCHOR, {
				stream << "<a class='anchor' name='" << this.escapeSpacesInAnchor(node.text) << "'>&nbsp;</a>";
			},
			\KEYWORD, {
				node.children.do {|child|
					stream << "<a class='anchor' name='kw_" << this.escapeSpacesInAnchor(child.text) << "'>&nbsp;</a>";
				}
			},
			\IMAGE, {
				f = node.text.split($#);
				stream << "<div class='image'>";
				img = "<img src='" ++ f[0] ++ "'/>";
				if(f[2].isNil) {
					stream << img;
				} {
					stream << this.htmlForLink(f[2]++"#"++(f[3]?"")++"#"++img,false);
				};
				f[1] !? { stream << "<br><b>" << f[1] << "</b>" }; // ugly..
				stream << "</div>\n";
			},
// Other stuff
			\NOTE, {
				stream << "<div class='note'><span class='notelabel'>NOTE:</span> ";
				noParBreak = true;
				this.renderChildren(stream, node);
				stream << "</div>";
			},
			\WARNING, {
				stream << "<div class='warning'><span class='warninglabel'>WARNING:</span> ";
				noParBreak = true;
				this.renderChildren(stream, node);
				stream << "</div>";
			},
			\FOOTNOTE, {
				footNotes = footNotes.add(node);
				stream << "<a class='footnote anchor' name='footnote_org_"
				<< footNotes.size
				<< "' href='#footnote_"
				<< footNotes.size
				<< "'><sup>"
				<< footNotes.size
				<< "</sup></a> ";
			},
			\CLASSTREE, {
				stream << "<ul class='tree'>";
				this.renderClassTree(stream, node.text.asSymbol.asClass);
				stream << "</ul>";
			},
// Lists and tree
			\LIST, {
				stream << "<ul>\n";
				this.renderChildren(stream, node);
				stream << "</ul>\n";
			},
			\TREE, {
				stream << "<ul class='tree'>\n";
				this.renderChildren(stream, node);
				stream << "</ul>\n";
			},
			\NUMBEREDLIST, {
				stream << "<ol>\n";
				this.renderChildren(stream, node);
				stream << "</ol>\n";
			},
			\ITEM, { // for LIST, TREE and NUMBEREDLIST
				stream << "<li>";
				noParBreak = true;
				this.renderChildren(stream, node);
			},
// Definitionlist
			\DEFINITIONLIST, {
				stream << "<dl>\n";
				this.renderChildren(stream, node);
				stream << "</dl>\n";
			},
			\DEFLISTITEM, {
				this.renderChildren(stream, node);
			},
			\TERM, {
				stream << "<dt>";
				noParBreak = true;
				this.renderChildren(stream, node);
			},
			\DEFINITION, {
				stream << "<dd>";
				noParBreak = true;
				this.renderChildren(stream, node);
			},
// Tables
			\TABLE, {
				stream << "<table>\n";
				this.renderChildren(stream, node);
				stream << "</table>\n";
			},
			\TABROW, {
				stream << "<tr>";
				this.renderChildren(stream, node);
			},
			\TABCOL, {
				stream << "<td>";
				noParBreak = true;
				this.renderChildren(stream, node);
			},
// Methods
			\CMETHOD, {
				this.renderMethod(
					stream, node,
					\classMethod,
					currentClass !? {currentClass.class},
					currentImplClass !? {currentImplClass.class}
				);
			},
			\IMETHOD, {
				this.renderMethod(
					stream, node,
					\instanceMethod,
					currentClass,
					currentImplClass
				);
			},
			\METHOD, {
				this.renderMethod(
					stream, node,
					\genericMethod,
					nil, nil
				);
			},
			\CPRIVATE, {},
			\IPRIVATE, {},
			\COPYMETHOD, {},
			\CCOPYMETHOD, {},
			\ICOPYMETHOD, {},
			\ARGUMENTS, {
				stream << "<h4>Arguments:</h4>\n<table class='arguments'>\n";
				currArg = 0;
				if(currentMethod.notNil and: {node.children.size < (currentNArgs-1)}) {
					"SCDoc: In %\n"
					"  Method %% has % args, but doc has % argument:: tags.".format(
						currDoc.fullPath,
						if(currentMethod.ownerClass.isMetaClass) {"*"} {"-"},
						currentMethod.name,
						currentNArgs-1,
						node.children.size,
					).warn;
				};
				this.renderChildren(stream, node);
				stream << "</table>";
			},
			\ARGUMENT, {
				currArg = currArg + 1;
				stream << "<tr><td class='argumentname'>";
				if(node.text.isNil) {
					currentMethod !? {
						if(currentMethod.varArgs and: {currArg==(currentMethod.argNames.size-1)}) {
							stream << "... ";
						};
						stream << if(currArg < currentMethod.argNames.size) {
							if(currArg > minArgs) {
								"("++currentMethod.argNames[currArg]++")";
							} {
								currentMethod.argNames[currArg];
							}
						} {
							"(arg"++currArg++")" // excessive arg
						};
					};
				} {
					stream << if(currentMethod.isNil or: {currArg < currentMethod.argNames.size}) {
						currentMethod !? {
							f = currentMethod.argNames[currArg].asString;
							if(
								(z = if(currentMethod.varArgs and: {currArg==(currentMethod.argNames.size-1)})
										{"... "++f} {f}
								) != node.text;
							) {
								"SCDoc: In %\n"
								"  Method %% has arg named '%', but doc has 'argument:: %'.".format(
									currDoc.fullPath,
									if(currentMethod.ownerClass.isMetaClass) {"*"} {"-"},
									currentMethod.name,
									z,
									node.text,
								).warn;
							};
						};
						if(currArg > minArgs) {
							"("++node.text++")";
						} {
							node.text;
						};
					} {
						"("++node.text++")" // excessive arg
					};
				};
				stream << "<td class='argumentdesc'>";
				this.renderChildren(stream, node);
			},
			\RETURNS, {
				stream << "<h4>Returns:</h4>\n<div class='returnvalue'>";
				this.renderChildren(stream, node);
				stream << "</div>";

			},
			\DISCUSSION, {
				stream << "<h4>Discussion:</h4>\n";
				this.renderChildren(stream, node);
			},
// Sections
			\CLASSMETHODS, {
				if(node.notPrivOnly) {
					stream << "<h2><a class='anchor' name='classmethods'>Class Methods</a></h2>\n";
				};
				this.renderChildren(stream, node);
			},
			\INSTANCEMETHODS, {
				if(node.notPrivOnly) {
					stream << "<h2><a class='anchor' name='instancemethods'>Instance Methods</a></h2>\n";
				};
				this.renderChildren(stream, node);
			},
			\DESCRIPTION, {
				stream << "<h2><a class='anchor' name='description'>Description</a></h2>\n";
				this.renderChildren(stream, node);
			},
			\EXAMPLES, {
				stream << "<h2><a class='anchor' name='examples'>Examples</a></h2>\n";
				this.renderChildren(stream, node);
			},
			\SECTION, {
				stream << "<h2><a class='anchor' name='" << this.escapeSpacesInAnchor(node.text)
				<< "'>" << this.escapeSpecialChars(node.text) << "</a></h2>\n";
				if(node.makeDiv.isNil) {
					this.renderChildren(stream, node);
				} {
					stream << "<div id='" << node.makeDiv << "'>";
					this.renderChildren(stream, node);
					stream << "</div>";
				};
			},
			\SUBSECTION, {
				stream << "<h3><a class='anchor' name='" << this.escapeSpacesInAnchor(node.text)
				<< "'>" << this.escapeSpecialChars(node.text) << "</a></h3>\n";
				if(node.makeDiv.isNil) {
					this.renderChildren(stream, node);
				} {
					stream << "<div id='" << node.makeDiv << "'>";
					this.renderChildren(stream, node);
					stream << "</div>";
				};
			},
			{
				"SCDoc: In %\n"
				"  Unknown SCDocNode id: %".format(currDoc.fullPath, node.id).warn;
				this.renderChildren(stream, node);
			}
		);
	}

	*renderTOC {|stream, node|
		node.children !? {
			stream << "<ul class='toc'>";
			node.children.do {|n|
				switch(n.id,
					\DESCRIPTION, {
						stream << "<li class='toc1'><a href='#description'>Description</a></li>\n";
						this.renderTOC(stream, n);
					},
					\EXAMPLES, {
						stream << "<li class='toc1'><a href='#examples'>Examples</a></li>\n";
						this.renderTOC(stream, n);
					},
					\CLASSMETHODS, {
						if(n.notPrivOnly) {
							stream << "<li class='toc1'><a href='#classmethods'>Class methods</a></li>\n";
							this.renderTOC(stream, n);
						};
					},
					\INSTANCEMETHODS, {
						if(n.notPrivOnly) {
							stream << "<li class='toc1'><a href='#instancemethods'>Instance methods</a></li>\n";
							this.renderTOC(stream, n);
						};
					},
					\CMETHOD, {
						stream << "<li class='toc3'>"
						<< (n.children[0].children.collect{|m|
							"<a href='#*"++m.text++"'>"++this.escapeSpecialChars(m.text)++"</a> ";
						}.join(" "))
						<< "</li>\n";
					},
					\IMETHOD, {
						stream << "<li class='toc3'>"
						<< (n.children[0].children.collect{|m|
							"<a href='#-"++m.text++"'>"++this.escapeSpecialChars(m.text)++"</a> ";
						}.join(" "))
						<< "</li>\n";
					},
					\METHOD, {
						stream << "<li class='toc3'>"
						<< (n.children[0].children.collect{|m|
							"<a href='#."++m.text++"'>"++this.escapeSpecialChars(m.text)++"</a> ";
						}.join(" "))
						<< "</li>\n";
					},

					\SECTION, {
						stream << "<li class='toc1'><a href='#" << this.escapeSpacesInAnchor(n.text) << "'>"
						<< this.escapeSpecialChars(n.text) << "</a></li>\n";
						this.renderTOC(stream, n);
					},
					\SUBSECTION, {
						stream << "<li class='toc2'><a href='#" << this.escapeSpacesInAnchor(n.text) << "'>"
						<< this.escapeSpecialChars(n.text) << "</a></li>\n";
						this.renderTOC(stream, n);
					}
				);
			};
			stream << "</ul>";
		};
	}

	*addUndocumentedMethods {|list, body, id2, id, title|
		var l;
		if(list.size>0) {
			l = list.collectAs(_.asString,Array).sort.collect {|name|
				SCDocNode()
				.id_(id2)
				.children_([
					SCDocNode()
					.id_(\METHODNAMES)
					.children_([
						SCDocNode()
						.id_(\STRING)
						.text_(name.asString)
					])
				]);
			};
			body.addDivAfter(id, nil, title, l);
		}
	}

	*renderClassTree {|stream, cls|
		var name, doc, desc = "";
		name = cls.name.asString;
		doc = SCDoc.documents["Classes/"++name];
		doc !? { desc = " - "++doc.summary };
		if(cls.name.isMetaClassName, {^this});
		stream << "<li> <a href='" << baseDir << "/Classes/" << name << ".html'>"
		<< name << "</a>" << desc << "\n";

		cls.subclasses !? {
			stream << "<ul class='tree'>\n";
			cls.subclasses.copy.sort {|a,b| a.name < b.name}.do {|x|
				this.renderClassTree(stream, x);
			};
			stream << "</ul>\n";
		};
	}

	*renderFootNotes {|stream|
		if(footNotes.notNil) {
			stream << "<div class='footnotes'>\n";
			footNotes.do {|n,i|
				stream << "<a class='anchor' name='footnote_" << (i+1) << "'/><div class='footnote'>"
				<< "[<a href='#footnote_org_" << (i+1) << "'>" << (i+1) << "</a>] - ";
				noParBreak = true;
				this.renderChildren(stream, n);
				stream << "</div>";
			};
			stream << "</div>";
		};
	}

	*renderFooter {|stream, doc|
		stream << "<div class='doclink'>";
		doc.fullPath !? {
			stream << "helpfile source: <a href='" << URI.fromLocalPath(doc.fullPath).asString << "'>"
			<< doc.fullPath << "</a><br>"
		};
		stream << "link::" << doc.path << "::<br>"
		<< "</div>"
		<< "</div></body></html>";
	}

	*renderOnStream {|stream, doc, root|
		var body = root.children[1];
		var redirect;
		currDoc = doc;
		footNotes = nil;
		noParBreak = false;

		if(doc.isClassDoc) {
			currentClass = doc.klass;
			currentImplClass = doc.implKlass;
			if(currentClass != Object) {
				body.addDivAfter(\CLASSMETHODS,"inheritedclassmets","Inherited class methods");
				body.addDivAfter(\INSTANCEMETHODS,"inheritedinstmets","Inherited instance methods");
			};
			this.addUndocumentedMethods(doc.undoccmethods, body, \CMETHOD, \CLASSMETHODS, "Undocumented class methods");
			this.addUndocumentedMethods(doc.undocimethods, body, \IMETHOD, \INSTANCEMETHODS, "Undocumented instance methods");
			body.sortClassDoc;
		} {
			currentClass = nil;
			currentImplClass = nil;
		};

		this.renderHeader(stream, doc, body);
		this.renderChildren(stream, body);
		this.renderFootNotes(stream);
		this.renderFooter(stream, doc);
		currDoc = nil;
	}

	*renderToFile {|filename, doc, root|
		var stream;
		File.mkdir(filename.dirname);
		stream = File(filename, "w");
		if(stream.isOpen) {
			this.renderOnStream(stream, doc, root);
			stream.close;
		} {
			warn("SCDoc: Could not open file % for writing".format(filename));
		}
	}
}

/*
HTML renderer
*/
SCDocHTMLRenderer {
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

    *htmlForLink {|link|
        var n, m, f, c, doc;
        // FIXME: how slow is this? can we optimize
        #n, m, f = link.split($#); // link, anchor, label
        ^if ("^[a-zA-Z]+://.+".matchRegexp(link) or: (link.first==$/)) {
            if(f.size<1) {f=link};
            c = if(m.size>0) {n++"#"++m} {n};
            "<a href=\""++c++"\">"++this.escapeSpecialChars(f)++"</a>";
        } {
            if(n.size>0) {
                c = baseDir+/+n;
                doc = SCDoc.documents[n];
                // link to other doc (might not be rendered yet)
                if(doc.notNil) {
                    c = c ++ ".html";
                } {
                    // link to ready-made html (Search, Browse, etc)
                    if(File.exists(SCDoc.helpTargetDir+/+n++".html")) {
                        c = c ++ ".html";
                    } {
                        // link to other file?
                        if(File.exists(SCDoc.helpTargetDir+/+n).not) {
                            "SCDoc: In %\n"
                            "  Broken link: '%'"
                            .format(currDoc.fullPath, link).warn;
                        };
                    };
                };
            } {
                c = ""; // link inside same document
            };
            if(m.size>0) { c = c ++ "#" ++ m }; // add #anchor
            if(f.size<1) { // no label
                if(n.size>0) {
                    f = if(doc.notNil) {doc.title} {n.basename};
                    if(m.size>0) {
                        f = f++": "++m;
                    }
                } {
                    f = if(m.size>0) {m} {"(empty link)"};
                };
            };
            "<a href=\""++c++"\">"++this.escapeSpecialChars(f)++"</a>";
        };
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

    *renderHeader {|stream, doc|
        var x, cats, m, z;
        var folder = doc.path.dirname;
        var undocumented = false;
        if(folder==".",{folder=""});
        
        // FIXME: use SCDoc.helpTargetDir relative to baseDir
        baseDir = ".";
        doc.path.occurrencesOf($/).do {
            baseDir = baseDir ++ "/..";
        };

        stream
        << "<html><head><title>" << doc.title << "</title>\n"
        << "<link rel='stylesheet' href='" << baseDir << "/scdoc.css' type='text/css' />\n"
        << "<link rel='stylesheet' href='" << baseDir << "/frontend.css' type='text/css' />\n"
        << "<link rel='stylesheet' href='" << baseDir << "/custom.css' type='text/css' />\n"
        << "<meta http-equiv='Content-Type' content='text/html; charset=UTF-8' />\n"
        << "<script src='" << baseDir << "/scdoc.js' type='text/javascript'></script>\n"
        << "<script src='" << baseDir << "/docmap.js' type='text/javascript'></script>\n" // FIXME: remove?
        << "<script src='" << baseDir << "/prettify.js' type='text/javascript'></script>\n"
        << "<script src='" << baseDir << "/lang-sc.js' type='text/javascript'></script>\n"
        << "<script src='" << baseDir << "/MathJax/MathJax.js?config=TeX-AMS_HTML,scmathjax' type='text/javascript'></script>\n"
        << "<script type='text/javascript'>var helpRoot='" << baseDir << "';</script>\n"
        << "</head>\n";

        stream
        << "<ul id='menubar'></ul>\n"
        << "<body onload='fixTOC();prettyPrint()'>\n"
        << "<div class='contents'>\n"
        << "<div class='header'>\n"
        << "<div id='label'>SuperCollider " << folder.asString.toUpper;
        if(doc.isExtension) {
            stream << " (extension)";
        };
        stream << "</div>\n";

        doc.categories !? {
            stream
            << "<div id='categories'>"
            << (doc.categories.collect {|r|
                "<a href='"++baseDir +/+ "Browse.html#"++r++"'>"++r++"</a>"
            }.join(", "))
            << "</div>\n";
        };

        stream << "<h1>" << doc.title;
        if((folder=="") and: {doc.title=="Help"}) {
            stream << "<span class='headerimage'><img src='" << baseDir << "/images/SC_icon.png'/></span>";
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
                << m.dirname << "/<a href='" << URI.fromLocalPath(m).asString << "'>"
                << m.basename << "</a></div>";
                if(currentClass != Object) {
                    stream << "<div id='superclasses'>"
                    << "Inherits from: "
                    << (currentClass.superclasses.collect {|c|
                        "<a href=\"../Classes/"++c.name++".html\">"++c.name++"</a>"
                    }.join(" : "))
                    << "</div>\n";
                };
                if(currentClass.subclasses.notNil) {
                    z = false;
                    stream << "<div id='subclasses'>"
                    << "Subclasses: "
                    << (currentClass.subclasses.collect(_.name).sort.collect {|c,i|
                        if(i==12,{z=true;"<span id='hiddensubclasses' style='display:none;'>"},{""})
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
        if(doc.isUndocumentedClass) {
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

    *renderMethod {|stream, node, cls, icls, css, pfx|
        var args = node.text ?? ""; // only outside class/instance methods
        var names = node.children[0].children.collect(_.text);
        var mstat, sym, m, m2, mname2;
        var lastargs, args2;
        var x, maxargs = -1;
        var methArgsMismatch = false;
        minArgs = inf;
        currentMethod = nil;
        names.do {|mname|
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
                    }
                ;
            } {
                m = nil;
                m2 = nil;
                mstat = 1;
            };

            x = {
                stream << "<h3 class='" << css << "'>"
                << "<span class='methprefix'>" << (pfx??"&nbsp;") << "</span>"
                << "<a name='" << (pfx??".") << mname << "' href='"
                << baseDir << "/Overviews/Methods.html#"
                << mname2 << "'>" << mname2 << "</a>"
            };

            x.value;
            switch (mstat,
                // getter only
                1, { stream << " " << args << "</h3>\n"; },
                // getter and setter
                3, { stream << "</h3>\n"; },
                // method not found
                0, {
                    "SCDoc: In %\n"
                    "  Method %% not found.".format(currDoc.fullPath,pfx,mname2).warn;
                    stream << ": METHOD NOT FOUND!</h3>\n";
                }
            );

            // has setter
            if(mstat & 2 > 0) {
                x.value;
                if(args2.size<2) {
                    stream << " = " << args << "</h3>\n";
                } {
                    stream << "_ (" << args << ")</h3>\n";
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
            "  Grouped methods % does not have the same argument signature."
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
        var f, z;
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
                stream << "<a class='anchor' name='" << node.text << "'>&nbsp;</a>";
            },
            \KEYWORD, {
                node.children.do {|child|
                    stream << "<a class='anchor' name='kw_" << child.text << "'>&nbsp;</a>";
                }
            },
            \MATHBLOCK, { // uses MathJax to typeset TeX math
                stream << "<span class='math'>\\[\n"
                << this.escapeSpecialChars(node.text)
                << "\n\\]\n</span>";
            },
            \MATH, {
                stream << "<span class='math'>\\("
                << this.escapeSpecialChars(node.text)
                << "\\)</span>";
            },
            \IMAGE, {
                f = node.text.split($#);
                stream << "<div class='image'><img src='" << f[0] << "'/>";
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
                this.renderMethod (
                    stream, node,
                    currentClass !? {currentClass.class},
                    currentImplClass !? {currentImplClass.class},
                    "cmethodname", "*"
                );
            },
            \IMETHOD, {
                this.renderMethod (
                    stream, node,
                    currentClass,
                    currentImplClass,
                    "imethodname", "-"
                );
            },
            \METHOD, {
                this.renderMethod (
                    stream, node,
                    nil, nil,
                    "imethodname", nil
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
                stream << "<h2><a class='anchor' name='" << node.text
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
                stream << "<h3><a class='anchor' name='" << node.text
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
                        stream << "<li class='toc1'><a href='#" << n.text << "'>"
                        << this.escapeSpecialChars(n.text) << "</a></li>\n";
                        this.renderTOC(stream, n);
                    },
                    \SUBSECTION, {
                        stream << "<li class='toc2'><a href='#" << n.text << "'>"
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
            stream << "source: <a href='" << URI.fromLocalPath(doc.fullPath).asString << "'>"
            << doc.fullPath << "</a><br>"
        };
        stream << "link::" << doc.path << "::<br>"
        << "sc version: " << Main.version << "</div>"
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
        
        this.renderHeader(stream, doc);
        stream << "<div id='toc'>\n";
        this.renderTOC(stream, body);
        stream << "</div>";
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


/*
abstract superclass for SCDoc renderers
*/
SCDocRenderer {
    // render the document tree of parser.root into outputFile, relative to given destination help folder
    render {|parser, outputFile, folder, makeTOC=true|
        ^this.subclassResponsibility(thisMethod);
    }
    // return the URL for help on given string
    findHelpFile {|str|
        ^this.subclassResponsibility(thisMethod);
    }
}

/*
HTML renderer
*/
SCDocHTMLRenderer : SCDocRenderer {
    var parser;
    var currentClass;
    var currentImplClass;
    var collectedArgs;
    var dirLevel;
    var baseDir;
    var footNotes;
    classvar <>enableClassLinksInCode = true;

    *simplifyName {|txt|
        ^txt.toLower.tr($\ ,$_);
    }

    findHelpFile {|str|
        var old, sym, pfx = "file://" ++ SCDoc.helpTargetDir;

        if(str.isNil or: {str.isEmpty}) { ^pfx +/+ "Help.html" };
        if(SCDoc.findHelpSource(str).notNil) { ^pfx +/+ str ++ ".html" };

        sym = str.asSymbol;
        if(sym.asClass.notNil) {
            ^pfx +/+ (if(SCDoc.findHelpSource("Classes"+/+str).isNil) {
                (old = Help.findHelpFile(str)) !? {
                    "OldHelpWrapper.html#"++old++"?"++SCDoc.helpTargetDir +/+ "Classes" +/+ str ++ ".html"
                }
            } ?? { "Classes" +/+ str ++ ".html" })
        };

        ^pfx +/+ if(block {|brk|
            Class.allClasses.do{|c| if(c.findMethod(sym).notNil) {brk.value(true)}};
            false;
        }) {
            "Overviews/Methods.html#" ++ str
        } { "Search.html#" ++ str }
    }

    *checkBrokenLinks {
        var f,m,p,file;
        var check_link = {|link|
            if(("^[a-zA-Z]+://.+".matchRegexp(link) or: (link.first==$/)).not) {
                f = link.split($#);
                m = SCDoc.helpTargetDir +/+ f[0] ++ ".html";
                if((f[0]!="") and: {File.exists(m).not}) {
                    postln("Broken link: "++file++": "++link);
                };
            };
        };
        var do_children = {|children|
            children.do {|node|
                switch(node.tag,
                    \link, {
                        check_link.(node.text);
                    },
                    \related, {
                        SCDoc.splitList(node.text).do {|l|
                            check_link.(l);
                        };
                    },
                    {
                        node.children !? {
                            do_children.(node.children);
                        }
                    }
                );
            };
        };
        p = SCDocParser.new;
        PathName(SCDoc.helpSourceDir).filesDo {|path|
            var source = path.fullPath;
            var lastDot = source.findBackwards(".");
            var ext = source.copyToEnd(lastDot);
            if(ext == ".schelp", {
                file = source;
                p.parseFile(source);
                do_children.(p.root);
            });
        };
        postln("Done");
    }

    escapeSpecialChars {|str|
        ^str.replace("&","&amp;").replace("<","&lt;").replace(">","&gt;");
    }

    renderFootNotes {|file|
        if (footNotes.notEmpty) {
            file.write("<div class='footnotes'>\n");
            footNotes.do {|n,i|
                file.write("<a class='anchor' name='footnote_"++(i+1)++"'/><div class='footnote'>");
                file.write("[<a href='#footnote_org_"++(i+1)++"'>"++(i+1)++"</a>] - ");
                n.children.do(this.renderHTMLSubTree(file,_));
                file.write("</div>");
            };
            file.write("</div>");
        };
    }

    autoLinkClasses {|txt|
        var x = "", z = 0;
        if(enableClassLinksInCode) {
            // FIXME: also match plural? like "Floats".. - if word was not class but ends in s and is a class without that s, then use that.
            // it would be useful in prose, but not code examples..
            txt.findRegexp("\\b[A-Z][A-Za-z_0-9]+").select{|m| m[1].asSymbol.asClass.notNil}.do {|m|
                x = x ++ txt.copyRange(z,m[0]-1);
                x = x ++ "<a class='clslnk' href='"++baseDir+/+"Classes"+/+m[1]++".html'>" ++ m[1] ++ "</a>";
                z = m[0] + m[1].size;
            };
            ^(x ++ txt.copyToEnd(z));
        } {
            ^txt;
        }
    }

    *makeArgString {|m|
        var res = "";
        var value;
        var l = m.argNames;
        var last = l.size-1;
        l.do {|a,i|
            if (i>0) { //skip 'this' (first arg)
                if(i==last and: {m.varArgs}) {
                    res = res ++ " <span class='argstr'>";
                    res = res ++ "... " ++ a;
                } {
                    if (i>1) { res = res ++ ", " };
                    res = res ++ "<span class='argstr'>";
                    res = res ++ a;
                    value = m.prototypeFrame[i];
                    if (value.notNil) {
                        value = switch(value.class,
                            Symbol, { "'"++value.asString++"'" },
                            Char, { "$"++value.asString },
                            String, { "\""++value.asString++"\"" },
                            { value.asString }
                        );
                        res = res ++ " = " ++ value.asString;
                    };
                };
                res = res ++ "</span>";
            };
        };
        if (res.notEmpty) {
            ^("("++res++")");
        } {
            ^"";
        };
    }

    renderHTMLSubTree {|file,node,parentTag=false|
        var c, ic, f, m, m2, n, mname, args, split, mstat, sym, css, pfx;

        var do_children = {|p=false|
            node.children !? {
                node.children.do {|e| this.renderHTMLSubTree(file,e,if(p,{node.tag},{parentTag})) };
            };
        };

        switch(node.tag,
            'prose', {
                if(node.display == \block, {
                    file.write("\n<p>");
                });
                // FIXME: using autoLinkClasses here is often very nice, but a bit ugly when the word is not meant to be a class,
                // like "In this building lives a big cat" or "Control your mind", etc..
                // one workaround might be to don't match some words if they start a sentence??
//                file.write(this.autoLinkClasses(this.escapeSpecialChars(node.text)));
                file.write(this.escapeSpecialChars(node.text));
            },
            'section', {
                file.write("<h2><a class='anchor' name='"++this.class.simplifyName(node.text)++"'>"++this.escapeSpecialChars(node.text)++"</a></h2>\n");
                if(node.makeDiv.isNil) {
                    do_children.();
                } {
                    file.write("<div id='"++node.makeDiv++"'>");
                    do_children.();
                    file.write("</div>");
                };
            },
            'subsection', {
                file.write("<h3><a class='anchor' name='"++this.class.simplifyName(node.text)++"'>"++this.escapeSpecialChars(node.text)++"</a></h3>\n");
                if(node.makeDiv.isNil) {
                    do_children.();
                } {
                    file.write("<div id='"++node.makeDiv++"'>");
                    do_children.();
                    file.write("</div>");
                };
            },
            'classmethods', {
                if(node.children.select{|n|n.tag!=\private}.notEmpty) { //FIXME use detect
                    file.write("<h2><a class='anchor' name='classmethods'>Class Methods</a></h2>\n");
                    do_children.(true);
                } {
                    do_children.(true);
                };
            },
            'instancemethods', {
                if(node.children.select{|n|n.tag!=\private}.notEmpty) { //FIXME use detect
                    file.write("<h2><a class='anchor' name='instancemethods'>Instance Methods</a></h2>\n");
                    do_children.(true);
                } {
                    do_children.(true);
                };
            },
            'private', {
            },
            'method', {
                //for multiple methods with same signature and similar function:
                //ar kr (x = 42, y = 123)
                f = node.text.findRegexp(" *([^(]+) *(\\(.*\\))?");
//                args = "";
                args = f[2][1];
//FIXME: handle overridden argumentnames/defaults?
//perhaps we should check argument names at least? and only use overriding for "hidden" default values?
//also, perhaps better to read the default values from the argument tags?
//ignore markup-provided arguments for now..
                switch(parentTag,
                    \instancemethods, {
                        c = currentClass;
                        ic = currentImplClass;
                        css = "imethodname";
                        pfx = "-";
                    },
                    \classmethods, {
                        c = currentClass !? {currentClass.class};
                        ic = currentImplClass !? {currentImplClass.class}; // else we get Nil for nil!
                        css = "cmethodname";
                        pfx = "*";
                    },
                    {
                        c = nil;
                        ic = nil;
                        css = "imethodname";
                        pfx = nil;
                    }
                );

                split = f[1][1].findRegexp("[^ ,]+");
                split.do {|r|
                    mname = r[1];
                    if(c.notNil) {
                        mstat = 0;
                        sym = mname.asSymbol;
                        //check for normal method or getter
                        m = ic !? {ic.findRespondingMethodFor(sym.asGetter)};
                        m = m ?? {c.findRespondingMethodFor(sym.asGetter)};
                        m !? {
                            mstat = mstat | 1;
                            args = this.class.makeArgString(m);
                        };
                        //check for setter
                        m2 = ic !? {ic.findRespondingMethodFor(sym.asSetter)};
                        m2 = m2 ?? {c.findRespondingMethodFor(sym.asSetter)};
                        m2 !? {
                            mstat = mstat | 2;
                            args = (m2.argNames ?? [nil,"value"])[1];
                        };
                    } {
                        m = nil;
                        m2 = nil;
                        mstat = 1;
                    };
                    file.write("<h3 class='"++css++"'>");
                    file.write("<span class='methprefix'>"++(pfx??"&nbsp;")++"</span>"++"<a name='"++(pfx??".")++mname++"' href='"++baseDir+/+"Overviews/Methods.html#"++this.escapeSpecialChars(mname)++"'>"++this.escapeSpecialChars(mname)++"</a>");

                    switch (mstat,
                        // getter only
                        1, { file.write(" "++args++"</h3>\n"); },
                        // setter only
                        2, { file.write(" = "++args++"</h3>\n"); },
                        // getter and setter
                        3, { file.write(" [= "++args++"]</h3>\n"); },
                        // method not found
                        0, { file.write(": METHOD NOT FOUND!</h3>\n");
                            warn("SCDocHTMLRenderer: Method not found:"+mname+"in doc for class"+c.name);
                        }
                    );
                    m = m ?? m2;
                    if(m.notNil) {
                        if(m.isExtensionOf(c) and: {ic.isNil or: {m.isExtensionOf(ic)}}) {
                            file.write("<div class='extmethod'>From extension in <a href='" ++ m.filenameSymbol ++ "'>" ++ m.filenameSymbol ++ "</a></div>\n");
                        } {
                            if(m.ownerClass == ic) {
                                file.write("<div class='supmethod'>From implementing class</div>\n");
                            } {
                                if(m.ownerClass != c) {
                                    n = m.ownerClass.name.asString.replace("Meta_","");
                                    file.write("<div class='supmethod'>From superclass: <a href='" ++ baseDir +/+ "Classes" +/+ n ++ ".html'>" ++ n ++ "</a></div>\n");
                                }
                            }
                        };
                    };
                };

                file.write("<div class='method'>");

                collectedArgs = [];
                do_children.();

                if(collectedArgs.notEmpty) {
                    file.write("<h4>Arguments:</h4>\n");
                    file.write("<table class='arguments'>\n");
                    collectedArgs.do {|a|
                        file.write("<tr><td class='argumentname'>"+a.text+"<td class='argumentdesc'>");
                        a.children.do {|e| this.renderHTMLSubTree(file,e,false) };
                    };
                    file.write("</table>");
                };

                n = parser.findNode(\returns, node.children);
                if(n.tag.notNil) {
                    file.write("<h4>Returns:</h4>\n<div class='returnvalue'>");
                    n.children.do {|e| this.renderHTMLSubTree(file,e,false) };
                    file.write("</div>");
                };

                n = parser.findNode(\discussion, node.children);
                if(n.tag.notNil) {
                    file.write("<h4>Discussion:</h4>\n");
                    n.children.do {|e| this.renderHTMLSubTree(file,e,false) };
                };
                file.write("</div>");
            },
            'argument', {
                collectedArgs = collectedArgs.add(node);
            },
            'returns', {
            },
            'description', {
                if(node.children.notEmpty) {
                    file.write("<h2><a class='anchor' name='description'>Description</a></h2>\n");
                    do_children.();
                };
            },
            'examples', {
                file.write("<h2><a class='anchor' name='examples'>Examples</a></h2>\n");
                do_children.();
            },
            'note', {
                file.write("<div class='note'><span class='notelabel'>NOTE:</span> ");
                do_children.();
                file.write("</div>");
            },
            'warning', {
                file.write("<div class='warning'><span class='warninglabel'>WARNING:</span> ");
                do_children.();
                file.write("</div>");
            },
            'emphasis', {
                file.write("<em>"++this.escapeSpecialChars(node.text)++"</em>");
            },
            'teletype', {
                if(node.display == \block, {
                    file.write("<pre>"++this.escapeSpecialChars(node.text)++"</pre>");
                }, {
                    file.write("<code>"++this.escapeSpecialChars(node.text)++"</code>");
                });
            },
            'strong', {
                file.write("<strong>"++this.escapeSpecialChars(node.text)++"</strong>");
            },
            'soft', {
                file.write("<span class='soft'>"++this.escapeSpecialChars(node.text)++"</span>");
            },
            'link', {
                file.write(this.htmlForLink(node.text));
            },
            'anchor', {
                file.write("<a class='anchor' name='"++node.text++"'>&nbsp;</a>");
            },
            'keyword', {
                file.write("<a class='anchor' name='kw_"++node.text++"'>&nbsp;</a>");
            },
            'code', {
                if(node.display == \block, {
                    file.write("<pre class='code prettyprint lang-sc'>"++this.autoLinkClasses(this.escapeSpecialChars(node.text))++"</pre>\n");
                }, {
                    file.write("<code class='code prettyprint lang-sc'>"++this.autoLinkClasses(this.escapeSpecialChars(node.text))++"</code>");
                });
            },
            'formula', {
                if(node.display == \block, {
                    file.write("<pre class='formula'>"++this.escapeSpecialChars(node.text)++"</pre>\n");
                }, {
                    file.write("<code class='formula'>"++this.escapeSpecialChars(node.text)++"</code>");
                });
            },
            'image', {
//                if(node.display == \block, {
                f = node.text.split($#);
                    file.write("<div class='image'><img src='"++f[0]++"'/>");
                    f[1] !? { file.write("<br><b>"++f[1]++"</b>") };
                    file.write("</div>\n");
//                }, {
//                    file.write("<span class='image'><img src='"++node.text++"'/></span>\n");
//                });
            },
            'list', {
                file.write("<ul>\n");
                do_children.(true);
                file.write("</ul>\n");
            },
            'tree', {
                file.write("<ul class='tree'>\n");
                do_children.(true);
                file.write("</ul>\n");
            },
            'definitionlist', {
                file.write("<dl>\n");
                do_children.(true);
                file.write("</dl>\n");
            },
            'numberedlist', {
                file.write("<ol>\n");
                do_children.(true);
                file.write("</ol>\n");
            },
            'table', {
                file.write("<table>\n");
                do_children.(true);
                file.write("</table>\n");
            },
            'footnote', {
                footNotes.add(node);
                file.write("<a class='footnote anchor' name='footnote_org_"++footNotes.size++"' href='#footnote_"++footNotes.size++"'><sup>"++footNotes.size++"</sup></a> ");
            },
            '##', {
                switch(parentTag,
                    'list',             { file.write("<li>") },
                    'tree',             { file.write("<li>") },
                    'numberedlist',     { file.write("<li>") },
                    'definitionlist',   { file.write("<dt>") },
                    'table',            { file.write("<tr><td>") }
                );
            },
            '||', {
                switch(parentTag,
                    'definitionlist',   { file.write("<dd>") },
                    'table',            { file.write("<td>") }
                );
            },

            'classtree', {
                file.write("<ul class='tree'>");
                this.renderClassTree(file,node.text.asSymbol.asClass);
                file.write("</ul>");
            },

            //these are handled explicitly
            'title', { },
            'summary', { },
            'class', { },
            'redirect', { },
            'related', { },
            'categories', { },
            'headerimage', { },

            'root', {
                do_children.();
            },

            { //unhandled tag
//                file.write("(TAG:"++node.tag++")");
                node.text !? {file.write(this.escapeSpecialChars(node.text))};
            }
        );
    }

    renderClassTree {|f,c|
        var name, doc, desc = "";
        name = c.name.asString;
        doc = SCDoc.docMap["Classes/"++name];
        doc !? { desc = " - "++doc.summary };
        if(c.name.isMetaClassName, {^this});
        f.write("<li> <a href='"++baseDir+/+"Classes/"++name++".html'>"++name++"</a>"++desc++"\n");

        c.subclasses !? {
            f.write("<ul class='tree'>\n");
            c.subclasses.copy.sort {|a,b| a.name < b.name}.do {|x|
                this.renderClassTree(f,x);
            };
            f.write("</ul>\n");
        };
    }

    renderTOC {|f,title|
        var parent = nil, pfx, r;
        var do_children = {|children|
            children !? {
                f.write("<ul class='toc'>");
                children.do {|n|
                    switch(n.tag,
                        \description, {
                            f.write("<li class='toc1'><a href='#description'>Description</a></li>\n");
                            do_children.(n.children);
                        },
                        \examples, {
                            f.write("<li class='toc1'><a href='#examples'>Examples</a></li>\n");
                            do_children.(n.children);
                        },
                        \classmethods, {
                            if(n.children.select{|x|x.tag!=\private}.notEmpty) {
                                f.write("<li class='toc1'><a href='#classmethods'>Class methods</a></li>\n");
                                parent = n.tag;
                                do_children.(n.children);
                                parent = nil;
                            };
                        },
                        \instancemethods, {
                            if(n.children.select{|x|x.tag!=\private}.notEmpty) {
                                f.write("<li class='toc1'><a href='#instancemethods'>Instance methods</a></li>\n");
                                parent = n.tag;
                                do_children.(n.children);
                                parent = nil;
                            };
                        },
                        \method, {
                            pfx = switch(parent,
                                \classmethods, {"*"},
                                \instancemethods, {"-"},
                                {"."});
                            f.write("<li class='toc3'>");
                            r = n.text.findRegexp("\\(.*\\)|[^ ,]+").flop[1].reject{|x|x[0]==$(};
                            f.write(r.collect{|m|
                                "<a href='#"++pfx++m++"'>"++this.escapeSpecialChars(m)++"</a> ";
                            }.join(" "));
                            f.write("</li>\n");
                        },
                        \section, {
                            f.write("<li class='toc1'><a href='#"++this.class.simplifyName(n.text)++"'>"++this.escapeSpecialChars(n.text)++"</a></li>\n");
                            do_children.(n.children);
                        },
                        \subsection, {
                            f.write("<li class='toc2'><a href='#"++this.class.simplifyName(n.text)++"'>"++this.escapeSpecialChars(n.text)++"</a></li>\n");
                            do_children.(n.children);
                        }
                    );
                };
                f.write("</ul>");
            }
        };
        f.write("<div id='toc'>\n");
//        f.write("<div id='toctitle'>Table of contents <a id='toc_toggle' href='#' onclick='showTOC(this);return false'></a></div>");
//        f.write("<div id='toctitle'>"++title++"</div><div id='toclabel'>table of contents:</div>");
        do_children.(parser.root);
        f.write("</div>");
    }

    renderHTMLHeader {|f,name,type,subtarget,toc=true|
        var x, cats, m, z;
        var folder = subtarget.dirname;
        var undocumented = false;
        f.write("<html><head><title>"++name++"</title>");
        f.write("<link rel='stylesheet' href='"++baseDir++"/scdoc.css' type='text/css' />");
        f.write("<link rel='stylesheet' href='"++baseDir++"/frontend.css' type='text/css' />");
        f.write("<link rel='stylesheet' href='"++baseDir++"/custom.css' type='text/css' />");
        f.write("<meta http-equiv='Content-Type' content='text/html; charset=UTF-8' />");
        f.write("<script src='" ++ baseDir ++ "/scdoc.js' type='text/javascript'></script>");
        f.write("<script src='" ++ baseDir ++ "/docmap.js' type='text/javascript'></script>");
        f.write("<script src='" ++ baseDir ++ "/prettify.js' type='text/javascript'></script>");
        f.write("<script src='" ++ baseDir ++ "/lang-sc.js' type='text/javascript'></script>");
        f.write("<script type='text/javascript'>var helpRoot='"++baseDir++"';</script>");
        f.write("</head>");

        f.write("<ul id='menubar'></ul>");

        f.write("<body onload='fixTOC();prettyPrint()'>");
        f.write("<div class='contents'>");
        if(folder==".",{folder=""});
        f.write("<div class='header'>");
        f.write("<div id='label'>SuperCollider "++folder.asString.toUpper);
        if(SCDoc.docMap[subtarget].installed==\extension) {
            f.write(" (extension)");
        };
        f.write("</div>");
        x = parser.findNode(\categories);
        if(x.text.notEmpty, {
            undocumented = x.text.find("Undocumented").notNil;
            f.write("<div id='categories'>");
            f.write(SCDoc.splitList(x.text).collect {|r|
                "<a href='"++baseDir +/+ "Browse.html#"++r++"'>"++r++"</a>"
            }.join(", "));
            f.write("</div>");
        });

        f.write("<h1>"++name);
        if((folder=="") and: {name=="Help"}, {
            f.write("<span class='headerimage'><img src='"++baseDir++"/images/SC_icon.png'/></span>");
        });
        f.write("</h1>");
        x = parser.findNode(\summary);
        f.write("<div id='summary'>"++this.escapeSpecialChars(x.text)++"</div>");
        f.write("</div>");

        f.write("<div class='subheader'>\n");

        if(type==\class) {
            if(currentClass.notNil) {
                m = currentClass.filenameSymbol.asString;
                f.write("<div id='filename'>Source: "++m.dirname++"/<a href='file://"++m++"'>"++m.basename++"</a></div>");
                if(currentClass != Object) {
                    f.write("<div id='superclasses'>");
                    f.write("Inherits from: ");
                    f.write(currentClass.superclasses.collect {|c|
                        "<a href=\"../Classes/"++c.name++".html\">"++c.name++"</a>"
                    }.join(" : "));
                    f.write("</div>");
                };
                if(currentClass.subclasses.notNil) {
                    f.write("<div id='subclasses'>");
                    f.write("Subclasses: ");
                    z = false;
                    f.write(currentClass.subclasses.collect(_.name).sort.collect {|c,i|
                        if(i==12,{z=true;"<span id='hiddensubclasses' style='display:none;'>"},{""})
                        ++"<a href=\"../Classes/"++c++".html\">"++c++"</a>"
                    }.join(", "));
                    if(z) {
                        f.write("</span><a class='subclass_toggle' href='#' onclick='javascript:showAllSubclasses(this); return false'>&hellip;&nbsp;see&nbsp;all</a>");
                    };
                    f.write("</div>");
                };
                if(currentImplClass.notNil) {
                    f.write("<div class='inheritance'>");
                    f.write("Implementing class: ");
                    f.write("<a href=\"../Classes/"++currentImplClass.name++".html\">"++currentImplClass.name++"</a>");
                    f.write("</div>");
                };
            } {
                f.write("<div id='filename'>Location: <b>NOT INSTALLED!</b></div>");
            };
        };

        x = parser.findNode(\related);
        if(x.text.notEmpty, {
            f.write("<div id='related'>");
            f.write("See also: ");
            f.write(SCDoc.splitList(x.text).collect {|r| this.htmlForLink(r)}.join(", "));
            f.write("</div>");
        });

        // FIXME: Remove this when conversion to new help system is done!
        if(undocumented) {
            x = Help.findHelpFile(name);
            x !? {
                f.write("[ <a href='" ++ baseDir ++ "/OldHelpWrapper.html#"
                ++x++"?"++SCDoc.helpTargetDir +/+ subtarget ++ ".html"
                ++"'>old help</a> ]")
            };
        };

        f.write("</div>");
        if(toc, {this.renderTOC(f,name)});
    }

    htmlForLink {|link|
        var n, m, f, c;
        #n, m, f = link.split($#); // link, anchor, label
        ^if ("^[a-zA-Z]+://.+".matchRegexp(link) or: (link.first==$/)) {
            if(f.size<1) {f=link};
            "<a href=\""++link++"\">"++this.escapeSpecialChars(f)++"</a>";
        } {
            #n, m, f = link.split($#); // link, anchor, label
            c = if(n.size>0) {baseDir+/+n++if("[^.]+\\.[a-zA-Z0-9]+$".matchRegexp(n),"",".html")} {""}; // url
            if(m.size>0) {c=c++"#"++m}; // add #anchor
            if(f.size<1) { // no label
                if(n.size>0) {
                    f = if(SCDoc.docMap[n].notNil)
                        {SCDoc.docMap[n].title} // use doc title
                        {n.basename}; // use filename
                    if(m.size>0) {
                        f = f++":"+m;
                    }
                } {
                    f = if(m.size>0) {m} {"(empty link)"};
                };
            };
            "<a href=\""++c++"\">"++this.escapeSpecialChars(f)++"</a>";
        };
    }

    addUndocumentedMethods {|class,tag,title|
        var node = parser.findNode(tag);
        var mets = parser.generateUndocumentedMethods(class, node, title);
        mets !? {
            if(node.tag.isNil, { //no subtree, create one
                parser.root = parser.root.add(node = (tag:tag, children:List.new));
            });
            node.children = node.children.add(mets);
        };
        ^node;
    }

    addInheritedMethods {|tag,div,title|
        var node = parser.findNode(tag);
        var mets = (tag:\subsection, text:title, children:[], makeDiv:div);
        if(node.tag.isNil, { //no subtree, create one
            parser.root = parser.root.add(node = (tag:tag, children:List.new));
        });
        node.children = node.children.add(mets);
        ^node;
    }

    render {|p, filename, subtarget, toc=true|
        var f,x,name,folder;

        parser = p;

        if(filename.endsWith(".html").not) {
            MethodError(this.class.name.asString+"will only render .html files",filename).throw;
        };
        f = File.open(filename, "w");

        //folder is the directory path of the file relative to the help tree,
        //like 'Classes' or 'Tutorials'.
        folder = subtarget.dirname;
        dirLevel = folder.split($/).reject {|y| (y.size<1) or: (y==".")}.size;
        baseDir = ".";
        dirLevel.do { baseDir = baseDir +/+ ".." };

        footNotes = List.new;

        x = parser.findNode(\class);
        if(x.text.notEmpty, {
            name = x.text.stripWhiteSpace;
            currentClass = name.asSymbol.asClass;
            currentImplClass = nil;

            currentClass !? {
                if(currentClass != Object) {
                    this.addInheritedMethods(\classmethods,"inheritedclassmets","Inherited class methods");
                    this.addInheritedMethods(\instancemethods,"inheritedinstmets","Inherited instance methods");
                };

                // FIXME: should probably get undocumented methods of implementing class?
                this.addUndocumentedMethods(currentClass.class,\classmethods,"Undocumented class methods");
                this.addUndocumentedMethods(currentClass,\instancemethods,"Undocumented instance methods");
                //TODO: add methods from +ClassName.schelp (recursive search)

                x = parser.findNode(\redirect).text.stripWhiteSpace;
                if(x.notEmpty) {
                    try {
                        currentImplClass = currentClass.perform(x.asSymbol);
                    };
                };
            };

            x = 10;
            parser.root.do {|n|
                switch(n.tag,
                    \description,       { n.sort = 0 },
                    \classmethods,      { n.sort = 3 },
                    \instancemethods,   { n.sort = 4 },
                    \examples,          { n.sort = 7 },
                    { n.sort = x = x + 1 }
                );
            };
            parser.root = parser.root.sort {|a,b| a.sort<b.sort};

            this.renderHTMLHeader(f,name,\class,subtarget,toc);
            this.renderHTMLSubTree(f,(tag:'root',children:parser.root));
        },{
            x = parser.findNode(\title);
            name = x.text.stripWhiteSpace;
            this.renderHTMLHeader(f,name,\other,subtarget,toc);
            this.renderHTMLSubTree(f,(tag:'root',children:parser.root));
        });

        this.renderFootNotes(f);

        parser.currentFile !? {
            f.write("<div class='doclink'>source: <a href='file://"++parser.currentFile++"'>"++parser.currentFile++"</a><br>");
            f.write("link::"++subtarget++"::</div>");
        };
        //f.write("<div class='version'>SuperCollider version "++Main.version++"</div>");
        f.write("</div></body></html>");
        f.close;
    }
}

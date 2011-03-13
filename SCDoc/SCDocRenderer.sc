SCDocRenderer {
    var <>parser;

    var currentClass;
    var currentImplClass;
    var collectedArgs;
//    var retValue;
    var dirLevel;
    var baseDir;
    var footNotes;

    *new {|p=nil|
        ^super.newCopyArgs(p);//.init;
    }

//    init {
//    }

    *simplifyName {|txt|
        ^txt.toLower.tr($\ ,$_);
    }

    escapeSpecialChars {|str|
//        ^str.replace("\"","&quot;").replace("&","&amp;").replace("<","&lt;").replace(">","&gt;");
        ^str.replace("&","&amp;").replace("<","&lt;").replace(">","&gt;");
    }

    renderFootNotes {|file|
        if (footNotes.notEmpty) {
            file.write("<div class='footnotes'>\n");
            footNotes.do {|n,i|
                file.write("<a name='footnote_"++(i+1)++"'/><div class='footnote'>");
                file.write("[<a href='#footnote_org_"++(i+1)++"'>"++(i+1)++"</a>] - ");
                n.children.do(this.renderHTMLSubTree(file,_));
                file.write("</div>");
            };
            file.write("</div>");
        };
    }

    autoLinkClasses {|txt|
        var x = "", z = 0;
        // FIXME: also match plural? like "Floats".. - if word was not class but ends in s and is a class without that s, then use that. 
        // it would be useful in prose, but not code examples..
        txt.findRegexp("\\b[A-Z][A-Za-z_0-9]+").select{|m| m[1].asSymbol.asClass.notNil}.do {|m|
            x = x ++ txt.copyRange(z,m[0]-1);
            x = x ++ "<a class='clslnk' href='"++baseDir+/+"Classes"+/+m[1]++".html'>" ++ m[1] ++ "</a>";
            z = m[0] + m[1].size;
        };
        ^(x ++ txt.copyToEnd(z));
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
                    file.write("<p>");
                });
                // FIXME: using autoLinkClasses here is often very nice, but a bit ugly when the word is not meant to be a class,
                // like "In this building lives a big cat" or "Control your mind", etc..
                // one workaround might be to don't match some words if they start a sentence??
//                file.write(this.autoLinkClasses(this.escapeSpecialChars(node.text)));
                file.write(this.escapeSpecialChars(node.text));
            },
            'section', {
                file.write("<a name='"++SCDocRenderer.simplifyName(node.text)++"'><h2>"++this.escapeSpecialChars(node.text)++"</h2></a>\n");
                do_children.();
            },
            'subsection', {
                file.write("<a name='"++SCDocRenderer.simplifyName(node.text)++"'><h3>"++this.escapeSpecialChars(node.text)++"</h3></a>\n");
                do_children.();
            },
            'classmethods', {
                if(node.children.select{|n|n.tag!=\private}.notEmpty) {
                    file.write("<a name='classmethods'><h2>Class Methods</h2></a>\n<div id='classmethods'>");
                    do_children.(true);
                    file.write("</div>");
                } {
                    do_children.(true);
                };
            },
            'instancemethods', {
                if(node.children.select{|n|n.tag!=\private}.notEmpty) {
                    file.write("<a name='instancemethods'><h2>Instance Methods</h2></a>\n<div id='instancemethods'>");
                    do_children.(true);
                    file.write("</div>");
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
                        c = currentClass.class;
                        ic = currentImplClass !? {currentImplClass.class}; // else we get Nil for nil!
                        css = "cmethodname";
                        pfx = "*";
                    },
                    {
                        c = nil;
                        ic = nil;
                        css = "imethodname";
                        pfx = "";
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
                            args = SCDoc.makeArgString(m);
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
                    file.write("<a name='"++pfx++mname++"'><h3 class='"++css++"'><span class='methprefix'>"++pfx++"</span>"++this.escapeSpecialChars(mname));

                    switch (mstat,
                        // getter only
                        1, { file.write(" "++args++"</h3></a>\n"); },
                        // setter only
                        2, { file.write(" = "++args++"</h3></a>\n"); },
                        // getter and setter
                        3, { file.write(" [= "++args++"]</h3></a>\n"); },
                        // method not found
                        0, { file.write(": METHOD NOT FOUND!</h3></a>\n"); }
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
                    file.write("<h4>Parameters:</h4>\n");
                    file.write("<table class='arguments'>\n");
                    collectedArgs.do {|a|
                        file.write("<tr><td class='argumentname'>"+a.text+"<td class='argumentdesc'>");
                        a.children.do {|e| this.renderHTMLSubTree(file,e,false) };
                    };
                    file.write("</table>");
                };

                n = this.parser.findNode(\returns, node.children);
                if(n.tag.notNil) {
                    file.write("<h4>Returns:</h4>\n<div class='returnvalue'>");
                    n.children.do {|e| this.renderHTMLSubTree(file,e,false) };
                    file.write("</div>");
                };

                n = this.parser.findNode(\discussion, node.children);
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
                    file.write("<a name='description'><h2>Description</h2></a>\n<div id='description'>");
                    do_children.();
                    file.write("</div>");
                };
            },
            'examples', {
                file.write("<a name='examples'><h2>Examples</h2></a>\n<div id='examples'>");
                do_children.();
                file.write("</div>");
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
            'strong', {
                file.write("<strong>"++this.escapeSpecialChars(node.text)++"</strong>");
            },
            'soft', {
                file.write("<span class='soft'>"++this.escapeSpecialChars(node.text)++"</span>");
            },
            'link', {
                if("^[a-zA-Z]+://.+".matchRegexp(node.text) or: (node.text.first==$/),{
                    file.write("<a href=\""++node.text++"\">"++this.escapeSpecialChars(node.text)++"</a>");
                },{
                    f = node.text.split($#);
                    m = if(f[1].size>0, {"#"++f[1]}, {""});
                    n = f[2] ?? { f[0].split($/).last };
                    c = if(f[0].size>0, {baseDir +/+ f[0]++".html"},{n=f[2]??f[1];""});
                    file.write("<a href=\""++c++m++"\">"++this.escapeSpecialChars(n)++"</a>");
                });
            },
            'anchor', {
                file.write("<a name='"++node.text++"'</a>");
            },
            'code', {
                if(node.display == \block, {
                    file.write("<pre class='code prettyprint lang-sc'>"++this.autoLinkClasses(this.escapeSpecialChars(node.text))++"</pre>\n");
                }, {
                    file.write("<code class='code prettyprint lang-sc'>"++this.autoLinkClasses(this.escapeSpecialChars(node.text))++"</code>\n");
                });
            },
            'formula', {
                if(node.display == \block, {
                    file.write("<pre class='formula'>"++this.escapeSpecialChars(node.text)++"</pre>\n");
                }, {
                    file.write("<code class='formula'>"++this.escapeSpecialChars(node.text)++"</code>\n");
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
                file.write("<a class='footnote' name='footnote_org_"++footNotes.size++"' href='#footnote_"++footNotes.size++"'><sup>"++footNotes.size++"</sup></a> ");
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

    renderTOC {|f|
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
                            };
                        },
                        \instancemethods, {
                            if(n.children.select{|x|x.tag!=\private}.notEmpty) {
                                f.write("<li class='toc1'><a href='#instancemethods'>Instance methods</a></li>\n");
                                parent = n.tag;
                                do_children.(n.children);
                            };
                        },
                        \method, {
                            pfx = if(parent==\classmethods,"*","-");
                            f.write("<li class='toc3'>");
                            r = n.text.findRegexp(" *([^(]+) *(\\(.*\\))?");
                            f.write(r[1][1].findRegexp("[^ ,]+").flop[1].collect {|m|
                                "<a href='#"++pfx++m++"'>"++m++"</a>";
                            }.join(", "));
                            f.write("</li>\n");
                        },
                        \section, {
                            f.write("<li class='toc1'><a href='#"++SCDocRenderer.simplifyName(n.text)++"'>"++n.text++"</a></li>\n");
                            do_children.(n.children);
                        },
                        \subsection, {
                            f.write("<li class='toc2'><a href='#"++SCDocRenderer.simplifyName(n.text)++"'>"++n.text++"</a></li>\n");
                            do_children.(n.children);
                        }
                    );
                };
                f.write("</ul>");
            }
        };
        f.write("<div id='toctitle'>Table of contents <a id='toc_toggle' href='#' onclick='showTOC(this);return false'></a></div>");
        f.write("<div id='toc'>\n");
        do_children.(parser.root);
        f.write("</div>");
    }

    renderHTMLHeader {|f,name,type,folder,toc=true|
        var x, cats, m, z;
        f.write("<html><head><title>"++name++"</title><link rel='stylesheet' href='"++baseDir++"/scdoc.css' type='text/css' />");
        f.write("<meta http-equiv='Content-Type' content='text/html; charset=UTF-8' />");
        f.write("<script src='" ++ baseDir ++ "/scdoc.js' type='text/javascript'></script>");
        f.write("<script src='" ++ baseDir ++ "/prettify.js' type='text/javascript'></script>");
        f.write("<script src='" ++ baseDir ++ "/lang-sc.js' type='text/javascript'></script>");

        f.write("</head><body onload='fixTOC();prettyPrint()'>");
        
        f.write(
            "<table class='headMenu'><tr>"
            "<td><a href='" ++ baseDir +/+ "Help.html'>Home</a>"
            "<td><a href='" ++ baseDir +/+ "Browse.html'>Browse</a>"
            "<td><a href='" ++ baseDir +/+ "Overviews/Documents.html'>Document index</a>"
            "<td><a href='" ++ baseDir +/+ "Overviews/Classes.html'>Class index</a>"
            "<td><a href='" ++ baseDir +/+ "Overviews/Methods.html'>Method index</a>"
            "<td><a href='" ++ baseDir +/+ "Search.html'>Search</a>"
            "</table>"
        );

//        cats = SCDoc.splitList(parser.findNode(\categories).text);
//        cats = if(cats.notNil, {cats.join(", ")}, {""});
        if(folder==".",{folder=""});
        f.write("<div class='header'>");
//        f.write("<div id='label'><a href='"++baseDir+/+"Help.html"++"'>SuperCollider</a> "++folder.asString.toUpper++"</div>");
        f.write("<div id='label'>SuperCollider "++folder.asString.toUpper);
        if(type==\class and: currentClass.notNil) {
            if(currentClass.filenameSymbol.asString.beginsWith(thisProcess.platform.classLibraryDir).not,
                    {f.write(" (extension)")});
        };
        f.write("</div>");
        x = parser.findNode(\categories);
        if(x.text.notEmpty, {
            f.write("<div id='categories'>");
//            f.write("Categories: ");
            f.write(SCDoc.splitList(x.text).collect {|r|
//                "<a href='"++baseDir +/+ "Overviews/Categories.html#"++SCDocRenderer.simplifyName(r).split($>).first++"'>"++r++"</a>"
                "<a href='"++baseDir +/+ "Browse.html#"++r++"'>"++r++"</a>"
            }.join(", "));
            f.write("</div>");
        });    

        f.write("<h1>"++name);
//        x = parser.findNode(\headerimage);
//        if(x.text.notEmpty, { f.write("<span class='headerimage'><img src='"++x.text++"'/></span>")});
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
                f.write("<div id='filename'>Location: "++m.dirname++"/<a href='file://"++m++"'>"++m.basename++"</a></div>");
                if(currentClass != Object) {
                    f.write("<div class='inheritance'>");
                    f.write("Inherits from: ");
                    f.write(currentClass.superclasses.collect {|c|
                        "<a href=\"../Classes/"++c.name++".html\">"++c.name++"</a>"
                    }.join(" : "));
                    f.write("</div>");
                };
                if(currentClass.subclasses.notNil) {
                    f.write("<div class='inheritance'>");
                    f.write("Subclasses: ");
                    f.write(currentClass.subclasses.collect {|c|
                        "<a href=\"../Classes/"++c.name++".html\">"++c.name++"</a>"
                    }.join(", "));
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
            f.write(SCDoc.splitList(x.text).collect {|r|
                z = r.split($#);
                m = if(z[1].size>0, {"#"++z[1]}, {""});
                "<a href=\""++baseDir +/+ z[0]++".html"++m++"\">"++r.split($/).last++"</a>"
            }.join(", "));
            f.write("</div>");
        });

        // FIXME: Remove this when conversion to new help system is done!
        if((type==\class) and: {currentClass.notNil}, {
            f.write("[ <a href='"++Help.findHelpFile(currentClass.name.asString)++"'>old help</a> ]");
        });

        f.write("</div>");
        
        if(toc, {this.renderTOC(f)});
    }

    addUndocumentedMethods {|class,tag|
        var node = parser.findNode(tag);
        var mets = parser.generateUndocumentedMethods(class, node,
            "Undocumented "++if(tag==\classmethods,"class methods","instance methods"));
        mets !? {
            if(node.tag.isNil, { //no subtree, create one
                parser.root.add(node = (tag:tag, children:List.new));
            });
            node.children.add(mets);
        };
        ^node;
    }

    renderHTML {|filename, folder=".", toc=true|
        var f,x,name;
        
        SCDoc.postProgress("Rendering "++filename);

        ("mkdir -p"+filename.dirname.escapeChar($ )).systemCmd;

        f = File.open(filename, "w");

        //folder is the directory path of the file relative to the help tree,
        //like 'Classes' or 'Tutorials'.
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
                this.addUndocumentedMethods(currentClass.class,\classmethods);
                this.addUndocumentedMethods(currentClass,\instancemethods);
                //TODO: add methods from +ClassName.schelp (recursive search)
                
                x = parser.findNode(\redirect).text.stripWhiteSpace;
                if(x.notEmpty) {
                    currentImplClass = currentClass.perform(x.asSymbol);
                };
            };

            x = 10;
            parser.root.do {|n|
                switch(n.tag,
                    \description,       { n.sort = 0 },
                    \classmethods,      { n.sort = 1 },
                    \instancemethods,   { n.sort = 2 },
                    \examples,          { n.sort = 3 },
                    { n.sort = x = x + 1 }
                );
            };
            parser.root.sort {|a,b| a.sort<b.sort};

            this.renderHTMLHeader(f,name,\class,folder,toc);
            this.renderHTMLSubTree(f,(tag:'root',children:parser.root));
        },{
            x = parser.findNode(\title);
            name = x.text.stripWhiteSpace;
            this.renderHTMLHeader(f,name,\other,folder,toc);
            this.renderHTMLSubTree(f,(tag:'root',children:parser.root));
        });

        this.renderFootNotes(f);

        f.write("<div class='version'>SuperCollider version "++Main.version++"</div>");
        f.write("</body></html>");
        f.close;
    }
}


SCDoc {
    classvar <helpTargetDir;
    classvar <helpSourceDir;
    classvar <categoryMap;
    classvar <docMap;
    classvar <p, <r;
    classvar doWait;
    classvar progressText = nil, progressWindow = nil;
    classvar progressCount = 0, progressMax = 1;

    *helpSourceDir_ {|path|
        helpSourceDir = path.standardizePath;
    }

    *helpTargetDir_ {|path|
        helpTargetDir = path.standardizePath;
    }
    
    *postProgress {|string|
        var prg = (progressCount/progressMax*100).round(0.1).asString ++ "%";
        if(progressText.notNil) {
            progressText.string = "Progress: "++prg++"\n\n"++string;
        } {
            (prg+string).postln;
        };
        if(doWait, {0.wait});
    }

    *docMapToJSON {|path|
        var f = File.open(path,"w");

        if(f.isNil, {^nil});
        
        f.write("docmap = [\n");
        docMap.pairsDo {|k,v|
            f.write("{\n");
            v.pairsDo {|k2,v2|
                v2=v2.asString.replace("'","\\'");
                f.write("'"++k2++"': '"++v2++"',\n");
            };

            f.write("},\n");
        };
        f.write("]\n");
        f.close;
    }

    *splitList {|txt|
//        ^txt.findRegexp("[-_>#a-zA-Z0-9]+[-_>#/a-zA-Z0-9 ]*[-_>#/a-zA-Z0-9]+").flop[1];
        ^txt.findRegexp("[^, ]+[^,]*[^, ]+").flop[1];
    }

    *initClass {
        helpTargetDir = thisProcess.platform.userAppSupportDir +/+ "/Help";
        helpSourceDir = thisProcess.platform.systemAppSupportDir +/+ "/HelpSource";
        r = SCDocRenderer.new;
        r.parser = p = SCDocParser.new;
        doWait = false;
    }

    *collectAllMethods {
        var name, t;
        t = IdentityDictionary.new;

        Class.allClasses.do {|c|
            name = c.name.asString;
            c.methods.do {|x|
                if(t[x.name]==nil, {t[x.name] = List.new});

                t[x.name].add([name,x.isExtensionOf(c)]);
            };
        };

        ^t;
    }
    *makeOverviews {
        var mets, f, n;
        
        SCDoc.postProgress("Generating Class tree...");
        p.overviewClassTree;
        r.renderHTML(helpTargetDir +/+ "Overviews/ClassTree.html","Overviews",false);
        progressCount = progressCount + 1;

        SCDoc.postProgress("Generating Class index...");
        p.overviewAllClasses(docMap);
        r.renderHTML(helpTargetDir +/+ "Overviews/Classes.html","Overviews",false);
        progressCount = progressCount + 1;

        SCDoc.postProgress("Generating Methods index...");
        mets = this.collectAllMethods;
//        mets = p.overviewAllMethods(docMap);
//        r.renderHTML(helpTargetDir +/+ "Overviews/Methods.html","Overviews",false);
        progressCount = progressCount + 1;

        SCDoc.postProgress("Writing Methods JSON index...");
        f = File.open(SCDoc.helpTargetDir +/+ "methods.js","w");
        f.write("methods = [\n");
        mets.pairsDo {|k,v|
            f.write("['"++k++"',[");
            v.do {|c,i|
                n = c[0];
//                f.write("'"++n++"',");
                f.write("'"++n);
                if(c[1]) { //is extension of
                    f.write("+");
                };
                f.write("',");
            };
            f.write("]],\n");
        };
        f.write("\n];");
        f.close;
        progressCount = progressCount + 1;
        
        SCDoc.postProgress("Generating Documents index...");
        p.overviewAllDocuments(docMap);
        r.renderHTML(helpTargetDir +/+ "Overviews/Documents.html","Overviews", false);
        progressCount = progressCount + 1;

//        SCDoc.postProgress("Generating Categories overview...");
//        p.overviewCategories(categoryMap);
//        r.renderHTML(helpTargetDir +/+ "Overviews/Categories.html","Overviews", true);
        progressCount = progressCount + 1;
        
    }

    *makeMethodList {|c,n,tag|
        var l, mets, name, syms;

        (mets = c.methods) !? {
            n.add((tag:tag, children:l=List.new));
            syms = mets.collectAs(_.name,IdentitySet);
            mets.do {|m| //need to iterate over mets to keep the order
                name = m.name;
                if (name.isSetter.not or: {syms.includes(name.asGetter).not}) {
                    l.add((tag:\method, text:name.asString));
                };
            };
        };
    }

    *classHasArKrIr {|c|
        ^#[\ar,\kr,\ir].collect {|m| c.class.findRespondingMethodFor(m).notNil }.reduce {|a,b| a or: b};
    }
    
    *makeArgString {|m|
        var res = "";
        var value;
        m.argNames.do {|a,i|
            if (i>0) { //skip 'this' (first arg)
                if (i>1) { res = res ++ ", " };
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
        };
        if (res.notEmpty) {
            ^("("++res++")");
        } {
            ^"";
        };
    }

    *handleUndocumentedClasses {|force=false|
        var n, m, name, cats, dest;
//        var src, dest;
//        var srcbase = helpSourceDir +/+ "Classes";
        var destbase = helpTargetDir +/+ "Classes";
        SCDoc.postProgress("Checking for undocumented classes...");
        Class.allClasses.do {|c|
            name = c.name.asString;
//            src = srcbase +/+ name ++ ".schelp";
            dest = destbase +/+ name ++ ".html";
//            if(File.exists(src).not and: {name.find("Meta_")!=0}, {
            if((File.exists(dest).not or: {docMap["Classes" +/+ name].isNil}) and: {name.find("Meta_")!=0}, { //this was actually slower!
            //FIXME: doesn't work quite right in case one removes the src file, then it's still in docMap cache..
                n = List.new;
                n.add((tag:\class, text:name));
                n.add((tag:\summary, text:""));

                cats = "Undocumented classes";
                if(this.classHasArKrIr(c), {
                    cats = cats ++ ", UGens>Undocumented";
                    if(c.categories.notNil) {
                        cats = cats ++ ", "++c.categories.join(", ");
                    };
                });
                n.add((tag:\categories, text:cats));

                p.root = n;
                this.addToDocMap(p, "Classes" +/+ name);
                
                if((force or: File.exists(dest).not), {
                    SCDoc.postProgress("Generating doc for class: "++name);
                    n.add((tag:\description, children:m=List.new));
                    m.add((tag:\prose, text:"This class is missing documentation. Please create and edit HelpSource/Classes/"++name++".schelp", display:\block));

                    // FIXME: Remove this when conversion to new help system is done!
                    m.add((tag:\prose, text:"Old help file: ", display:\block));
                    m.add((tag:\link, text:Help.findHelpFile(c.name.asString) ?? "not found", display:\inline));

                    this.makeMethodList(c.class,n,\classmethods);
                    this.makeMethodList(c,n,\instancemethods);
                    r.renderHTML(dest,"Classes");
                });
            });
            n = docMap["Classes" +/+ name];
            n !? {n.delete = false};
        };
    }
    
    *classHash {|c|
        var hash;
        //Problems: probably very slow! Also it doesn't work if one of the superclasses changed..
        hash = (((c.methods ++ c.class.methods) ?? []).collect {|m| m.code}).flatten.asString.hash;
        ^hash;
    }

    *addToDocMap {|parser, path|
        var x = parser.findNode(\class).text;
        var doc = (
            path:path,
            summary:parser.findNode(\summary).text,
            categories:parser.findNode(\categories).text
        );

        doc.title = if(x.notEmpty,x,{parser.findNode(\title).text});

//        if(x.notEmpty) {
//            doc.hashnum = this.classHash(x.stripWhiteSpace.asSymbol.asClass);
//        };
        
        docMap[path] = doc;
    }

    *makeCategoryMap {
        var cats, c;
        SCDoc.postProgress("Creating category map...");
        categoryMap = Dictionary.new;
        docMap.pairsDo {|k,v|
            cats = SCDoc.splitList(v.categories);
            cats = cats ? ["Uncategorized"];
            cats.do {|cat|
                if (categoryMap[cat].isNil) {
                    categoryMap[cat] = List.new;
                };
                categoryMap[cat].add(v);
            };

            // check if class is standard, extension or missing
            if(v.path.dirname=="Classes") {
                c = v.path.basename.asSymbol.asClass;
                v.installed = if(c.notNil) {
                    if(c.filenameSymbol.asString.beginsWith(thisProcess.platform.classLibraryDir).not)
                        {\extension}
                        {\standard}
                } {\missing};
            };
        };

    }

    *readDocMap {
        var path = helpTargetDir +/+ "scdoc_cache";
        SCDoc.postProgress("Reading docMap cache...");
        docMap = Object.readArchive(path);

        if(docMap.isNil) {
            docMap = Dictionary.new;
            SCDoc.postProgress("Not found, Creating new docMap cache");
            ^true;
        };
        ^false;
    }

    *writeDocMap {
        var path = helpTargetDir +/+ "scdoc_cache";
        SCDoc.postProgress("Writing docMap cache");
        docMap.writeArchive(path);
    }

    *updateFile {|source, rootDir, force=false|
        var lastDot = source.findBackwards(".");
        var subtarget = source.copyRange(rootDir.size + 1, lastDot - 1);
        var target = helpTargetDir +/+ subtarget ++".html";
        var folder = target.dirname;
        var ext = source.copyToEnd(lastDot);
        var didUpdate = false;
        if(source.beginsWith(rootDir).not) {
            SCDoc.postProgress("File location error:\n"++source++"\nis not inside "++rootDir);
            ^nil;
        };
        if(ext == ".schelp", {
            //update only if needed
            if(force or: {docMap[subtarget].isNil} or: {("test"+source.escapeChar($ )+"-ot"+target.escapeChar($ )).systemCmd!=0}
//                or: {subtarget.dirname == "Classes" and: {this.classHash(subtarget.basename.asSymbol.asClass) != docMap[subtarget].hashnum}}
                , { 
                SCDoc.postProgress("Parsing "++source);
                p.parseFile(source);
                this.addToDocMap(p,subtarget);
                r.renderHTML(target,subtarget.dirname);
                didUpdate = true;
            });
            docMap[subtarget].delete = false;
        }, {
            //fixme: copy only if file is newer (problem: different args on different platforms)
            if(("test"+source.escapeChar($ )+"-ot"+(folder +/+ source.basename).escapeChar($ )).systemCmd!=0, {
                SCDoc.postProgress("Copying" + source + "to" + (folder +/+ source.basename));
                ("mkdir -p"+folder.escapeChar($ )).systemCmd;
                ("cp" + source.escapeChar($ ) + folder.escapeChar($ )).systemCmd;
                didUpdate = true;
            });
        });
        ^didUpdate;
    }
    
    *makeProgressWindow {
        var a, b;
        if(GUI.scheme.isNil and: doWait, {^nil});
        
        progressWindow = Window("Documentation update",500@200).alwaysOnTop_(true).userCanClose_(false).layout_(QVLayout.new);

        StaticText(progressWindow).string_("Please wait while updating help files...\n");
        progressText = TextView(progressWindow).editable_(false);

        progressWindow.front;
    }

    *updateAll {|force=false,doneFunc=nil,threaded=true,gui=true|
        // FIXME: split updateFile() so that it first collects all files that needs updating, then we run it on that list.
        // then we can have progress of files that needs update, not for all files.
        var fileList = List.new;
        var processFiles = {
            SCDoc.postProgress("Processing helpfiles...");
            fileList.do {|entry|
                if(this.updateFile(entry[0], entry[1], force)) {
                    progressCount = progressCount + 1;
                } {
                    progressMax = progressMax - 1;
                }
            };
        };
        var recurseHelpSource = {|dir|
//            SCDoc.postProgress("Parsing all in "++dir);
            SCDoc.postProgress("Collecting helpfiles in "++dir);
            PathName(dir).filesDo {|path|
//                this.updateFile(path.fullPath, dir, force);
                fileList.add([path.fullPath, dir]);
            };
        };
        var findExtHelp = {|p|
            p.folders.do {|p2|
                if(p2.fileName=="HelpSource") {
                    recurseHelpSource.(p2.fullPath);
                } {
                    findExtHelp.(p2);
                }
            }
        };

        var f = {
            if(force.not, {
                force = this.readDocMap;
            }, {
                docMap = Dictionary.new;
            });
            
            docMap.do{|e|
                e.delete = true;
            };

            recurseHelpSource.(helpSourceDir);
            findExtHelp.(PathName(thisProcess.platform.userExtensionDir));
            findExtHelp.(PathName(thisProcess.platform.systemExtensionDir));
            progressMax = fileList.size + 4 + 6;
            progressCount = 0;
            processFiles.value;

            this.handleUndocumentedClasses(force);
            progressCount = progressCount + 1;
            docMap.pairsDo{|k,e|
                if(e.delete==true, {
                    SCDoc.postProgress("Deleting "++e.path);
                    docMap.removeAt(k);
                    //TODO: we should also remove the rendered dest file if existent? or maybe too dangerous..
                });
                e.removeAt(\delete); //remove the key since we don't need it anymore
            };
            this.writeDocMap;
            progressCount = progressCount + 1;
            this.makeCategoryMap;
            progressCount = progressCount + 1;
            this.makeOverviews;
            this.postProgress("Writing Document JSON index...");
            this.docMapToJSON(this.helpTargetDir +/+ "docmap.js");
            progressCount = progressCount + 1;
            
            "SCDoc done!".postln;
            doneFunc.value();
            doWait=false;
            progressWindow !? { progressWindow.userCanClose = true };
        };
        if(gui, {this.makeProgressWindow});
        if(doWait = threaded, {
            Routine(f).play(AppClock);
        }, f);
    }

    *checkBrokenLinks {
        var f,m;
        var file;
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
        
        PathName(helpSourceDir).filesDo {|path|
            var source = path.fullPath;
            var lastDot = source.findBackwards(".");
//            var subtarget = source.copyRange(helpSourceDir.size+1,lastDot-1);
//            var target = helpTargetDir +/+ subtarget ++".html";
            var ext = source.copyToEnd(lastDot);
//            var lev;
            if(ext == ".schelp", {
                file = source;
                p.parseFile(source);
//                lev = target.dirname.split($/).reject {|y| (y.size<1) or: (y==".")}.size;
//                baseDir = ".";
//                lev.do { baseDir = baseDir +/+ ".." };
                do_children.(p.root);
            });
        };
        postln("Done");
    }
    
    *findClassOrMethod {|str|
        ^ SCDoc.helpTargetDir +/+ if(str[0].isUpper,
            {"Classes" +/+ str ++ ".html"},
            {"Overviews/Methods.html#" ++ str}
        );
    }
}

+ String {
    stripWhiteSpace {
        var a=0, b=this.size-1;
        //FIXME: how does this handle strings that are empty or single characters?
        while({(this[a]==$\n) or: (this[a]==$\ ) or: (this[a]==$\t)},{a=a+1});
        while({(this[b]==$\n) or: (this[b]==$\ ) or: (this[b]==$\t)},{b=b-1});
        ^this.copyRange(a,b);
    }
}

+ Method {
    isExtensionOf {|class|
        ^(
            (this.filenameSymbol != class.filenameSymbol)
            and:
                if((class!=Object) and: (class!=Meta_Object),
                    {class.superclasses.includes(this.ownerClass).not},
                    {true})
        );
    }
}

SCDoc {
    // Increment this whenever we make a change to the SCDoc system so that all help-files should be processed again
    classvar version = 22;

    classvar <helpTargetDir;
    classvar <helpSourceDir;
    classvar helpSourceDirs;
    classvar doc_map = nil;
    classvar <p, <r;
    classvar <undocumentedClasses;
    classvar <>verbose = false;
    classvar doWait;
    classvar progressCount = 0, progressMax = 0;
    classvar new_classes = nil;
    classvar didRun = false;
    classvar isProcessing = false;
    classvar lastUITick = 0;

    *helpSourceDir_ {|path|
        helpSourceDir = path.standardizePath;
    }

    *helpSourceDirs {
        this.findHelpSourceDirs;
        ^helpSourceDirs;
    }

    *helpTargetDir_ {|path|
        if(path!=helpTargetDir) {didRun = false};
        helpTargetDir = path.standardizePath;
    }

    *postProgress {|string|
        var prg = "";
        if(progressMax>0) {prg = (progressCount/progressMax*100).round(0.1).asString ++ "% "};
        if(verbose) {("SCDoc:"+prg++string).postln};
        this.maybeWait;
    }

    *docMapToJSON {|path|
        var f;

        File.delete(path);
        f = File.open(path,"w");

        f.write("docmap = {\n");
        doc_map.pairsDo {|k,v|
            f.write("'"++k++"':"+"{\n");
            v.pairsDo {|k2,v2|
                if(v2.isKindOf(Array)) {
                    v2 = "["+v2.collect{|x|"'"++x++"'"}.join(",")+"]";
                } {
                    v2 = "'"++v2.asString.replace("'","\\'")++"'";
                };
                f.write("'"++k2++"': "++v2++",\n");
            };

            f.write("},\n");
        };
        f.write("}\n");
        f.close;
    }

    *splitList {|txt|
        ^txt.findRegexp("[^, ]+[^,]*[^, ]+").flop[1];
    }

    *initClass {
        this.helpSourceDir_(thisProcess.platform.classLibraryDir.dirname +/+ "HelpSource");
        this.helpTargetDir_(thisProcess.platform.userAppSupportDir +/+ "Help");
        r = SCDocHTMLRenderer.new;
        p = SCDocParser.new;
        doWait = false;
    }

    *addMethodList {|c,n,tag|
        var l, x = this.makeMethodList(c);
        if(x.notEmpty) {
            l = x.collect{|m| (tag:\method, text:m)};
            n = n.add((tag:tag, children:l));
        };
        ^n;
    }

    *makeMethodList {|c|
        var l, mets, name, syms;
        l = Array.new;
        (mets = c.methods) !? {
            syms = mets.collectAs(_.name,IdentitySet);
            mets.do {|m| //need to iterate over mets to keep the order
                name = m.name;
                if (name.isSetter.not or: {syms.includes(name.asGetter).not}) {
                    l = l.add(name.asGetter.asString);
                };
            };
        };
        ^l;
    }

    *classHasArKrIr {|c|
        ^#[\ar,\kr,\ir].collect {|m| c.class.findRespondingMethodFor(m).notNil }.reduce {|a,b| a or: b};
    }

    *addToDocMap {|parser, path|
        var folder = path.dirname, classname = parser.findNode(\class).text;
        var doc = (
            path:path,
            summary:parser.findNode(\summary).text,
            categories:parser.findNode(\categories).text
        );

        doc.title = if(classname.notEmpty,classname,{parser.findNode(\title).text});

        if(doc.title.isEmpty) {
            doc.title = "NO TITLE:"+path;
            warn("Document at"+path+"has no title:: or class::");
        };
        if(doc.summary.isEmpty) {
            warn("Document at"+path+"has no summary::");
        };
        if(doc.categories.isEmpty) {
            warn("Document at"+path+"has no categories::");
        };

        if(classname.notEmpty) {
            if(path.basename != doc.title) {
                warn("Document at"+path+"is not named according to class name:"+doc.title);
            };
            if(folder != "Classes") {
                warn("Document at"+path+"is a class doc but is not in Classes/ folder");
            };
        } {
            if(folder == "Classes") {
                warn("Document at"+path+"is not a class doc but is in Classes/ folder");
            };
        };

        doc_map[path] = doc;
    }

    *makeCategoryMap {
        var cats, c, map;
        this.postProgress("Creating category map...");
        map = Dictionary.new;
        this.docMap.pairsDo {|k,v|
            cats = this.splitList(v.categories);
            cats = cats ? ["Uncategorized"];
            cats.do {|cat|
                if (map[cat].isNil) {
                    map[cat] = List.new;
                };
                map[cat].add(v);
            };

        };
        ^map;
    }

    *checkVersion {
        var f, path = this.helpTargetDir +/+ "version";
        if(path.load != version) {
            // FIXME: should we call this.syncNonHelpFiles here to ensure that helpTargetDir exists?
            this.postProgress("version update, refreshing timestamp");
            // this will update the mtime of the version file, triggering re-rendering and clean doc_map
            f = File.open(path,"w");
            f.write(version.asCompileString);
            f.close;
            ^true;
        };
        ^false;
    }

    *readDocMap {
        var path = this.helpTargetDir +/+ "scdoc_cache";
        var verpath = this.helpTargetDir +/+ "version";

        if(this.checkVersion or: {("test"+verpath.escapeChar($ )+"-nt"+path.escapeChar($ )).systemCmd==0}) {
            this.postProgress("not reading scdoc_cache due to version timestamp update");
            doc_map = nil;
        } {
            doc_map = path.load;
        };

        if(doc_map.isNil) {
            doc_map = Dictionary.new;
            ^true;
        };
        ^false;
    }

    *writeDocMap {
        var f, path = this.helpTargetDir +/+ "scdoc_cache";
        File.delete(path);
        f = File.open(path,"w");
        f.write(doc_map.asCompileString);
        f.close;
    }

    *docMap {
        if(doc_map.isNil) {
            this.getAllMetaData;
        };
        ^doc_map;
    }

    *tickProgress { progressCount = progressCount + 1 }

    *maybeWait {
        var t;
        if(doWait and: {(t = Main.elapsedTime)-lastUITick > 0.1}) {
            0.wait;
            lastUITick = t;
        }
    }

    *parseAndRender {|src,dest,subtarget|
        var p2;
        SCDoc.postProgress(src+"->"+dest);
        p.parseFile(src);

        // hackish workaround: if a doc addition is removed, it is not detected
        // and removed from the 'additions' set in the docmap entry.
        // so here we check that the file exists..
        doc_map[subtarget].additions.select(File.exists(_)).do {|ext|
            p2 = p2 ?? {p.class.new};
            p2.parseFile(ext);
            p.merge(p2);
        };

        r.render(p,dest,subtarget);
    }

    *renderAll {|force=false,threaded=true,findExtensions=true,doneFunc|
        var count, count2, func, x, fileList, subtarget, dest, t = Main.elapsedTime, oldVerbose = verbose;

        func = {
            progressMax = 100;
            progressCount = 0;
            verbose = true;

            this.cleanState(force);
            if(findExtensions) {
                this.findHelpSourceDirs;
            } {
                helpSourceDirs = Set[helpSourceDir];
            };
            this.tickProgress;
            this.getAllMetaData;
            this.tickProgress;

            fileList = Dictionary.new;
            count = 0;
            this.postProgress("Updating all files");
            helpSourceDirs.do {|dir|
                fileList[dir] = ("find -L"+dir.escapeChar($ )+"-type f -name '*.schelp' -not -name '*.ext.schelp'")
                    .unixCmdGetStdOutLines.reject(_.isEmpty).asSet;
                count = count + fileList[dir].size;
            };
            count2 = undocumentedClasses.size;
            progressCount = 3 * ((count+count2)/progressMax);
            progressMax = count+count2+progressCount;

            this.postProgress("Found"+count+"help files");
            helpSourceDirs.do {|dir|
                x = fileList[dir].size;
                if(x>0) {
                    fileList[dir].do {|path|
                        subtarget = path[dir.size+1 .. path.findBackwards(".")?path.size-1];
                        dest = helpTargetDir+/+subtarget++".html";
                        if(force
                        or: {("test"+path.escapeChar($ )+"-nt"+dest.escapeChar($ )+"-o ! -e"+dest.escapeChar($ )).systemCmd==0}) {
                            this.parseAndRender(path,dest,subtarget);
                        } {
                            this.postProgress("Skipping"+dest);
                        };
                        this.tickProgress;
                    };
                };
            };
            this.postProgress("Found"+count2+"undocumented classes");
            undocumentedClasses.do {|name|
                dest = helpTargetDir+/+"Classes"+/+name++".html";
                if(this.makeClassTemplate(name.asString,dest).not) {
                    this.postProgress("Skipping"+dest);
                };
                this.tickProgress;
            };
            this.postProgress("Done! time spent:"+(Main.elapsedTime-t)+"sec");
            verbose = oldVerbose;
            doneFunc.value();
        };

        doWait = threaded;
        if(doWait, {
            Routine(func).play(AppClock);
        }, func);
    }

    *cleanState {|noCache=false|
        didRun = false;
        helpSourceDirs = nil;
        if(noCache) {
            doWait = thisThread.isKindOf(Routine);
            this.syncNonHelpFiles; // ensure helpTargetDir exists
            ("touch"+(helpTargetDir+/+"version").escapeChar($ )).systemCmd;
        }
    }

    *findHelpSource {|subtarget|
        var src;
        this.findHelpSourceDirs;
        block {|break|
            helpSourceDirs.do {|dir|
                var x = dir+/+subtarget++".schelp";
                if(File.exists(x)) {
                    src = x;
                    break.value;
                };
            };
        };
        ^src
    }

    *prepareHelpForURL {|url|
        var proto, path, query, anchor;
        var subtarget, src, c, cmd;
        var verpath = this.helpTargetDir +/+ "version";

        doWait = thisThread.isKindOf(Routine);

        if(isProcessing) {
            "SCDoc: prepareHelpForURL already running.. waiting for the first to finish.".warn;
            if(doWait.not) {
                Error("SCDoc: cannot wait for already running prepareHelpForURL, this call was not made inside a Routine").throw;
            };
            c = Condition.new;
            Routine {
                while {0.5.wait; isProcessing};
                c.unhang;
            }.play(AppClock);
            c.hang;
        };
        protect {
            isProcessing = true;

            // parse URL
            url = url.replace("%20"," ");
            #proto, path, query, anchor = url.findRegexp("(^\\w+://)?([^#?]+)(\\?[^#]+)?(#.*)?")[1..].flop[1];
            if(proto.isEmpty) {proto="file://"};
            if(proto!="file://") {isProcessing = false; ^url}; // just pass through remote url's
            if(path.beginsWith(helpTargetDir).not) {isProcessing = false; ^url}; // just pass through remote url's

            if(File.exists(helpTargetDir).not) {
                this.cleanState;
            };

            // sync non-schelp files once every session
            if(didRun.not) {
                didRun = true;
                this.getAllMetaData;
            };

            // strip to subfolder/basename (like Classes/SinOsc)
            subtarget = path[helpTargetDir.size+1 .. path.findBackwards(".")?path.size-1];

            // find help source file
            src = this.findHelpSource(subtarget);

            // create a simple stub if class was undocumented
            if(src.isNil and: {subtarget.dirname=="Classes"}) {
                this.makeClassTemplate(subtarget.basename,path);
            };

            if(File.exists(path).not) {
                if(src.notNil) { // no target file, but helpsource found, parse and render.
                    this.parseAndRender(src,path,subtarget);
                    isProcessing = false;
                    ^url;
                } {
                    this.postProgress("Broken link:"+url);
                    isProcessing = false;
                    ^nil;
                };
            } {
                cmd = {
                    ("test" + ([src,verpath]++doc_map[subtarget].additions).collect {|x|
                        x.escapeChar($ )+"-nt"+path.escapeChar($ )
                    }.join(" -o ")).systemCmd == 0
                };

                if(src.notNil and: cmd) {
                    // target file and helpsource exists, and helpsource is newer than target
                    this.parseAndRender(src,path,subtarget);
                    isProcessing = false;
                    ^url;
                };
                // we reach here if the target URL exists and there's no schelp source file,
                // or if the target URL does not need update.
            };
        } {
            isProcessing = false;
        };
        ^url;
    }

    *makeClassTemplate {|name,path|
        var class = name.asSymbol.asClass;
        var n, m, cats, f;

        doWait = thisThread.isKindOf(Routine);

        if(class.notNil and: {path.isNil
            or: {
                ("test"+(helpTargetDir+/+"version").escapeChar($ )+"-nt"+path.escapeChar($ )+"-o ! -e"+path.escapeChar($ )).systemCmd==0
            }
        }) {
            this.postProgress("Undocumented class:"+name+", generating stub and template");
            cats = "Undocumented classes";
            if(this.classHasArKrIr(class)) {
                cats = cats ++ ", UGens>Undocumented";
            };
            if(class.categories.notNil) {
                cats = cats ++ ", "++class.categories.join(", ");
            };

            m=[(
                tag:\prose,
                text:"This class is missing documentation. See the ",
                display:\block
            ),(
                tag:\link,
                text:"#help_template#generated help template",
                display:\inline
            )];

            n = [
                (tag:\class, text:name),
                (tag:\summary, text:"(not documented)"),
                (tag:\categories, text:cats),
                (tag:\description, children:m)
            ];

            n = this.addMethodList(class.class,n,\classmethods);
            n = this.addMethodList(class,n,\instancemethods);

            f = {|tag|
                var txt,c,m,l,last,sym;
                if(tag==\classmethods) {
                    txt = "\nCLASSMETHODS::\n\n";
                    c = class.class;
                } {
                    txt = "\nINSTANCEMETHODS::\n\n";
                    c = class;
                };
                p.findNode(tag,n).children.do {|x|
                    if(x.tag==\method) {
                        txt = txt ++ "METHOD::" + x.text ++ "\n(describe method here)\n\n";
                        sym = x.text.asSymbol;
                        m = c.findRespondingMethodFor(sym.asSetter);
                        m = m ?? {c.findRespondingMethodFor(sym)};
                        m !? {
                            l = m.argNames;
                            last = l.size-1;
                            l.do {|a,i|
                                if (i>0) { //skip 'this' (first arg)
                                    txt = txt ++ "argument:: ";
                                    if(i==last and: {m.varArgs}) {
                                        txt = txt ++ " ... ";
                                    };
                                    txt = txt ++ a ++ "\n(describe argument here)\n\n";
                                }
                            }
                        }
                    };
                    txt = txt ++ "returns:: (returnvalue)\n\n";
                };
                txt;
            };

            m = "CLASS::"+name
                ++"\nsummary:: (put short description here)\n"
                ++"categories::"+cats
                ++"\nrelated:: Classes/SomeRelatedClass, Reference/SomeRelatedStuff, etc.\n\n"
                ++"DESCRIPTION::\n(put long description here)\n\n"
                ++ f.(\classmethods) ++ f.(\instancemethods)
                ++"\nEXAMPLES::\n\ncode::\n(some example code)\n::\n";

            n = n.add((
                tag:\section, text:"Help Template", children:[
                    (tag:\prose, display:\block,
                    text:"Copy the template below or run"),
                    (tag:\code,
                    text:"Document.new(string:SCDoc.makeClassTemplate(\\"++name++"))",
                    display:\block),
                    (tag:\prose,
                    text:"to open a new Document with the template.\nSave it to HelpSource/Classes/"++name++".schelp",
                    display:\block),
                    (tag:\code,
                    text:m,
                    display:\block)
                ]
            ));

            if(path.notNil) {
                p.root = n;
                p.currentFile = nil;
                r.render(p,path,"Classes/"++name);
                ^true;
            } {
                ^m;
            };
        };
        ^if(path.notNil,false,nil);
    }

    *checkSystemCmd {|cmd|
        if(("which"+cmd+"> /dev/null").systemCmd != 0) {
            Error("'"++cmd++"' is not installed. Please install it and try again.").throw;
        };
    }

    *findHelpSourceDirs {
        if(helpSourceDirs.notNil) {^this};
        this.postProgress("Finding HelpSource folders...");
        this.checkSystemCmd("find");
        helpSourceDirs = Set[helpSourceDir];
        [thisProcess.platform.userExtensionDir, thisProcess.platform.systemExtensionDir].do {|dir|
            helpSourceDirs = helpSourceDirs | ("find -L"+dir.escapeChar($ )+"-name 'HelpSource' -type d -prune")
                .unixCmdGetStdOutLines.asSet;
        };
        this.postProgress(helpSourceDirs.asString);
    }

    *syncNonHelpFiles {
        var cmd, c;

        doWait = thisThread.isKindOf(Routine);
        this.findHelpSourceDirs;
        this.postProgress("Synchronizing non-schelp files...");
        this.checkSystemCmd("rsync");

        cmd = "rsync -rlt --chmod=u+rwX --exclude '*.schelp' --exclude '.*' %/ %";

        if(doWait) {
            c = Condition.new;
            helpSourceDirs.do {|dir|
                cmd.format(dir.escapeChar($ ),helpTargetDir.escapeChar($ )).unixCmd({c.unhang},false);
                c.hang;
            };
        } {
            helpSourceDirs.do {|dir|
                cmd.format(dir.escapeChar($ ),helpTargetDir.escapeChar($ )).systemCmd;
            };
        };
        this.postProgress("Synchronizing non-schelp files: Done");
    }

    *getAllMetaData {
        var subtarget, classes, cats, t = Main.elapsedTime;
        var update = false, doc, ndocs;

        this.syncNonHelpFiles; // ensure that helpTargetDir exist
        classes = Class.allClasses.collectAs(_.name,IdentitySet).reject(_.isMetaClassName);
        this.readDocMap;
        this.postProgress("Getting metadata for all docs...");

        //FIXME: if classtree changed, force total re-render (touch version timestamp)

        ndocs = 0;
        helpSourceDirs.do {|dir|
            var x, path, mtime, ext, sym, class;
            ext = (dir != helpSourceDir);
            this.postProgress("- Collecting from"+dir);
            Platform.case(
//                \linux, {"find -L"+dir.escapeChar($ )+"-type f -name '*.schelp' -printf '%p;%T@\n'"},
                \linux, {"find -L"+dir.escapeChar($ )+"-type f -name '*.schelp' -not -name '*.ext.schelp' -exec stat -c \"%n;%Z\" {} +"},
                \osx, {"find -L"+dir.escapeChar($ )+"-type f -name '*.schelp' -not -name '*.ext.schelp' -exec stat -f \"%N;%m\" {} +"}
            ).unixCmdGetStdOutLines.do {|line|
                #path, mtime = line.split($;);
                subtarget = path[dir.size+1 ..].drop(-7);
                doc = doc_map[subtarget];

                if(subtarget.dirname=="Classes") {
                    sym = subtarget.basename.asSymbol;
                    class = sym.asClass;
                } {
                    sym = nil;
                };

                //FIXME: if implementor class changed since last time, force a re-render.
                //if doc.redirect && doc.implementor != class.tryPerform(doc.redirect.asSymbol).asSymbol

                if(doc.isNil or: {mtime != doc.mtime}) {
                    p.parseMetaData(path);
                    this.addToDocMap(p,subtarget);
                    doc = doc_map[subtarget];
                    if(dir.beginsWith(Platform.userExtensionDir +/+ "quarks")) {
                        x = "Quarks>"++dir.dirname.basename;
                        if(doc.categories.notEmpty) { x = x ++ ", " ++ doc.categories };
                        doc.categories = x;
                    };
                    doc.methods = p.methodList;
                    doc.keywords = p.keywordList;
                    doc.mtime = mtime;
                    doc.installed = if(ext){\extension}{\standard};
                    if(sym.notNil) { // doc is a class-doc
                        if(class.notNil) { // class exists
                            doc.superclasses = class.superclasses.collect(_.name).reject(_.isMetaClassName);
                            doc.subclasses = class.subclasses.collect(_.name).reject(_.isMetaClassName);
                            x = p.findNode(\redirect).text.stripWhiteSpace;
                            if(x.notEmpty) {
                                x = try { class.perform(x.asSymbol) };
                                x !? { doc.implementor = x.asSymbol };
                            };
                        } {
                            doc.installed = \missing;
                        };
                    };
                    update = true;
                    ndocs = ndocs + 1;
                };
                doc.keep = true;
                if(sym.notNil) {
                    classes.remove(sym);
                };
                this.maybeWait;
            };
        };
        this.postProgress("Added"+ndocs+"new documents");

        ndocs = 0;
        helpSourceDirs.do {|dir|
            var old;
            ("find -L"+dir.escapeChar($ )+"-type f -name '*.ext.schelp'").unixCmdGetStdOutLines.do {|file|
                subtarget = file[dir.size+1 ..].drop(-11);
                doc = doc_map[subtarget];
                if(doc.notNil) {
                    // FIXME: if this doc adds a method to a non-class doc, it will not show up in doc.methods...
                    old = doc.additions.copy;
                    doc.additions = doc.additions.add(file).asSet;
                    // filter out non-existing files in case helpsource moved..
                    // this is more a hackish workaround than a good solution.
                    // it will not work if a doc addition is removed instead of moved.
                    doc.additions = doc.additions.select(File.exists(_));
                    update = update or: {doc.additions != old};
                    ndocs = ndocs + 1;
                    this.postProgress("Addition for"+subtarget+":"+file);
                } {
                    warn("SCDoc: Ignoring additions for non-existing document:"+file);
                };
                this.maybeWait;
            }
        };
        this.postProgress("Found"+ndocs+"document additions");

        this.postProgress("Processing"+classes.size+"undocumented classes");
        undocumentedClasses = classes;
        ndocs = 0;
        classes.do {|name|
            var class, dir;
            subtarget = "Classes/"++name.asString;
            if(doc_map[subtarget].isNil) {
                cats = "Undocumented classes";
                class = name.asClass;
                dir = class.filenameSymbol.asString;
                if(dir.beginsWith(Platform.userExtensionDir +/+ "quarks")) {
                    cats = cats ++ ", " ++ "Quarks>"++class.package;
                };

                if(this.classHasArKrIr(class)) {
                    cats = cats ++ ", UGens>Undocumented";
                };
                if(class.categories.notNil) {
                    cats = cats ++ ", "++class.categories.join(", ");
                };

                p.root = [
                    (tag:\class, text:name.asString),
                    (tag:\categories, text:cats),
                    (tag:\summary, text:"(not documented)")
                ];
                this.addToDocMap(p,subtarget);
                doc = doc_map[subtarget];
                doc.methods =
                    (this.makeMethodList(class.class).collect{|m| "_*"++m}
                    ++ this.makeMethodList(class).collect{|m| "_-"++m});

                doc.installed = if(class.filenameSymbol.asString.beginsWith(thisProcess.platform.classLibraryDir).not)
                    {\extension}
                    {\standard};

                doc.superclasses = class.superclasses.collect(_.name).reject(_.isMetaClassName);
                doc.subclasses = class.subclasses.collect(_.name).reject(_.isMetaClassName);

                ndocs = ndocs + 1;
                update = true;
            };
            doc_map[subtarget].keep = true;
            this.maybeWait;
        };
        this.postProgress("Generated metadata for"+ndocs+"undocumented classes");
        // NOTE: If we remove a Classes/Name.schelp for an existing class, the doc_map won't get updated.
        // but this shouldn't happen in real-life anyhow..

        ndocs = 0;
        doc_map.pairsDo{|k,e|
            if(e.keep!=true, {
                doc_map.removeAt(k);
                ndocs = ndocs + 1;
                update = true;
            });
            e.removeAt(\keep); //remove the key since we don't need it anymore
        };
        this.postProgress("Removed"+ndocs+"documents from cache");

        if(update) {
            this.writeDocMap;
            this.postProgress("Writing JSON doc map");
            this.docMapToJSON(helpTargetDir +/+ "docmap.js");
            NotificationCenter.notify(SCDoc, \docMapDidUpdate);
        };
        this.postProgress("Done! time spent:"+(Main.elapsedTime-t)+"sec");
    }

    *findHelpFile {|str|
        ^r.findHelpFile(str.stripWhiteSpace);
    }
}

+ String {
    stripWhiteSpace {
        var ws = [$\n, $\r, $\t, $\ ];
        var a=0, b=this.size-1;
        while({ ws.includes(this[a])},{a=a+1});
        while({ ws.includes(this[b])},{b=b-1});
        ^this.copyRange(a,b);
    }
	unixCmdGetStdOutLines {
		var pipe, lines, line;

		pipe = Pipe.new(this, "r");
		lines = Array.new;
		line = pipe.getLine;
		while({line.notNil}, {lines = lines.add(line); line = pipe.getLine; });
		pipe.close;

		^lines;
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

+ Help {
	*dir {
		^SCDoc.helpTargetDir
	}
}

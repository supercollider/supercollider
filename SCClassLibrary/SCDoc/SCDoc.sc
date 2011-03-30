SCDoc {
    classvar <helpTargetDir;
    classvar <helpSourceDir;
    classvar <helpBaseDir;
    classvar <helpSourceDirs;
    classvar doc_map = nil;
    classvar <p, <r;
    classvar doWait;
    classvar progressText = nil, progressWindow = nil;
    classvar progressCount = 0, progressMax = 0;
    classvar progressTopic = nil, progressBar = nil, closeButton = nil;
    classvar new_classes = nil;
    classvar didRun = false;

    *helpSourceDir_ {|path|
        helpSourceDir = path.standardizePath;
    }

    *helpBaseDir_ {|path|
        helpBaseDir = path.standardizePath;
    }

    *helpTargetDir_ {|path|
        if(path!=helpTargetDir) {didRun = false};
        helpTargetDir = path.standardizePath;
    }

    *waitForHelp {|func,gui=true|
        if(didRun.not)
            {this.updateAll(doneFunc:func,gui:gui)}
            {func.value};
    }

    *postProgress {|string,setTopic=false|
        var prg = "";
        if(progressMax>0) {prg = (progressCount/progressMax*100).round(0.1).asString ++ "% "};
        if(progressWindow.notNil) {
            if(setTopic, {
                progressTopic.string = string;
                progressText.string = prg;
            }, {
                progressText.string = prg+string;
            });
            if(progressMax>0) {progressBar.lo_(0).hi_(progressCount/progressMax)};
        };
        ("SCDoc:"+prg++string).postln;
        if(doWait, {0.wait});
    }

    *docMapToJSON {|path|
        var f;

        File.delete(path);
        f = File.open(path,"w");

        f.write("docmap = [\n");
        doc_map.pairsDo {|k,v|
            f.write("{\n");
            v.pairsDo {|k2,v2|
                if(v2.class == List) {
                    v2 = "["+v2.collect{|x|"'"++x++"'"}.join(",")+"]";
                } {
                    v2 = "'"++v2.asString.replace("'","\\'")++"'";
                };
                f.write("'"++k2++"': "++v2++",\n");
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
        this.helpSourceDir_(thisProcess.platform.classLibraryDir.dirname +/+ "HelpSource");
        this.helpBaseDir_(thisProcess.platform.classLibraryDir.dirname +/+ "HelpBase");
        this.helpTargetDir_(thisProcess.platform.userAppSupportDir +/+ "Help");
        r = SCDocHTMLRenderer.new;
        p = SCDocParser.new;
        doWait = false;
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
        var l = m.argNames;
        var last = l.size-1;
        l.do {|a,i|
            if (i>0) { //skip 'this' (first arg)
                if(i==last and: {m.varArgs}) {
                    res = res ++ " ... " ++ a;
                } {
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
        };
        if (res.notEmpty) {
            ^("("++res++")");
        } {
            ^"";
        };
    }

    *handleUndocumentedClasses {
        var n, m, name, cats, dest;
        var destbase = helpTargetDir +/+ "Classes";
        this.postProgress("Generating docs for"+new_classes.size+"undocumented classes...",true);
        new_classes.do {|sym|
            var c = sym.asClass;
            var name = sym.asString;
            dest = destbase +/+ name ++ ".html";
            n = List.new;
            n.add((tag:\class, text:name));
            n.add((tag:\summary, text:""));

            cats = "Undocumented classes";
            if(this.classHasArKrIr(c)) {
                cats = cats ++ ", UGens>Undocumented";
            };
            if(c.categories.notNil) {
                cats = cats ++ ", "++c.categories.join(", ");
            };
            n.add((tag:\categories, text:cats));

            p.root = n;
            this.addToDocMap(p, "Classes" +/+ name);
            
            n.add((tag:\description, children:m=List.new));
            m.add((tag:\prose, text:"This class is missing documentation. Please create and edit HelpSource/Classes/"++name++".schelp", display:\block));

            // FIXME: Remove this when conversion to new help system is done!
            m.add((tag:\prose, text:"Old help file: ", display:\block));
            m.add((tag:\link, text:Help.findHelpFile(name) ?? "not found", display:\inline));

            this.makeMethodList(c.class,n,\classmethods);
            this.makeMethodList(c,n,\instancemethods);
            r.render(p,dest,"Classes");

            doc_map["Classes" +/+ name].delete = false;
            doc_map["Classes" +/+ name].methods = r.methods;
            this.tickProgress;
        };
    }
    
/*    *classHash {|c|
        var hash;
        //Problems: probably very slow! Also it doesn't work if one of the superclasses changed..
        hash = (((c.methods ++ c.class.methods) ?? []).collect {|m| m.code}).flatten.asString.hash;
        ^hash;
    }*/

    *addToDocMap {|parser, path|
        var c, x = parser.findNode(\class).text;
        var doc = (
            path:path,
            summary:parser.findNode(\summary).text,
            categories:parser.findNode(\categories).text
        );

        doc.title = if(x.notEmpty,x,{parser.findNode(\title).text});
        
        // check if class is standard, extension or missing
        if(x.notEmpty) {
            c = path.basename.asSymbol.asClass;
            doc.installed = if(c.notNil) {
                if(c.filenameSymbol.asString.beginsWith(thisProcess.platform.classLibraryDir).not)
                    {\extension}
                    {\standard}
            } {\missing};
        };

//        if(x.notEmpty) {
//            doc.hashnum = this.classHash(x.stripWhiteSpace.asSymbol.asClass);
//        };
        
        doc_map[path] = doc;
    }

    *makeCategoryMap {
        var cats, c, map;
        this.postProgress("Creating category map...");
        map = Dictionary.new;
        doc_map.pairsDo {|k,v|
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

    *readDocMap {
        var path = this.helpTargetDir +/+ "scdoc_cache";
        doc_map = path.load;

        if(doc_map.isNil) {
            doc_map = Dictionary.new;
            "SCDoc: docMap cache not found, created new".postln;
            ^true;
        } {
            "SCDoc: read docMap cache from file".postln;
        };
        ^false;
    }

    *writeDocMap {
        var f, path = this.helpTargetDir +/+ "scdoc_cache";
        File.delete(path);
        f = File.open(path,"w");
        f.write(doc_map.asCompileString);
        f.close;
        "SCDoc: wrote docMap cache".postln;
    }
    
    *docMap {
        if(doc_map.isNil) {
            this.readDocMap;
        };
        ^doc_map;
    }

    *makeProgressWindow {
        if(GUI.scheme.name != \QtGUI, {^nil});
        
        if(progressWindow.isNil) {
            progressWindow = Window("Documentation update",500@200).alwaysOnTop_(true).userCanClose_(false).layout_(QVLayout.new);
            progressWindow.onClose = {progressWindow = nil};
            StaticText(progressWindow).string_("Please wait while updating help files...");
            progressBar = RangeSlider(progressWindow,300@20).orientation_(\horizontal).background_(Color(0.8,0.8,0.8)).knobColor_(Color(0.5,0.5,0.8));
            progressTopic = StaticText(progressWindow).font_(Font.defaultSansFace.boldVariant);
            progressText = TextView(progressWindow).editable_(false);
            closeButton = Button(progressWindow).states_([["Close"]]).enabled_(false).action = {progressWindow.close; progressWindow = nil};
            progressWindow.front;
        };
    }

    *updateFile {|source, rootDir|
        var lastDot = source.findBackwards(".");
        var subtarget = source.copyRange(rootDir.size + 1, lastDot - 1);
        var target = helpTargetDir +/+ subtarget ++".html";
        var folder = target.dirname;
        var ext = source.copyToEnd(lastDot);
        if(source.beginsWith(rootDir).not) {
            error("File location error:\n"++source++"\nis not inside "++rootDir);
            ^nil;
        };
        if(ext == ".schelp", {
            this.postProgress("Parsing "++source);
            p.parseFile(source);
            this.addToDocMap(p,subtarget);
            r.render(p,target,subtarget.dirname);
            doc_map[subtarget].methods = r.methods;
            if(subtarget.dirname=="Classes") {
                if(new_classes.includes(subtarget.basename.asSymbol)) {
                    new_classes.remove(subtarget.basename.asSymbol);
                    progressMax = progressMax - 1;
                };
            };
        }, {
            this.postProgress("Copying" + source + "to" + (folder +/+ source.basename));
            ("mkdir -p"+folder.escapeChar($ )).systemCmd;
            ("cp" + source.escapeChar($ ) + folder.escapeChar($ )).systemCmd;
        });
    }
    
    *tickProgress { progressCount = progressCount + 1 }

    *initHelpTargetDir {
        var sysdir = helpBaseDir.escapeChar($ );
        var stamp = helpTargetDir.escapeChar($ )+/+"helpbase_timestamp";
        /*
        NOTE:
        if the system-wide HelpBase dir was updated since last time we based our help-dir on it,
        then copy it again. Any pre-rendered help that was updated (due to a new version of SC, etc)
        will thus be copied over and overwrite the outdated html file in helpTargetDir.

        Note that rsync does not check if sourcefile is newer, only if it differs.
        This means that scdoc_cache, classlist_cache, helpdirlist_cache will also be replaced,
        thus triggering a re-compile of all docs not part of HelpBase (main SC without extensions).
        This is probably a good thing, since a new SC version might mean that SCDoc changed internally
        and that all docs should be re-compiled.
        */
        if(File.exists(helpTargetDir).not or: {("test"+sysdir+"-nt"+stamp+"-o \\! -e"+stamp).systemCmd==0}) {
            this.postProgress("Initializing user's help directory", true);
            if(File.exists(helpBaseDir)) {
                this.postProgress("Basing help tree on pre-rendered help, please wait...");
//                ("rsync -ax --link-dest="++sysdir+sysdir++"/"+helpTargetDir.escapeChar($ )+"2>/dev/null").systemCmd;
                ("rsync -rlt"+sysdir++"/"+helpTargetDir.escapeChar($ )+"2>/dev/null").systemCmd;
                this.postProgress("Done, creating timestamp");
                ("touch -r"+sysdir+stamp).systemCmd;
            } {
                this.postProgress("No pre-rendered help found, creating from scratch...");
            }
        };
    }

    *updateAll {|force=false,doneFunc=nil,threaded=true,gui=true,findExtensions=true,useHelpBase=true|
        var func;
        var docmap_path = helpTargetDir.escapeChar($ )+/+"scdoc_cache";
        var classlist_path = helpTargetDir+/+"classlist_cache";

        didRun = true;
        
        func = {
            var fileList, count, maybeDelete, x, f, n, old_classes, current_classes;

            if(useHelpBase) {
                this.initHelpTargetDir;
            };

            if(force.not) {
                force = this.readDocMap;
            } {
                doc_map = Dictionary.new;
            };

            progressMax = 1;
            progressCount = 0;
            maybeDelete = false;
            
            new_classes = IdentitySet.new;

            this.postProgress("Searching for HelpSource folders...",true);

            // check for existence of main helpSourceDir
            if(File.exists(helpSourceDir).not) {
                progressCount = 1;
                this.postProgress(helpSourceDir+"does not exist!\n\nPlease set SCDoc.helpSourceDir to SCDoc's HelpSource folder and run SCDoc.updateAll again.");
                progressWindow !? {
                    progressText.stringColor = Color(1,0,0);
                    progressWindow.userCanClose = true;
                    closeButton.enabled = true;
                };
                ^nil;
            };

            // find the set of helpSourceDirs
            helpSourceDirs = Set[helpSourceDir];
            if(findExtensions) {
                [thisProcess.platform.userExtensionDir, thisProcess.platform.systemExtensionDir].do {|dir|
                    helpSourceDirs = helpSourceDirs | ("find -L"+dir.escapeChar($ )+"-name 'HelpSource' -type d -prune")
                        .unixCmdGetStdOutLines.reject(_.isEmpty).asSet;
                };
            };
            this.postProgress(helpSourceDirs);

            this.postProgress("Ensuring help directory structure");
            x = Set.new;
            helpSourceDirs.do {|srcdir|
                x = x | ("find"+srcdir.escapeChar($ )+"\\( ! -regex '.*/\\..*' \\) -type d").unixCmdGetStdOutLines
                    .collect {|dir|
                        (helpTargetDir+/+dir.copyToEnd(srcdir.size)).escapeChar($ )
                    }.asSet;
            };
            if(x.isEmpty) {
                Error("SCDoc: strange error: could not replicate directory structure").throw;
            };
            x.do {|dir|
                this.postProgress("-"+dir);
                ("mkdir -p"+dir).systemCmd;
            };

            // get list of new or updated files
            fileList = Dictionary.new;
            count = 0;
            if(force) {
                this.postProgress("Updating all files",true);
                helpSourceDirs.do {|dir|
                    fileList[dir] = ("find -L"+dir.escapeChar($ )+"-type f")
                        .unixCmdGetStdOutLines.reject(_.isEmpty).asSet;
                    count = count + fileList[dir].size;
                };
            } {
                this.postProgress("Searching for new or updated files...",true);
                helpSourceDirs.do {|dir|
                    fileList[dir] = ("find -L"+dir.escapeChar($ )+"-newer"+docmap_path+"\\( ! -regex '.*/\\..*' \\) -type f")
                        .unixCmdGetStdOutLines.reject(_.isEmpty).asSet;
                    count = count + fileList[dir].size;
                };
            };
            this.postProgress("Found"+count+"files in need of update");

            //Read a list of all classes so that we can detect if any new ones where added (extensions).
            old_classes = Object.readArchive(classlist_path);
            current_classes = Class.allClasses.collectAs(_.name,IdentitySet).reject(_.isMetaClassName);
            this.postProgress("Checking for new classes...",true);
            if(old_classes.notNil) {
                current_classes.do{|name|
                    if(old_classes.includes(name).not) {
                        //or: {("test"+name.asClass.filenameSymbol.asString.escapeChar($ )+"-ot"+classlist_path).systemCmd!=0}) {
                        //tried to test if class source file is newer than the class_cache file to catch class file updates,
                        //but it is *very* slow and also won't trigger writing a new classlist_path file so that can't
                        //be used as time reference anyhow..
                        //Would be nice with a way to detect if a class did change though..
                        new_classes.add(name);
                    };
                };
            } {
                // this is a fresh run
                new_classes = current_classes.copy;
            };
            this.postProgress("Found"+new_classes.size+"new classes");

            // add all files in added HelpSource folders (e.g. newly installed quarks/extensions)
            x = Object.readArchive(helpTargetDir+/+"helpdirlist_cache");
            if(x.notNil) {
                (helpSourceDirs - x).do {|dir|
                    if(dir != helpSourceDir) {
                        this.postProgress("Found new HelpSource folder:"+dir);
                        fileList[dir] = fileList[dir] | ("find -L"+dir.escapeChar($ )+"\\( ! -regex '.*/\\..*' \\) -type f")
                            .unixCmdGetStdOutLines.reject(_.isEmpty).asSet;
                    };
                };
                if((x - helpSourceDirs).notEmpty) {
                    maybeDelete = true;
                };
            };
            File.delete(helpTargetDir+/+"helpdirlist_cache");
            helpSourceDirs.writeArchive(helpTargetDir+/+"helpdirlist_cache");

            count = 0;
            helpSourceDirs.do {|dir|
                count = count + fileList[dir].size;
            };
            this.postProgress("Total"+count+"files to process");
            progressMax = count + new_classes.size + 3;

            // parse/render or copy new and updated files
            // NOTE: any class docs processed here will remove that class from the new_classes set
            helpSourceDirs.do {|dir|
                x = fileList[dir].size;
                if(x>0) {
                    if(gui){this.makeProgressWindow};
                    this.postProgress("Processing"+x+"files in"+dir,true);
                    fileList[dir].do {|file|
                        this.updateFile(file,dir);
                        this.tickProgress;
                    };
                };
            };
            
            // use folder mtime to see if there might be any deleted files,
            // NOTE: this will also trigger on added helpfiles but that's ok I guess..
            if(force.not) {
                helpSourceDirs.do {|dir|
                    if(("find -L"+dir.escapeChar($ )+"-type d -newer"+docmap_path).unixCmdGetStdOut.isEmpty.not) {
                        maybeDelete = true;
                    };
                };
                if(maybeDelete or: force or: {old_classes != current_classes}) {
                    if(gui){this.makeProgressWindow};
                    this.postProgress("Help folders changed, scanning for deleted documents...",true);
                    doc_map.do(_.delete=true); // mark all docs in docMap for deletion
                    count = 0;
                    helpSourceDirs.do {|dir|
                        ("find -L"+dir.escapeChar($ )+"-name '*.schelp'").unixCmdGetStdOutLines.reject(_.isEmpty).do {|f|
                            var subtarget = f.copyRange(dir.size + 1, f.findBackwards(".") - 1);
                            x = doc_map[subtarget];
                            x !? {x.delete = false};
                        };
                    };
                    current_classes.do {|sym|
                        x = doc_map["Classes"+/+sym.asString];
                        x !? {x.delete = false};
                    };

                    doc_map.pairsDo{|k,e|
                        if(e.delete==true, {
                            this.postProgress("Removing"+e.path+"from cache");
                            doc_map.removeAt(k);
                            count = count + 1;
                        });
                        e.removeAt(\delete); //remove the key since we don't need it anymore
                    };
                    this.postProgress("Removed"+count+"documents");
                };
            };

            // generate simple doc for each class in new_classes, which now contains only undocumented *new* classes:
            if(new_classes.notEmpty) {
                if(gui){this.makeProgressWindow};
                this.handleUndocumentedClasses;
            };
            
            if(old_classes != current_classes) {
                if(gui){this.makeProgressWindow};
                File.delete(classlist_path);
                current_classes.writeArchive(classlist_path);
                this.postProgress("Generating Class tree...",true);
                p.overviewClassTree;
                r.render(p, helpTargetDir +/+ "Overviews/ClassTree.html", "Overviews", false);
                this.tickProgress;
            };

            this.writeDocMap;
            this.tickProgress;
            this.postProgress("Writing Document JSON index...",true);
            this.docMapToJSON(helpTargetDir +/+ "docmap.js");
            progressCount = progressMax;
            
            this.postProgress("Done!",true);
            "SCDoc done!".postln;
            doneFunc.value();
            doWait=false;
            progressWindow !? {
                progressWindow.userCanClose = true;
                closeButton.enabled = true;
            };
        };

        doWait = threaded or: gui;
//        if(gui, {this.makeProgressWindow});
        if(doWait, {
            Routine(func).play(AppClock);
        }, func);
        
    }

    *prepareHelpForURL {|url,doYield=false|
        var proto, path, anchor;
        var subtarget, src;

        var needMetaData = Set["Browse","Search","Overviews/Documents","Overviews/Classes","Overviews/Methods"];

        doWait = doYield;

        // parse URL
        #proto, path, anchor = url.findRegexp("(^\\w+://)?([^#]+)(#.*)?")[1..].flop[1];
        if(proto.isEmpty) {proto="file://"};
        if(proto!="file://") {^url}; // just pass through remote url's
        
        this.findHelpSourceDirs;
        
        // sync non-schelp files once every session
        if(didRun.not) {
            this.syncNonHelpFiles;
            didRun = true;
        };

        // strip to subfolder/basename (like Classes/SinOsc)
        subtarget = path[helpTargetDir.size+1 .. path.findBackwards(".")?path.size-1];
        
        // does URL need metadata?
        if(needMetaData.includes(subtarget)) {
            this.postProgress("URL needs metadata");
            if(doc_map.isNil) {
                this.getAllMetaData;
            };
            ^url;
        };

        if(subtarget=="Overviews/ClassTree") {
            this.postProgress("Generating Class tree...",true);
            p.overviewClassTree;
            r.render(p, path, "Overviews", false);
        };

        // find help source file
        block {|break|
            src = nil;
            helpSourceDirs.do {|dir|
                var x = dir+/+subtarget++".schelp";
                if(File.exists(x)) {
                    src = x;
                    break.value;
                };
            };
        };

        // create a simple stub if class was undocumented
        if(src.isNil and: {subtarget.dirname=="Classes"}) {
            this.postProgress("Undocumented class, generating stub and template");
            this.makeClassTemplate(subtarget.basename,path);
        };

        if(File.exists(path).not) {
            if(src.notNil) { // no target file, but helpsource found, parse and render.
                this.postProgress("Parsing"+src);
                p.parseFile(src);
                r.render(p,path,subtarget.dirname);
                ^url;
            } {
                ^"file://"++helpTargetDir++"/BrokenLink.html#"++url;
            };
        } {
            if(src.notNil and: {("test"+src.escapeChar($ )+"-nt"+path.escapeChar($ )).systemCmd==0}) {
                // target file and helpsource exists, and helpsource is newer than target
                p.parseFile(src);
                r.render(p,path,subtarget.dirname);
                ^url;
            };
        };
    
        ^url;
    }
    
    *makeClassTemplate {|name,path|
        var class = name.asSymbol.asClass;
        var n, m, cats, methodstemplate, f;
        if(class.notNil) {
            n = List.new;
            n.add((tag:\class, text:name));
            n.add((tag:\summary, text:""));

            cats = "Undocumented classes";
            if(this.classHasArKrIr(class)) {
                cats = cats ++ ", UGens>Undocumented";
            };
            if(class.categories.notNil) {
                cats = cats ++ ", "++class.categories.join(", ");
            };
            n.add((tag:\categories, text:cats));

            p.root = n;
            
            n.add((tag:\description, children:m=List.new));
            m.add((
                tag:\prose,
                text:"This class is missing documentation. See the ",
                display:\block
            ));
            m.add((
                tag:\link,
                text:"#help_template#generated help template",
                display:\inline
            ));

            this.makeMethodList(class.class,n,\classmethods);
            this.makeMethodList(class,n,\instancemethods);
            
            f = {|x|
                if(x.tag==\method) {
                    methodstemplate = methodstemplate ++ "Method::" + x.text ++ "\n(describe method here)\n\nreturns:: (returnvalue)\n\n";
                };
            };
            methodstemplate = "\nClassMethods::\n\n";
            p.findNode(\classmethods).children.do(f);
            methodstemplate = methodstemplate ++ "\nInstanceMethods::\n\n";
            p.findNode(\instancemethods).children.do(f);

            n.add((
                tag:\section, text:"Help Template", children:[
                    (tag:\prose, display:\block,
                    text:"Fill out and save the template below to HelpSource/Classes/"++name++".schelp"),
                    (tag:\code,
                    text:"Class::"+name
                    ++"\nsummary:: (put short description here)\n"
                    ++"categories::"+cats
                    ++"\nrelated:: Classes/SomeRelatedClass, Reference/SomeRelatedStuff, etc.\n\n"
                    ++"Description::\n(put long description here)\n\n"
                    ++methodstemplate
                    ++"\nExamples::\n\ncode::\n(some example code)\n::\n",
                    display:\block)
                ]
            ));

            r.render(p,path,"Classes");
        };
    }
    
    *findHelpSourceDirs {
        if(helpSourceDirs.notNil) {^this};
        this.postProgress("Finding HelpSource folders...");
        helpSourceDirs = Set[helpSourceDir];
        [thisProcess.platform.userExtensionDir, thisProcess.platform.systemExtensionDir].do {|dir|
            helpSourceDirs = helpSourceDirs | ("find -L"+dir.escapeChar($ )+"-name 'HelpSource' -type d -prune")
                .unixCmdGetStdOutLines.asSet;
        };
        this.postProgress(helpSourceDirs.asString);
    }
    
    *syncNonHelpFiles {
        this.findHelpSourceDirs;
        this.postProgress("Synchronizing non-schelp files");
        helpSourceDirs.do {|dir|
            ("rsync -rlt --exclude '*.schelp' --exclude '.*'"+dir.escapeChar($ )++"/"+helpTargetDir.escapeChar($ )+"2>/dev/null").systemCmd;
        };
    }
    
    *getAllMetaData {
        var subtarget, classes, mets, count = 0, t = Main.elapsedTime;
        this.postProgress("Getting metadata for all docs...");
        this.findHelpSourceDirs;
        // find undocumented classes
        classes = Class.allClasses.collectAs(_.name,IdentitySet).reject(_.isMetaClassName);
        doc_map = Dictionary.new;
        helpSourceDirs.do {|dir|
            this.postProgress("- Collecting from"+dir);
            ("find -L"+dir.escapeChar($ )+"-type f -name '*.schelp'").unixCmdGetStdOutLines.do {|path|
                mets = p.parseMetaData(path);
                subtarget = path[dir.size+1 ..].drop(-7);
                this.addToDocMap(p,subtarget);
                //FIXME: undocumented methods too?
                doc_map[subtarget].methods = mets;
                if(subtarget.dirname=="Classes") {
                    classes.remove(subtarget.basename.asSymbol);
                };
                if(doWait and: {count = count + 1; count > 10}, {0.wait; count = 0;});
            };
        };

        this.postProgress("Making metadata for undocumented classes");
        classes.do {|name|
            p.root = [
                (tag:\class, text:name.asString),
                (tag:\categories, text:"Undocumented classes")
            ];
            this.addToDocMap(p,"Classes/"++name.asString);
            //FIXME: methods too?
            if(doWait and: {count = count + 1; count > 10}, {0.wait; count = 0;});
        };
        
//        this.writeDocMap;   // not needed actually? unless we re-use this instead of parsing all headers,
                            // but then we need some magic to detect new files as in updateAll. checking for -newer scdoc_cache
                            // is not enough, also need to detect newly added helpSourceDir's
                            // or perhaps it's enough to see if the file is in the docmap?
                            // so for all *.schelp, add to filelist if not in docmap.
                            // then for all *.schelp newer than scdoc_cache, add to filelist.
                            // then parse everything in filelist.
        this.postProgress("Writing JSON doc map");
        this.docMapToJSON(helpTargetDir +/+ "docmap.js");
        this.postProgress("Done! time spent:"+(Main.elapsedTime-t)+"sec");
    }

    *findHelpFile {|str|
        ^r.findHelpFile(str);
    }
}

+ String {
    stripWhiteSpace {
        var a=0, b=this.size-1;
        while({(this[a]==$\n) or: (this[a]==$\ ) or: (this[a]==$\t)},{a=a+1});
        while({(this[b]==$\n) or: (this[b]==$\ ) or: (this[b]==$\t)},{b=b-1});
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

SCDoc {
    classvar <helpTargetDir;
    classvar <helpSourceDir;
//    classvar <categoryMap;
    classvar <docMap;
    classvar <p, <r;
    classvar doWait;
    classvar progressText = nil, progressWindow = nil;
    classvar progressCount = 0, progressMax = 1;
    classvar progressTopic = nil, progressBar = nil;
    classvar new_classes = nil;

    *helpSourceDir_ {|path|
        helpSourceDir = path.standardizePath;
    }

    *helpTargetDir_ {|path|
        helpTargetDir = path.standardizePath;
    }
    
    *postProgress {|string,setTopic=false|
        var prg = (progressCount/progressMax*100).round(0.1).asString ++ "%";
        if(progressText.notNil) {
            if(setTopic, {
                progressTopic.string = string;
                progressText.string = prg;
            }, {
                progressText.string = prg+string;
            });
            progressBar.lo_(0).hi_(progressCount/progressMax);
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
        
        SCDoc.postProgress("Generating Class tree...",true);
        p.overviewClassTree;
        r.renderHTML(helpTargetDir +/+ "Overviews/ClassTree.html","Overviews",false);
        progressCount = progressCount + 1;

        SCDoc.postProgress("Generating Class index...",true);
        p.overviewAllClasses(docMap);
        r.renderHTML(helpTargetDir +/+ "Overviews/Classes.html","Overviews",false);
        progressCount = progressCount + 1;

        SCDoc.postProgress("Generating Methods index...",true);
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
        
//        SCDoc.postProgress("Generating Documents index...",true);
//        p.overviewAllDocuments(docMap);
//        r.renderHTML(helpTargetDir +/+ "Overviews/Documents.html","Overviews", false);
//        progressCount = progressCount + 1;

//        SCDoc.postProgress("Generating Categories overview...");
//        p.overviewCategories(categoryMap);
//        r.renderHTML(helpTargetDir +/+ "Overviews/Categories.html","Overviews", true);
//        progressCount = progressCount + 1;
        
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
/*
    *handleUndocumentedClassesOld {|force=false|
        var n, m, name, cats, dest;
//        var src, dest;
//        var srcbase = helpSourceDir +/+ "Classes";
        var destbase = helpTargetDir +/+ "Classes";
        SCDoc.postProgress("Checking for undocumented classes...",true);
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
*/
    *handleUndocumentedClasses {
        var n, m, name, cats, dest;
        var destbase = helpTargetDir +/+ "Classes";
        SCDoc.postProgress("Generating docs for"+new_classes.size+"undocumented classes...",true);
        new_classes.do {|sym|
            var c = sym.asClass;
            var name = sym.asString;
            dest = destbase +/+ name ++ ".html";
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
            
//            SCDoc.postProgress("Generating doc for class: "++name);
            n.add((tag:\description, children:m=List.new));
            m.add((tag:\prose, text:"This class is missing documentation. Please create and edit HelpSource/Classes/"++name++".schelp", display:\block));

            // FIXME: Remove this when conversion to new help system is done!
            m.add((tag:\prose, text:"Old help file: ", display:\block));
            m.add((tag:\link, text:Help.findHelpFile(name) ?? "not found", display:\inline));

            this.makeMethodList(c.class,n,\classmethods);
            this.makeMethodList(c,n,\instancemethods);
            r.renderHTML(dest,"Classes");

            docMap["Classes" +/+ name].delete = false;
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
        
        docMap[path] = doc;
    }

    *makeCategoryMap {
        var cats, c, map;
        SCDoc.postProgress("Creating category map...");
        map = Dictionary.new;
        docMap.pairsDo {|k,v|
            cats = SCDoc.splitList(v.categories);
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
        var path = helpTargetDir +/+ "scdoc_cache";
        SCDoc.postProgress("Reading docMap cache...",true);
        docMap = path.load;

        if(docMap.isNil) {
            docMap = Dictionary.new;
            SCDoc.postProgress("Not found, Creating new docMap cache");
            ^true;
        };
        ^false;
    }

    *writeDocMap {
        var f, path = helpTargetDir +/+ "scdoc_cache";
        SCDoc.postProgress("Writing docMap cache",true);
        f = File(path,"w");
        f.write(docMap.asCompileString);
        f.close;
    }

    *makeProgressWindow {
        if(GUI.scheme.isNil and: doWait, {^nil});
        
        progressWindow = Window("Documentation update",500@150).alwaysOnTop_(true).userCanClose_(false).layout_(QVLayout.new);

        StaticText(progressWindow).string_("Please wait while updating help files...");
        progressBar = RangeSlider(progressWindow,300@20).orientation_(\horizontal).background_(Color(0.8,0.8,0.8)).knobColor_(Color(0.5,0.5,0.8));
        progressTopic = StaticText(progressWindow).font_(Font.defaultSansFace.boldVariant);
        progressText = TextView(progressWindow).editable_(false);

        progressWindow.front;
    }
/*
    *updateFileOld {|source, rootDir, force=false|
        var lastDot = source.findBackwards(".");
        var subtarget = source.copyRange(rootDir.size + 1, lastDot - 1);
        var target = helpTargetDir +/+ subtarget ++".html";
        var folder = target.dirname;
        var ext = source.copyToEnd(lastDot);
        var didUpdate = false;
        if(source.beginsWith(rootDir).not) {
            error("File location error:\n"++source++"\nis not inside "++rootDir);
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
*/
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
            SCDoc.postProgress("Parsing "++source);
            p.parseFile(source);
            this.addToDocMap(p,subtarget);
            r.renderHTML(target,subtarget.dirname);
            if(subtarget.dirname=="Classes",{new_classes.remove(subtarget.basename.asSymbol)});
        }, {
            SCDoc.postProgress("Copying" + source + "to" + (folder +/+ source.basename));
            ("mkdir -p"+folder.escapeChar($ )).systemCmd;
            ("cp" + source.escapeChar($ ) + folder.escapeChar($ )).systemCmd;
        });
    }
    
    *tickProgress { progressCount = progressCount + 1 }

    *updateAll {|force=false,doneFunc=nil,threaded=true,gui=true|
        var func;
        var docmap_path = SCDoc.helpTargetDir.escapeChar($ )+/+"scdoc_cache";
        var classlist_path = SCDoc.helpTargetDir+/+"classlist_cache";
        
        func = {
            var helpSourceDirs, fileList, count, maybeDelete, x, f, n, old_classes, current_classes;

            if(force.not) {
                force = this.readDocMap;
            } {
                docMap = Dictionary.new;
            };

            progressMax = 1;
            progressCount = 0;
            
            new_classes = IdentitySet.new;
            
            // get list of helpSourceDirs
            SCDoc.postProgress("Searching for HelpSource folders...",true);
            helpSourceDirs = [SCDoc.helpSourceDir];
            [thisProcess.platform.userExtensionDir, thisProcess.platform.systemExtensionDir].do {|dir|
                helpSourceDirs = helpSourceDirs ++ ("find -L"+dir.escapeChar($ )+"-name 'HelpSource' -type d")
                    .unixCmdGetStdOut.split($\n).reject(_.isEmpty);
            };

            SCDoc.postProgress(helpSourceDirs);

            // get list of new or updated files
            fileList = Dictionary.new;
            count = 0;
            if(force) {
                SCDoc.postProgress("Updating all files",true);
                helpSourceDirs.do {|dir|
                    fileList[dir] = ("find -L"+dir.escapeChar($ )+"-type f")
                        .unixCmdGetStdOut.split($\n).reject(_.isEmpty);
                    count = count + fileList[dir].size;
                };
            } {
                SCDoc.postProgress("Searching for new or updated files...",true);
                helpSourceDirs.do {|dir|
                    fileList[dir] = ("find -L"+dir.escapeChar($ )+"-newer"+docmap_path+"-type f")
                        .unixCmdGetStdOut.split($\n).reject(_.isEmpty);
                    count = count + fileList[dir].size;
                };
            };
            SCDoc.postProgress("Found"+count+"files in need of update");
            progressMax = count + 5;

            //Read a list of all classes so that we can detect if any new ones where added (extensions).
            old_classes = Object.readArchive(classlist_path);
            current_classes = Class.allClasses.collectAs(_.name,IdentitySet).reject(_.isMetaClassName);
            SCDoc.postProgress("Checking for new classes...",true);
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
            SCDoc.postProgress("Found"+new_classes.size+"new classes");

            /* FIXME:
               - read old HelpSourceDirs from archive
               - if one was added since last time, go through every file in that dir and compare
                 mtime to already rendered help, if newer then add to fileList[dir].
               - write new HelpSourceDirs to archive
               
               This catches newly installed extensions/quarks where the helpfiles was not newer than scdoc_cache

               is there a better solution for this? like checking the mtime for each helpsource dir compared
               to scdoc_cache (as we already do) but checking the mtime of the actual link?
               
               - check the mtime of the actual softlink or folder against scdoc_cache, if newer then go through every
               file in that dir and compare mtime to already rendered help, if newer then add to fileList[dir] so they will be updated.

               problem: the linked folder is most probably not the HelpSource folder itself, but the extensions folder..
               so we first need to collect all folders or links to folders in Extensions that is newer than scdoc_cache,
               then find any HelpSource folders under these and add all their files to fileList[dir]?

            */

            // parse/render or copy new and updated files
            // any class docs processed here will remove that class from the new_classes set
            helpSourceDirs.do {|dir|
                x = fileList[dir].size;
                if(x>0) {
                    SCDoc.postProgress("Processing"+x+"files in"+dir,true);
                    fileList[dir].do {|file|
                        this.updateFile(file,dir);
                        this.tickProgress;
                    };
                };
            };
            
            // use folder mtime to see if there might be any deleted files,
            // note: this will also trigger on added helpfiles but that's ok I guess..
            if(force.not) {
                maybeDelete = false;
                helpSourceDirs.do {|dir|
                    if(("find -L"+dir.escapeChar($ )+"-type d -newer"+docmap_path).unixCmdGetStdOut.isEmpty.not) {
                        maybeDelete = true;
                    };
                };
                if(maybeDelete or: force or: {old_classes != current_classes}) {
                    docMap.do(_.delete=true); // mark all docs in docMap for deletion
                    SCDoc.postProgress("Help folders changed, scanning for deleted documents...",true);
                    count = 0;
                    helpSourceDirs.do {|dir|
                        ("find -L"+dir.escapeChar($ )+"-name '*.schelp'").unixCmdGetStdOut.split($\n).reject(_.isEmpty).do {|f|
                            var subtarget = f.copyRange(dir.size + 1, f.findBackwards(".") - 1);
                            docMap[subtarget].delete = false;
                        };
                    };
                    current_classes.do {|sym|
                        x = docMap["Classes"+/+sym.asString];
                        x !? {x.delete = false};
                    };

                    docMap.pairsDo{|k,e|
                        if(e.delete==true, {
                            SCDoc.postProgress("Removing"+e.path+"from cache");
                            docMap.removeAt(k);
                            count = count + 1;
                        });
                        e.removeAt(\delete); //remove the key since we don't need it anymore
                    };
                    SCDoc.postProgress("Removed"+count+"documents");
                };
            };

            // generate simple doc for each class in new_classes, which now contains only undocumented *new* classes:
            if(new_classes.size>0) {
                progressMax = progressMax + new_classes.size;
                this.handleUndocumentedClasses;
            };
            
            if(old_classes != current_classes) {
                current_classes.writeArchive(classlist_path);
                SCDoc.postProgress("Generating Class tree...",true);
                p.overviewClassTree;
                r.renderHTML(helpTargetDir +/+ "Overviews/ClassTree.html","Overviews",false);
                this.tickProgress;

                SCDoc.postProgress("Generating Class index...",true);
                p.overviewAllClasses(docMap);
                r.renderHTML(helpTargetDir +/+ "Overviews/Classes.html","Overviews",false);
                this.tickProgress;
            };
            
            // move this to the old_classes!=current_classes check above? but methods can change even if the classlist has not..
            SCDoc.postProgress("Generating Methods index",true);
            f = File.open(SCDoc.helpTargetDir +/+ "methods.js","w");
            f.write("methods = [\n");
            this.collectAllMethods.pairsDo {|k,v|
                f.write("['"++k++"',[");
                v.do {|c,i|
                    n = c[0];
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
            this.tickProgress;

            this.writeDocMap;
            this.tickProgress;
            this.postProgress("Writing Document JSON index...",true);
            this.docMapToJSON(this.helpTargetDir +/+ "docmap.js");
            progressCount = progressMax;
            
            this.postProgress("Done!",true);
            "SCDoc done!".postln;
            doneFunc.value();
            doWait=false;
            progressWindow !? { progressWindow.userCanClose = true };

        };

        if(gui, {this.makeProgressWindow});
        if(doWait = threaded, {
            Routine(func).play(AppClock);
        }, func);
        
    }
/*
    *updateAllOld {|force=false,doneFunc=nil,threaded=true,gui=true|
        // FIXME: split updateFile() so that it first collects all files that needs updating, then we run it on that list.
        // then we can have progress of files that needs update, not for all files.
        var fileList = List.new;
        var processFiles = {
            SCDoc.postProgress("Processing helpfiles...",true);
            fileList.do {|entry|
                if(this.updateFile(entry[0], entry[1], force)) {
                    progressCount = progressCount + 1;
                } {
                    progressMax = progressMax - 1;
//                    SCDoc.postProgress("Skipping "++entry[0]);
                }
            };
        };
        var recurseHelpSource = {|dir|
//            SCDoc.postProgress("Parsing all in "++dir);
            SCDoc.postProgress("Found "++dir);
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

            SCDoc.postProgress("Collecting helpfiles...");
            recurseHelpSource.(helpSourceDir);
            findExtHelp.(PathName(thisProcess.platform.userExtensionDir));
            findExtHelp.(PathName(thisProcess.platform.systemExtensionDir));
            progressMax = fileList.size + 10;
            progressCount = 1;
            processFiles.value;
            progressCount = progressCount + 1;
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
//            this.makeCategoryMap;
            progressCount = progressCount + 1;
            this.makeOverviews;
            this.postProgress("Writing Document JSON index...",true);
            this.docMapToJSON(this.helpTargetDir +/+ "docmap.js");
            progressCount = progressMax;
            
            this.postProgress("SCDoc done!",true);
            doneFunc.value();
            doWait=false;
            progressWindow !? { progressWindow.userCanClose = true };
        };
        if(gui, {this.makeProgressWindow});
        if(doWait = threaded, {
            Routine(f).play(AppClock);
        }, f);
    }
*/
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

SCDoc {
    classvar <helpTargetDir;
    classvar <helpSourceDir;
    classvar helpSourceDirs;
    classvar doc_map = nil;
    classvar <p, <r;
    classvar <undocumentedClasses;
    classvar doWait;
    classvar progressText = nil, progressWindow = nil;
    classvar progressCount = 0, progressMax = 0;
    classvar progressTopic = nil, progressBar = nil, closeButton = nil;
    classvar new_classes = nil;
    classvar didRun = false;
    classvar isProcessing = false;
    classvar skipCacheOnce = false;
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
        this.maybeWait;
    }

    *docMapToJSON {|path|
        var f;

        File.delete(path);
        f = File.open(path,"w");

        f.write("docmap = [\n");
        doc_map.pairsDo {|k,v|
            f.write("{\n");
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
        f.write("]\n");
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
            l=Array.new;
            x.do {|m|
                l = l.add((tag:\method, text:m));
            };
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
                    l = l.add(name.asString);
                };
            };
        };
        ^l;
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

    *addToDocMap {|parser, path|
        var c, x = parser.findNode(\class).text;
        var doc = (
            path:path,
            summary:parser.findNode(\summary).text,
            categories:parser.findNode(\categories).text
        );

        doc.title = if(x.notEmpty,x,{parser.findNode(\title).text});
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

        // check if class is standard, extension or missing
        if(x.notEmpty) {
            c = path.basename.asSymbol.asClass;
            doc.installed = if(c.notNil) {
                if(c.filenameSymbol.asString.beginsWith(thisProcess.platform.classLibraryDir).not)
                    {\extension}
                    {\standard}
            } {\missing};
        };

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
            this.getAllMetaData;
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
            progressTopic = StaticText(progressWindow).font = Font(Font.defaultSansFace,12).boldVariant;
            progressText = TextView(progressWindow).editable_(false);
            closeButton = Button(progressWindow).states_([["Close"]]).enabled_(false).action = {progressWindow.close; progressWindow = nil};
            progressWindow.front;
        };
    }

    *tickProgress { progressCount = progressCount + 1 }

    *maybeWait {
        var t;
        if(doWait and: {(t = Main.elapsedTime)-lastUITick > 0.2}) {
            0.wait;
            lastUITick = t;
        }
    }

    *parseAndRender {|src,dest,kind|
        SCDoc.postProgress(src+"->"+dest);
        p.parseFile(src);
        r.render(p,dest,kind);
    }

    *renderAll {|force=false,gui=true,threaded=true,findExtensions=true,doneFunc|
        var count, count2, func, x, fileList, subtarget, dest, t = Main.elapsedTime;

        func = {
            progressMax = 100;
            progressCount = 0;

            if(findExtensions) {
                this.findHelpSourceDirs;
            } {
                helpSourceDirs = Set[helpSourceDir];
            };
            this.tickProgress;
            this.syncNonHelpFiles;
            this.tickProgress;
            this.getAllMetaData(true);
            this.tickProgress;
            
            fileList = Dictionary.new;
            count = 0;
            this.postProgress("Updating all files",true);
            helpSourceDirs.do {|dir|
                fileList[dir] = ("find -L"+dir.escapeChar($ )+"-type f -name '*.schelp'")
                    .unixCmdGetStdOutLines.reject(_.isEmpty).asSet;
                count = count + fileList[dir].size;
            };
            count2 = undocumentedClasses.size;
            progressCount = 3 * ((count+count2)/progressMax);
            progressMax = count+count2+progressCount;

            this.postProgress("Found"+count+"help files",true);
            helpSourceDirs.do {|dir|
                x = fileList[dir].size;
                if(x>0) {
                    fileList[dir].do {|path|
                        subtarget = path[dir.size+1 .. path.findBackwards(".")?path.size-1];
                        dest = helpTargetDir+/+subtarget++".html";
                        if(force
                        or: {("test"+path.escapeChar($ )+"-nt"+dest.escapeChar($ )+"-o ! -e"+dest.escapeChar($ )).systemCmd==0}) {
                            this.parseAndRender(path,dest,subtarget.dirname);
                        } {
                            this.postProgress("Skipping"+dest);
                        };
                        this.tickProgress;
                    };
                };
            };
            this.postProgress("Found"+count2+"undocumented classes",true);
            undocumentedClasses.do {|name|
                dest = helpTargetDir+/+"Classes"+/+name++".html";
                if(this.makeClassTemplate(name.asString,dest).not) {
                    this.postProgress("Skipping"+dest);
                };
                this.tickProgress;
            };
            this.postProgress("Done! time spent:"+(Main.elapsedTime-t)+"sec");
            doneFunc.value();
            progressWindow !? {
                progressWindow.userCanClose = true;
                closeButton.enabled = true;
            };
        };

        doWait = threaded or: gui;
        if(gui){this.makeProgressWindow};
        if(doWait, {
            Routine(func).play(AppClock);
        }, func);
    }

    *cleanState {|noCache=false|
        didRun = false;
        helpSourceDirs = nil;
        skipCacheOnce = noCache;
        // fixme: how can we force a re-render of files where the target already exists and is not older than the source?
        // letting getAllMetaData add an "doupdate" flag to each added doc in doc_map would work, but then we would need to
        // save the doc_map again (when??).
    }

    *prepareHelpForURL {|url,doYield=false|
        var proto, path, anchor;
        var subtarget, src, c;

        doWait = doYield;

        if(isProcessing) {
            "SCDoc: prepareHelpForURL already running.. waiting for the first to finish.".warn;
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
            #proto, path, anchor = url.findRegexp("(^\\w+://)?([^#]+)(#.*)?")[1..].flop[1];
            if(proto.isEmpty) {proto="file://"};
            if(proto!="file://") {isProcessing = false; ^url}; // just pass through remote url's
            if(path.beginsWith(helpTargetDir).not) {isProcessing = false; ^url}; // just pass through remote url's

            this.findHelpSourceDirs;

            if(File.exists(helpTargetDir).not) {
                this.cleanState;
            };

            // sync non-schelp files once every session
            if(didRun.not) {
                didRun = true;
                this.syncNonHelpFiles;
                this.getAllMetaData;
            };

            // strip to subfolder/basename (like Classes/SinOsc)
            subtarget = path[helpTargetDir.size+1 .. path.findBackwards(".")?path.size-1];

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
                this.makeClassTemplate(subtarget.basename,path);
            };

            if(File.exists(path).not) {
                if(src.notNil) { // no target file, but helpsource found, parse and render.
                    this.parseAndRender(src,path,subtarget.dirname);
                    isProcessing = false;
                    ^url;
                } {
                    this.postProgress("Broken link:"+url);
                    isProcessing = false;
                    ^nil;
                };
            } {
                if(src.notNil and: {("test"+src.escapeChar($ )+"-nt"+path.escapeChar($ )).systemCmd==0}) {
                    // target file and helpsource exists, and helpsource is newer than target
                    this.parseAndRender(src,path,subtarget.dirname);
                    isProcessing = false;
                    ^url;
                };
                // we reach here if the target URL exists and there's no schelp source file,
                // or if the target URL does not need update.
            };
        } {
            isProcessing = false;
        }
        ^url;
    }
    
    *makeClassTemplate {|name,path|
        var class = name.asSymbol.asClass;
        var n, m, cats, methodstemplate, f;
        f = class.filenameSymbol.asString.escapeChar($ );
        if(class.notNil and: {("test"+f+"-nt"+path.escapeChar($ )+"-o ! -e"+path.escapeChar($ )).systemCmd==0}) {
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
            
            f = {|x|
                if(x.tag==\method) {
                    methodstemplate = methodstemplate ++ "Method::" + x.text ++ "\n(describe method here)\n\nreturns:: (returnvalue)\n\n";
                };
            };

            methodstemplate = "\nClassMethods::\n\n";
            p.findNode(\classmethods,n).children.do(f);
            methodstemplate = methodstemplate ++ "\nInstanceMethods::\n\n";
            p.findNode(\instancemethods,n).children.do(f);

            n = n.add((
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
            p.root = n;
            r.render(p,path,"Classes");
            ^true;
        };
        ^false;
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
    
    *getAllMetaData {|force=false|
        var subtarget, classes, mets, cats, t = Main.elapsedTime;
        var update = false, doc, ndocs = 0;
        
        this.postProgress("Getting metadata for all docs...");
        this.findHelpSourceDirs;

        classes = Class.allClasses.collectAs(_.name,IdentitySet).reject(_.isMetaClassName);

        if(force or: skipCacheOnce) {
            doc_map = Dictionary.new;
            skipCacheOnce = false;
        } {
            this.readDocMap;
        };

        this.postProgress("Parsing metadata...");
        // parse all files in fileList
        helpSourceDirs.do {|dir|
            var path, mtime;
            this.postProgress("- Collecting from"+dir);
            Platform.case(
//                \linux, {"find -L"+dir.escapeChar($ )+"-type f -name '*.schelp' -printf '%p;%T@\n'"},
                \linux, {"find -L"+dir.escapeChar($ )+"-type f -name '*.schelp' -exec stat -c \"%n;%Z\" {} +"},
                \osx, {"find -L"+dir.escapeChar($ )+"-type f -name '*.schelp' -exec stat -f \"%N;%m\" {} +"}
            ).unixCmdGetStdOutLines.do {|line|
                #path, mtime = line.split($;);
                subtarget = path[dir.size+1 ..].drop(-7);
                doc = doc_map[subtarget];
                if(doc.isNil or: {mtime != doc.mtime}) {
                    mets = p.parseMetaData(path);
                    //FIXME: if doc uses 'classtree::', force a re-render by setting mtime=0 ??
                    this.addToDocMap(p,subtarget);
                    doc = doc_map[subtarget];
                    doc.methods = mets;
                    doc.mtime = mtime;
                    update = true;
                    ndocs = ndocs + 1;
                };
                doc.keep = true;
                if(subtarget.dirname=="Classes") {
                    classes.remove(subtarget.basename.asSymbol);
                };
                this.maybeWait;
            };
        };
        this.postProgress("Added"+ndocs+"new documents");
       
        this.postProgress("Processing"+classes.size+"undocumented classes");
        undocumentedClasses = classes;
        ndocs = 0;
        classes.do {|name|
            var class;
            subtarget = "Classes/"++name.asString;
            if(doc_map[subtarget].isNil) {
                cats = "Undocumented classes";
                class = name.asClass;
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
                doc_map[subtarget].methods =
                    (this.makeMethodList(class.class).collect{|m| "_*"++m}
                    ++ this.makeMethodList(class).collect{|m| "_-"++m});
                ndocs = ndocs + 1;
                update = true;
            };
            doc_map[subtarget].keep = true;
            this.maybeWait;
        };
        this.postProgress("Generated metadata for"+ndocs+"undocumented classes");

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
        };
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

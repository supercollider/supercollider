SCDoc {
    classvar <helpTargetDir;
    classvar <helpSourceDir;
    classvar doc_map = nil;
    classvar <p, <r;
    classvar doWait;
    classvar progressText = nil, progressWindow = nil;
    classvar progressCount = 0, progressMax = 1;
    classvar progressTopic = nil, progressBar = nil, closeButton = nil;
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
        doc_map.pairsDo {|k,v|
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
            if(this.classHasArKrIr(c), {
                cats = cats ++ ", UGens>Undocumented";
                if(c.categories.notNil) {
                    cats = cats ++ ", "++c.categories.join(", ");
                };
            });
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
            r.renderHTML(dest,"Classes");

            doc_map["Classes" +/+ name].delete = false;
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
        f = File(path,"w");
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
        if(GUI.scheme.isNil, {^nil});
        
        progressWindow = Window("Documentation update",500@200).alwaysOnTop_(true).userCanClose_(false).layout_(QVLayout.new);

        StaticText(progressWindow).string_("Please wait while updating help files...");
        progressBar = RangeSlider(progressWindow,300@20).orientation_(\horizontal).background_(Color(0.8,0.8,0.8)).knobColor_(Color(0.5,0.5,0.8));
        progressTopic = StaticText(progressWindow).font_(Font.defaultSansFace.boldVariant);
        progressText = TextView(progressWindow).editable_(false);
        closeButton = Button(progressWindow).states_([["Close"]]).enabled_(false).action = {progressWindow.close};

        progressWindow.front;
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
            r.renderHTML(target,subtarget.dirname);
            if(subtarget.dirname=="Classes",{new_classes.remove(subtarget.basename.asSymbol)});
        }, {
            this.postProgress("Copying" + source + "to" + (folder +/+ source.basename));
            ("mkdir -p"+folder.escapeChar($ )).systemCmd;
            ("cp" + source.escapeChar($ ) + folder.escapeChar($ )).systemCmd;
        });
    }
    
    *tickProgress { progressCount = progressCount + 1 }

    *updateAll {|force=false,doneFunc=nil,threaded=true,gui=true|
        var func;
        var docmap_path = this.helpTargetDir.escapeChar($ )+/+"scdoc_cache";
        var classlist_path = this.helpTargetDir+/+"classlist_cache";
        
        func = {
            var helpSourceDirs, fileList, count, maybeDelete, x, f, n, old_classes, current_classes;

            if(force.not) {
                force = this.readDocMap;
            } {
                doc_map = Dictionary.new;
            };

            ("mkdir -p"+this.helpTargetDir.escapeChar($ )).systemCmd;

            progressMax = 1;
            progressCount = 0;
            maybeDelete = false;
            
            new_classes = IdentitySet.new;

            // get list of helpSourceDirs
            this.postProgress("Searching for HelpSource folders...",true);

            if(File.exists(this.helpSourceDir).not) {
                progressCount = 1;
                this.postProgress(this.helpSourceDir+"does not exist!\n\nPlease set SCDoc.helpSourceDir to SCDoc's HelpSource folder and run SCDoc.updateAll again.");
                progressWindow !? {
                    progressText.stringColor = Color(1,0,0);
                    progressWindow.userCanClose = true;
                    closeButton.enabled = true;
                };
                ^this;
            };

            helpSourceDirs = Set[this.helpSourceDir];
            [thisProcess.platform.userExtensionDir, thisProcess.platform.systemExtensionDir].do {|dir|
                helpSourceDirs = helpSourceDirs | ("find -L"+dir.escapeChar($ )+"-name 'HelpSource' -type d -prune")
                    .unixCmdGetStdOutLines.reject(_.isEmpty).asSet;
            };

            this.postProgress(helpSourceDirs);

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
                    fileList[dir] = ("find -L"+dir.escapeChar($ )+"-newer"+docmap_path+"-type f")
                        .unixCmdGetStdOutLines.reject(_.isEmpty).asSet;
                    count = count + fileList[dir].size;
                };
            };
            this.postProgress("Found"+count+"files in need of update");
            progressMax = count + 4;

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
            x = Object.readArchive(this.helpTargetDir+/+"helpdirlist_cache");
            if(x.notNil) {
                (helpSourceDirs - x).do {|dir|
                    this.postProgress("Found new HelpSource folder:"+dir);
                    fileList[dir] = fileList[dir] | ("find -L"+dir.escapeChar($ )+"-type f")
                        .unixCmdGetStdOutLines.reject(_.isEmpty).asSet;
                };
                if((x - helpSourceDirs).notEmpty) {
                    maybeDelete = true;
                };
            };
            helpSourceDirs.writeArchive(this.helpTargetDir+/+"helpdirlist_cache");

            // parse/render or copy new and updated files
            // any class docs processed here will remove that class from the new_classes set
            helpSourceDirs.do {|dir|
                x = fileList[dir].size;
                if(x>0) {
                    this.postProgress("Processing"+x+"files in"+dir,true);
                    fileList[dir].do {|file|
                        this.updateFile(file,dir);
                        this.tickProgress;
                    };
                };
            };
            
            // use folder mtime to see if there might be any deleted files,
            // note: this will also trigger on added helpfiles but that's ok I guess..
            if(force.not) {
                helpSourceDirs.do {|dir|
                    if(("find -L"+dir.escapeChar($ )+"-type d -newer"+docmap_path).unixCmdGetStdOut.isEmpty.not) {
                        maybeDelete = true;
                    };
                };
                if(maybeDelete or: force or: {old_classes != current_classes}) {
                    doc_map.do(_.delete=true); // mark all docs in docMap for deletion
                    this.postProgress("Help folders changed, scanning for deleted documents...",true);
                    count = 0;
                    helpSourceDirs.do {|dir|
                        ("find -L"+dir.escapeChar($ )+"-name '*.schelp'").unixCmdGetStdOutLines.reject(_.isEmpty).do {|f|
                            var subtarget = f.copyRange(dir.size + 1, f.findBackwards(".") - 1);
                            doc_map[subtarget].delete = false;
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
            if(new_classes.size>0) {
                progressMax = progressMax + new_classes.size;
                this.handleUndocumentedClasses;
            };
            
            if(old_classes != current_classes) {
                current_classes.writeArchive(classlist_path);
                this.postProgress("Generating Class tree...",true);
                p.overviewClassTree;
                r.renderHTML(helpTargetDir +/+ "Overviews/ClassTree.html","Overviews",false);
                this.tickProgress;
            };
            
            // move this to the old_classes!=current_classes check above? but methods can change even if the classlist has not..
            this.postProgress("Generating Methods index",true);
            f = File.open(this.helpTargetDir +/+ "methods.js","w");
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
            progressWindow !? {
                progressWindow.userCanClose = true;
                closeButton.enabled = true;
            };
        };

        doWait = threaded or: gui;
        if(gui, {this.makeProgressWindow});
        if(doWait, {
            Routine(func).play(AppClock);
        }, func);
        
    }

    *checkBrokenLinks {
        var f,m;
        var file;
        var check_link = {|link|
            if(("^[a-zA-Z]+://.+".matchRegexp(link) or: (link.first==$/)).not) {
                f = link.split($#);
                m = this.helpTargetDir +/+ f[0] ++ ".html";
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
                        this.splitList(node.text).do {|l|
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
            var ext = source.copyToEnd(lastDot);
            if(ext == ".schelp", {
                file = source;
                p.parseFile(source);
                do_children.(p.root);
            });
        };
        postln("Done");
    }
    
    *findClassOrMethod {|str|
        ^ this.helpTargetDir +/+ if(str[0].isUpper,
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

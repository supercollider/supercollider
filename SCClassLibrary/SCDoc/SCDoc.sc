SCDocEntry {
    var <>path,     // document "path" without basedir and extension, like "Classes/SinOsc"
        <>title,
        <>summary,
        <>redirect,
        <>categories, // a list of categories
        <>related,    // a list of related docs
        <>docimethods, // a set of documented methods
        <>doccmethods, // a set of documented methods
        <>docmethods, // a set of documented methods
        <>privcmethods, // a set of private methods
        <>privimethods, // a set of private methods
        <>undoccmethods,
        <>undocimethods,
        <>keywords,   // a list of keywords
        <>additions,
        <>isExtension,
        <>mtime,
        <>fullPath,
        <>oldHelp;

    var <>isClassDoc;
    var <>klass, <>implKlass, <>implements;
    var <>isUndocumentedClass;

    printOn {|stream|
        stream << "SCDocEntry(" << path.cs << ", " << title.cs << ", " << summary.cs << ")";
    }

    prJSONString {|stream, key, x|
        if(x.isNil) { x = "" };
        stream << "'" << key << "': \"" << x.escapeChar(34.asAscii) << "\",\n";

    }
    prJSONList {|stream, key, v|
        if(v.isNil) { v = "" };
        stream << "'" << key << "': [ " << v.collect{|x|"\""++x.escapeChar(34.asAscii)++"\""}.join(",") << " ],\n";
    }

    toJSON {|stream|
        stream << "\"" << path.escapeChar(34.asAscii) << "\": {\n";
        this.prJSONString(stream, "title", title);
        this.prJSONString(stream, "path", path);
        this.prJSONString(stream, "summary", summary);
        this.prJSONString(stream, "installed", if(isExtension,"extension","standard")); //FIXME: also 'missing'.. better to have separate extension and missing booleans..
        this.prJSONString(stream, "categories",
            if(categories.notNil) {categories.join(", ")} {""}); // FIXME: export list instead
        this.prJSONList(stream, "keywords", keywords);
        this.prJSONList(stream, "related", related);
        this.prJSONList(stream, "methods", this.makeMethodList);
        if(oldHelp.notNil) {
            this.prJSONString(stream, "oldhelp", oldHelp);
        };
        if(klass.notNil) {
            klass.superclasses !? {
                this.prJSONList(stream, "superclasses", klass.superclasses.collect {|c|
                    c.name.asString
                })
            };
            klass.subclasses !? {
                this.prJSONList(stream, "subclasses", klass.subclasses.collect {|c|
                    c.name.asString
                })
            };
            implKlass !? {
                this.prJSONString(stream, "implementor", implKlass.name.asString);
            }
        };
        stream << "},\n";
    }

    *new {|node,path|
        ^super.new.init(node,path);
    }

    *newUndocClass {|name|
        var doc = super.new.init(nil,"Classes/"++name.asString);
        var f, cats, implements, c;
        doc.klass = name.asSymbol.asClass;
        doc.isClassDoc = true;
        doc.isUndocumentedClass = true;
        doc.title = name.asString;
        f = doc.klass.filenameSymbol.asString;
        doc.mtime = 0;
        doc.isExtension = f.beginsWith(Platform.classLibraryDir).not;
        doc.undoccmethods = doc.klass.class.methods.collectAs({|m|m.name.asGetter},IdentitySet);
        doc.undocimethods = doc.klass.methods.collectAs({|m|m.name.asGetter},IdentitySet);

        if((implements=doc.klass.tryPerform(\implementsClass)).class===Symbol) {
            (c = implements.asClass) !? {
                doc.implements = c;
                doc.summary = "Implements "++implements;
                doc.categories = ["Redirect Class Implementors"];
                ^doc;
            };
        };
        doc.summary = "(Undocumented class)";
        cats = ["Undocumented classes"];
        if(SCDoc.classHasArKrIr(doc.klass)) {
            cats = cats.add("UGens>Undocumented");
        };
        doc.klass.categories !? {
            cats = cats ++ doc.klass.categories;
        };
        doc.categories = cats;

        ^doc;
    }

    init {|node,aPath|
        var hdr, bdy;
        // 'path' variable is used as a key for SCDoc.documents dictionary.
        // Make sure it always uses forward slashes.
        // FIXME: implement & use a generic path conversion method?
        path = aPath.replace("\\","/");
        if(node.isNil) {^this};
        #hdr, bdy = node.children;
        isExtension = false;
        isUndocumentedClass = false;
        doccmethods = IdentitySet();
        docimethods = IdentitySet();
        docmethods = IdentitySet();
        privcmethods = IdentitySet();
        privimethods = IdentitySet();
        undoccmethods = IdentitySet();
        undocimethods = IdentitySet();
        hdr.children.do {|n|
            switch(n.id,
                \TITLE, { title = n.text },
                \CLASS, { title = n.text }, // not used anymore?
                \SUMMARY, { summary = n.text },
                \REDIRECT, { redirect = n.text },
                \CATEGORIES, { categories = n.children.collect {|child| child.text} },
                \RELATED, { related = n.children.collect {|child| child.text} },
            );
        };
        this.prScanMethodsKeywords(bdy);
        if(title.isNil) {
            warn("SCDoc:"+path+"has no title!");
            title = "(Untitled)";
        };
        if(isClassDoc = (path.dirname=="Classes")) {
            klass = title.asSymbol.asClass;
//            if(klass.isNil) {
//                warn("SCDoc:"+path++": No such class!");
//            };
            if(title != path.basename ) {
                warn("SCDoc:"+path++": Title and filename mismatch. Must be same for class docs!");
            };
        };
        if(categories.isNil) {
            warn("SCDoc:"+path+"has no categories!");
            categories = ["Uncategorized"];
        };
        if(summary.isNil) {
            warn("SCDoc:"+path+"has no summary!");
            summary = "(Missing summary)";
        };

    }

    destPath {
        ^SCDoc.helpTargetDir +/+ path ++ ".html";
    }

    makeMethodList {
        var list;
        //FIXME: no need for the extra _ char..
        docimethods.do {|name|
            list = list.add("_-"++name.asString);
        };
        doccmethods.do {|name|
            list = list.add("_*"++name.asString);
        };
        undocimethods.do {|name|
            list = list.add("?-"++name.asString);
        };
        undoccmethods.do {|name|
            list = list.add("?*"++name.asString);
        };
        docmethods.do {|name|
            list = list.add("_."++name.asString);
        };
        ^list;
    }

    setAdditions {|a|
        var x;
        additions = a;
        a.do {|f|
            (x = SCDoc.parseFilePartial(f)) !? {
                this.prScanMethodsKeywords(x);
            }
        }
    }

    indexUndocumentedMethods {
        var ignoreMethods = IdentitySet[\categories, \init, \checkInputs, \new1, \argNamesInputsOffset, \initClass, \storeArgs, \storeOn, \printOn];
        var syms, name, mets, l = Array.new;
        var docmets = IdentitySet.new;

        if(klass.isNil) {
            ^this
        };

        if(redirect.notNil) {
            try { implKlass = klass.perform(redirect.asSymbol) };
        } {
            implKlass = nil;
        };

        docmets = docimethods | privimethods | ignoreMethods;
        (mets = klass.methods) !? {
            mets.collectAs({|m|m.name.asGetter},IdentitySet).do {|name|
                if(docmets.includes(name).not) {
                    undocimethods = undocimethods.add(name);
                };
            };
        };

        docmets = doccmethods | privcmethods | ignoreMethods;
        (mets = klass.class.methods) !? {
            mets.collectAs({|m|m.name.asGetter},IdentitySet).do {|name|
                if(docmets.includes(name).not) {
                    undoccmethods = undoccmethods.add(name);
                };
            };
        };

    }

    prAddMethodNames {|node, list|
        node.children.do {|n|
            list = list.add(n.text.asSymbol);
        }
        ^list;
    }

    prAddCopyMethod {|node, list|
        ^list.add(node.text.split($ )[1].drop(1))
    }

    prScanMethodsKeywords {|node|
        if(node.isNil) {
//            warn("FIXME: for some reason prScanMethodsKeywords was called on a nil node")
            ^this;
        };
        switch(node.id,
            \METHOD, { docmethods = this.prAddMethodNames(node.children[0], docmethods) },
            \CMETHOD, { doccmethods = this.prAddMethodNames(node.children[0], doccmethods) },
            \IMETHOD, { docimethods = this.prAddMethodNames(node.children[0], docimethods) },

            \COPYMETHOD, { docmethods = this.prAddCopyMethod(node,docmethods) },
            \CCOPYMETHOD, { doccmethods = this.prAddCopyMethod(node,doccmethods) },
            \ICOPYMETHOD, { docimethods = this.prAddCopyMethod(node,docimethods) },

            \CPRIVATE, { privcmethods = this.prAddMethodNames(node, privcmethods) },
            \IPRIVATE, { privimethods = this.prAddMethodNames(node, privimethods) },
            \KEYWORD, {
                node.children.do {|n|
                    keywords = keywords.add(n.text);
                }
            },
            {
                node.children.do {|n|
                    this.prScanMethodsKeywords(n);
                }
            }
        );
    }
}

SCDocNode {
    var <>id, <>text, <>children, <>makeDiv, notPrivOnly, <>sort;

    printOn {|stream|
        stream << "SCDocNode(" << id << ", " << text.cs << ", " << children << ")";
    }

    findChild {|id|
        ^children.detect {|node| node.id===id}
    }

    notPrivOnly {
        if(notPrivOnly.isNil) {
            notPrivOnly = (children.detect {|x| x.id != \CPRIVATE and: {x.id != \IPRIVATE}}.notNil)
        };
        ^notPrivOnly
    }

    addDivAfter {|id, div, title, childs|
        var node = this.findChild(id);
        var mets = SCDocNode()
            .id_(\SUBSECTION)
            .text_(title)
            .children_(childs)
            .makeDiv_(div);
        if(node.isNil) { //no subtree, create one
            children = children.add(
                node = SCDocNode().id_(id)
            );
        };
        node.children = node.children.add(mets);
    }

    sortClassDoc {
        var x = 0;
        // FIXME: does this work correctly for prose before first section, etc?
        children.do {|n|
            switch(n.id,
                \DESCRIPTION,       { n.sort = 10 },
                \CLASSMETHODS,      { n.sort = 11 },
                \INSTANCEMETHODS,   { n.sort = 12 },
                \EXAMPLES,          { n.sort = 13 },
                \SECTION, { n.sort = 14 + x = x + 1 },
                { n.sort = x = x + 1 }
            );
        };
        children = children.sort {|a,b| a.sort<b.sort};
    }

    merge {|root2|
        var n;
        var sects = IdentitySet[\BODY,\CLASSMETHODS,\INSTANCEMETHODS,\SECTION,\SUBSECTION,\EXAMPLES];
        var do_children = {|dest,childs|
            var res;
            childs !? {
                childs.do {|node|
                    if(sects.includes(node.id)) {
                        n = dest.detect {|x| (x.id==node.id) and: {x.text==node.text}};
                        if(n.isNil) {
                            dest = dest.add(node);
                        } {
                            n.children = do_children.(n.children,node.children);
                        }
                    } {
                        dest = dest.add(node);
                    }
                }
            };
            dest;
        };
        children = do_children.(children,root2.children);
    }

/*    addNodeAfter {|id, node2|
        var node = this.findChild(id);
        if(node.isNil, { //no subtree, create one
            children = children.add(
                node = SCDocNode().id_(id)
            );
        });
        node.children = node.children.add(node2);
    }
*/
}

SCDoc {
    // Increment this whenever we make a change to the SCDoc system so that all help-files should be processed again
    classvar version = 56;

    classvar <helpTargetDir;
    classvar <helpTargetUrl;
    classvar <helpSourceDir;
    classvar <>verbosity = 1;
    classvar <>renderer;
    classvar documents;
    classvar helpSourceDirs;

    *parseFileFull {|path|
        ^this.prParseFile(path, 0)
    }
    *parseFilePartial {|path|
        ^this.prParseFile(path, 1)
    }
    *parseFileMetaData {|dir,path|
        var fullPath = dir +/+ path;
        var subpath = path.drop(-7);
        var entry, x = this.prParseFile(fullPath, 2);
        if(x.isNil) {^nil};
        entry = SCDocEntry(x, subpath);
        entry.isExtension = (dir != this.helpSourceDir);
        entry.fullPath = fullPath;
        entry.mtime = File.mtime(fullPath);

        if(dir.beginsWith(Platform.userExtensionDir +/+ "quarks")) {
            entry.categories = entry.categories ++ ["Quarks>"++dir.dirname.basename];
        };

        ^entry;
    }
    *prParseFile {|path, mode|
        _SCDoc_ParseFile
        ^this.primitiveFailed
    }
    *indexOldHelp {
        var f = {|x,cat="Old Helpfiles",indent=0|
            var a,b,doc;
            x.pairsDo {|k,v|
                if(v.isKindOf(Dictionary)) {
                    k = k.asString;
                    a = 0;
                    b = k.size-1;
                    while({ $[ == k[a]},{a=a+1});
                    while({ $] == k[b]},{b=b-1});
                    k = k.copyRange(a,b);
                    f.(v,cat++">"++k.asString,indent+1);
                } {
                    if(v.size>0) {
                        doc = SCDocEntry(nil,"Old Help"+/+v);
                        doc.oldHelp = URI.fromLocalPath(v).asString;
                        doc.title = v.basename;
                        doc.summary = "(not yet converted to new help format)";
                        doc.categories = [cat];
                        doc.isExtension = true;
                        SCDoc.documents[doc.path] = doc;
                    }
                }
            }
        };
        Help.rebuildTree;
        f.(Help.tree);
    }
    *indexAllDocuments { |clearCache=false|
        var now = Main.elapsedTime;
        var key, doc;
        var nonHelpFiles;
        var undocClasses = Class.allClasses.reject(_.isMetaClass).collectAs({|c|c.name},IdentitySet);
        var additions = Dictionary();
        this.checkVersion(clearCache);
        this.postMsg("Indexing help-files...",0);
        documents = Dictionary(); // or use IdDict and symbols as keys?
        helpSourceDirs = nil; // force re-scan of HelpSource folders
        this.helpSourceDirs.do {|dir|
            PathName(dir).filesDo {|f|
                case
                {f.fullPath.endsWith(".ext.schelp")} {
                    f = f.fullPath;
                    key = f[dir.size+1 ..].drop(-11).replace("\\","/");
                    additions[key] = additions[key].add(f);
                }
                {f.extension=="schelp"} {
                    doc = this.parseFileMetaData(dir, f.fullPath.drop(dir.size+1));
                    doc !? {
                        documents[doc.path] = doc;
                        if(doc.isClassDoc) {
                            undocClasses.remove(doc.title.asSymbol);
                        }
                    }
                }
                {
                    f = f.fullPath;
                    nonHelpFiles = nonHelpFiles.add([f,f.drop(dir.size+1)]);
                };
            }
        };
        this.postMsg("Handling"+additions.size+"document additions...",1);
        additions.pairsDo {|key, val|
            doc = documents[key];
            if(doc.notNil) {
                doc.setAdditions(val);
            } {
                warn("SCDoc: Additions % for non-existent help file".format(val));
            }
        };
        this.postMsg("Indexing undocumented methods...",1);
        documents.do {|d|
            if(d.isClassDoc) { d.indexUndocumentedMethods };
        };
        this.postMsg("Adding entries for"+undocClasses.size+"undocumented classes...",1);
        undocClasses.do {|x|
            doc = SCDocEntry.newUndocClass(x);
            documents[doc.path] = doc;
        };
        this.postMsg("Copying"+nonHelpFiles.size+"non-help files...",1);
        nonHelpFiles.do {|x|
            var dest = SCDoc.helpTargetDir+/+x[1];
            var folder = dest.dirname;
            File.mkdir(folder);
            if(File.exists(dest).not or: {File.mtime(x[0]) > File.mtime(dest)}) {
                File.delete(dest);
                File.copy(x[0],dest);
            };
        };
        this.postMsg("Indexing old helpfiles...");
        this.indexOldHelp;
        this.postMsg("Exporting docmap.js...",1);
        this.exportDocMapJS(this.helpTargetDir +/+ "docmap.js");
        this.postMsg("Indexed % documents in % seconds".format(documents.size,round(Main.elapsedTime-now,0.01)),0);
        NotificationCenter.notify(SCDoc, \didIndexAllDocs);
    }
    *didIndexDocuments {
        ^documents.notNil
    }
    *documents {
        if(documents.isNil) {
            this.indexAllDocuments;
        };
        ^documents;
    }
    *helpSourceDirs {
        var find;
        if(helpSourceDirs.isNil) {
            this.postMsg("locating HelpSource folders...",2);
            helpSourceDirs = [helpSourceDir]; // Note: an array will keep the order.
            find = {|dir|
                dir.folders.do {|f|
                    if(f.folderName=="HelpSource") {
                        helpSourceDirs = helpSourceDirs.add(f.fullPath.withoutTrailingSlash);
                    } {
                        find.(f);
                    };
                }
            };
            [thisProcess.platform.userExtensionDir, thisProcess.platform.systemExtensionDir].do {|dir|
                find.(PathName(dir));
            };
       };
       ^helpSourceDirs
    }
    *exportDocMapJS {|path|
        var f = File.open(path,"w");
        f << "docmap = {\n";
        this.documents.do {|doc|
            doc.toJSON(f);
        };
        f << "}\n";
        f.close;
    }

    *helpSourceDir_ {|path|
        helpSourceDir = path.standardizePath;
    }

    *helpTargetDir_ {|path|
//        if(path!=helpTargetDir) {didRun = false};
        helpTargetDir = path.standardizePath;
        helpTargetUrl = URI.fromLocalPath(helpTargetDir).asString;
    }

    *postMsg {|txt, lvl=0|
        if(verbosity>lvl) {
            postln("SCDoc: "++txt);
        };
        if(thisThread.isKindOf(Routine)) { 0.yield; }
    }

    *parseDoc {|doc|
        var add, root;
        (root = this.parseFileFull(doc.fullPath)) !? {
            doc.additions.do {|f|
                (add = this.parseFilePartial(f)) !? {
                    root.children[1].merge(add);
                }
            };
            this.handleCopyMethods(root,doc);
        };
        ^root;
    }

    *parseAndRender {|doc|
        var dest = doc.destPath;
        var root = this.parseDoc(doc);
        root !? {
            this.postMsg("% -> %".format(doc.fullPath, dest),2);
            this.renderer.renderToFile(dest, doc, root);
        }
    }

    *prepareHelpForURL {|url|
        var path, targetBasePath, pathIsCaseInsensitive;
        var subtarget, src, c, cmd, doc, destExist, destMtime;
        var verpath = this.helpTargetDir +/+ "version";

        path = url.asLocalPath;

        // detect old helpfiles and wrap them in OldHelpWrapper
        if(url.scheme == "sc") { ^URI(SCDoc.findHelpFile(path)); };

        // just pass through remote url's
        if(url.scheme != "file") {^url};

        targetBasePath = SCDoc.helpTargetDir;
        if (thisProcess.platform.name === \windows)
            { targetBasePath = targetBasePath.replace("/","\\") };
        pathIsCaseInsensitive = thisProcess.platform.name === \windows;

        // detect old helpfiles and wrap them in OldHelpWrapper
        if(
            /*
            // this didn't work for quarks due to difference between registered old help path and the quarks symlink in Extensions.
            // we could use File.realpath(path) below but that would double the execution time,
            // so let's just assume any local file outside helpTargetDir is an old helpfile.
            block{|break|
            Help.do {|key, path|
            if(url.endsWith(path)) {
            break.value(true)
            }
            }; false
            }*/
            compare(
                path [..(targetBasePath.size-1)],
                targetBasePath,
                pathIsCaseInsensitive
            ) != 0
        ) {
            ^SCDoc.getOldWrapUrl(url)
        };

        if(destExist = File.exists(path))
        {
            destMtime = File.mtime(path);
        };

        if(path.endsWith(".html")) {
            subtarget = path.drop(this.helpTargetDir.size+1).drop(-5).replace("\\","/");
            doc = this.documents[subtarget];
            doc !? {
                if(doc.isUndocumentedClass) {
                    if(doc.mtime == 0) {
                        this.renderUndocClass(doc);
                        doc.mtime = 1;
                    };
                    ^url;
                };
                if(File.mtime(doc.fullPath)>doc.mtime) { // src changed after indexing
                    this.postMsg("% changed, re-indexing documents".format(doc.path),2);
                    this.indexAllDocuments;
                    ^this.prepareHelpForURL(url);
                };
                if(destExist.not
                    or: {doc.mtime>destMtime}
                    or: {doc.additions.detect {|f| File.mtime(f)>destMtime}.notNil}
                    or: {File.mtime(this.helpTargetDir +/+ "scdoc_version")>destMtime}
                    or: {doc.klass.notNil and: {File.mtime(doc.klass.filenameSymbol.asString)>destMtime}}
                ) {
                    this.parseAndRender(doc);
                };
                ^url;
            };
        };

        if(destExist) {
            ^url;
        };

        warn("SCDoc: Broken link:" + url.asString);
        ^nil;
    }

    *initClass {
        this.helpSourceDir_(thisProcess.platform.classLibraryDir.dirname +/+ "HelpSource");
        this.helpTargetDir_(thisProcess.platform.userAppSupportDir +/+ "Help");
        renderer = SCDocHTMLRenderer;
    }

    *classHasArKrIr {|c|
        ^#[\ar,\kr,\ir].collect {|m| c.class.findRespondingMethodFor(m).notNil }.reduce {|a,b| a or: b};
    }

    *checkVersion {|clearCache=false|
        var f, path = this.helpTargetDir +/+ "scdoc_version";
        if(clearCache or: {path.load != version}) {
            this.postMsg("refreshing scdoc version timestamp",1);
            // this will update the mtime of the version file, triggering re-rendering of files older than now
            File.mkdir(this.helpTargetDir);
            f = File.open(path,"w");
            f.write(version.asCompileString);
            f.close;
            ^true;
        };
        ^false;
    }

    *renderAll {|includeExtensions=true|
        this.postMsg("Rendering all documents");
        this.documents.do {|doc|
            if(doc.oldHelp.isNil and: {includeExtensions or: {doc.isExtension.not}}) {
                if(doc.isUndocumentedClass) {
                    this.renderUndocClass(doc);
                } {
                    this.parseAndRender(doc);
                }
            }
        };
        this.postMsg("Done!");
    }

    *makeClassTemplate {|doc|
        var name = doc.title;
        var cats = doc.categories;
        var class = doc.klass;
        var n, m, f, c;

        f = {|cm|
            var txt,c,m,l,last,sym;
            if(cm) {
                txt = "\nCLASSMETHODS::\n\n";
                n = doc.undoccmethods;
                c = class.class;
            } {
                txt = "\nINSTANCEMETHODS::\n\n";
                n = doc.undocimethods;
                c = class;
            };
            n.do {|x|
                txt = txt ++ "METHOD::" + x ++ "\n(describe method here)\n\n";
                sym = x.asSymbol;
                m = c.findRespondingMethodFor(sym.asSetter);
                m = m ?? {c.findRespondingMethodFor(sym)};
                m !? {
                    l = m.argNames;
                    last = l.size-1;
                    l.do {|a,i|
                        if (i>0) { //skip 'this' (first arg)
                            txt = txt ++ "ARGUMENT:: ";
                            if(i==last and: {m.varArgs}) {
                                txt = txt ++ " ... ";
                            };
                            txt = txt ++ a ++ "\n(describe argument here)\n\n";
                        }
                    }
                };
                txt = txt ++ "returns:: (describe returnvalue here)\n\n";
            };
            txt;
        };

        ^ "TITLE::"+name
            ++"\nsummary:: (put short description here)\n"
            ++"categories::"+cats.join(", ")
            ++"\nrelated:: Classes/SomeRelatedClass, Reference/SomeRelatedStuff, etc.\n\n"
            ++"DESCRIPTION::\n(put long description here)\n\n"
            ++ f.(true) ++ f.(false)
            ++"\nEXAMPLES::\n\ncode::\n(some example code)\n::\n";
     }

    *renderUndocClass {|doc|
        var node, desc, body;

        node = SCDocNode().id_(\DOCUMENT).children_([
            SCDocNode().id_(\HEADER).children_([
                // the header content is already in the SCDocEntry..
            ]),
            body = SCDocNode().id_(\BODY).children_([
                SCDocNode().id_(\DESCRIPTION).children_([
                    desc = SCDocNode().id_(\PROSE)
                ])
            ])
        ]);

        if(doc.implements.notNil) {
            this.postMsg("Generating class redirect implementor doc: % for %".format(doc.title,doc.implements.name),2);
            desc.children = [
                SCDocNode().id_(\TEXT).text_("Implements "),
                SCDocNode().id_(\LINK).text_("Classes/"++doc.implements.name)
            ];
        } {
            this.postMsg("Undocumented class: "++doc.title++", generating stub and template",2);
            desc.children = [
                SCDocNode().id_(\TEXT).text_("This class is missing documentation.")
            ];
            body.children = body.children.add(
                SCDocNode().id_(\SECTION).text_("Help template").children_([
                    SCDocNode().id_(\PROSE).children_([
                        SCDocNode().id_(\TEXT).text_("Copy and paste the text below and save to HelpSource/Classes/"++doc.title++".schelp")
                    ]),
                    SCDocNode().id_(\CODEBLOCK).text_(this.makeClassTemplate(doc))
                ])
            );
        };

        this.renderer.renderToFile(doc.destPath, doc, node);
    }


    *getMethodDoc {|classname,methodname|
        var doc, id, node, mname;

        var findmet = {|n|
            if((n.id == id) and: {n.children[0].children.detect{|x|x.text==mname}.notNil}) {
                n;
            } {
                block {|break|
                    n.children.do {|n2|
                        n2 = findmet.(n2,id);
                        if(n2.notNil) {
                            break.value(n2);
                        }
                    };
                    nil;
                }
            }
        };

        var err = {|txt|
            warn("SCDoc.getMethodDoc(%, %): %".format(classname,methodname,txt));
        };

        doc = this.documents["Classes/"++classname];
        if(doc.isNil or: {doc.fullPath.isNil}) {
            err.("class document not found");
            ^nil;
        };
        id = switch(methodname[0],
            $*, \CMETHOD,
            $-, \IMETHOD,
            $., \METHOD,
            {
                err.("methodname must be prefixed with '*', '-' or '.'");
                ^nil;
            }
        );
        mname = methodname.drop(1);
        node = this.parseDoc(doc);
        if(node.isNil) {
            err.("could not parse class document");
            ^nil;
        };
        node = findmet.(node);
        if(node.isNil) {
            err.("method not found");
            ^nil;
        };
        ^node;
    }

    *handleCopyMethods {|node,doc|
        var found = {|n|
            var name, met, x;
            #name, met = n.text.findRegexp("[^ ,]+").flop[1];
            x = this.getMethodDoc(name, met);
            if(x.isNil) {
                warn("  from: %".format(doc.fullPath));
            };
            x;
        };

        node.children.do{|n,i|
            switch(n.id,
                \CCOPYMETHOD, { n = found.(n); n !? {n.id_(\CMETHOD); node.children[i] = n} },
                \ICOPYMETHOD, { n = found.(n); n !? {n.id_(\IMETHOD); node.children[i] = n} },
                \COPYMETHOD, { n = found.(n); n !? {n.id_(\METHOD); node.children[i] = n} },
                {
                    this.handleCopyMethods(n,doc);
                }
            );
        };
    }

    *findHelpFile {|str|
        var old, sym, pfx = SCDoc.helpTargetUrl;

        if(str.isNil or: {str.isEmpty}) { ^pfx ++ "/Help.html" };
        if(this.documents[str].notNil) { ^pfx ++ "/" ++ str ++ ".html" };

        sym = str.asSymbol;
        if(sym.asClass.notNil) {
            ^pfx ++ (if(this.documents["Classes/"++str].isUndocumentedClass) {
                (old = Help.findHelpFile(str)) !? {
                    "/OldHelpWrapper.html#"++old++"?"++SCDoc.helpTargetUrl ++ "/Classes/" ++ str ++ ".html"
                }
            } ?? { "/Classes/" ++ str ++ ".html" });
        };

        if(str.last == $_) { str = str.drop(-1) };
        ^pfx ++ if("^[a-z][a-zA-Z0-9_]*$|^[-<>@|&%*+/!?=]+$".matchRegexp(str))
            { "/Overviews/Methods.html#" } { "/Search.html#" } ++ str;
    }

    *getOldWrapUrl {|url|
        var urlString, className, newUrl;
        urlString = url.asString;
        newUrl = URI.fromLocalPath( SCDoc.helpTargetDir +/+ "OldHelpWrapper.html" );
        newUrl.fragment = urlString;
        newUrl.query =
            SCDoc.helpTargetUrl
            ++ if((className=urlString.basename.split($.).first).asSymbol.asClass.notNil)
                {"/Classes/" ++ className ++ ".html"}
                {"/Guides/WritingHelp.html"}
        ^newUrl;
    }
}

URI {
    /*
    NOTE:
    This class attempts compliance with specification
    Uniform Resource Identifier (URI): Generic Syntax (RFC 3986)
    http://datatracker.ietf.org/doc/rfc3986/

    If you intend to modify it, please consult the specification!
    */

    classvar parseRegexp = "^(([^:/?#]+):)?(//([^/?#]*))?([^?#]*)(\\?([^#]*))?(#(.*))?";

    var <>scheme, <>authority, <>path, <>query, <>fragment;

    *new { |validUriString|
        ^super.new.init(validUriString)
    }

    *fromLocalPath { |string|
        var uri = super.new;
        uri.scheme = "file";
        uri.authority = "";
        uri.path = string;
        if (thisProcess.platform.name === \windows) {
            uri.path = uri.path.replace("\\","/");
            if (uri.path.size >= 2 and: {uri.path[1] == $:})
            { uri.path = "/" ++ uri.path; }
        }
        ^ uri;
    }

    *tolerant { |string|
        var uri;

        if (thisProcess.platform.name === \windows
            and: { string.size >= 2 and: { string[1] == $:} } )
        {
            ^ this.fromLocalPath(string);
        };

        uri = this.new(string);
        if (uri.scheme.isNil) {
            uri.scheme = "file";
            if (uri.authority.isNil) { uri.authority = "" }
        };
        ^ uri;
    }

    init { |string|
        var result;
        if (string.notNil) {
            string = string.replace("%20"," ");
            result = string.findRegexp( parseRegexp ).flop[1];
            if (result[1].size > 0) { scheme = result[2] };
            if (result[3].size>0) { authority = result[4] };
            path = result[5];
            if (result[6].size > 0) { query = result[7] };
            if (result[8].size > 0) { fragment = result[9] };
        };
    }

    asLocalPath {
        var localPath;
        if (scheme != "file") { ^nil };
        if (thisProcess.platform.name === \windows) {
            localPath = path;
            if (localPath.beginsWith("/")) { localPath = localPath.drop(1) };
            localPath = localPath.replace("/","\\");
            ^localPath;
        }
        ^ path.copy;
    }

    asString {
        var str = "";
        if (scheme.notNil) { str = str ++ scheme ++ ":" };
        if (authority.notNil) { str = str ++ "//" ++ authority };
        str = str ++ path;
        if (query.notNil) { str = str ++ "?" ++ query };
        if (fragment.notNil) { str = str ++ "#" ++ fragment };
        ^str;
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

SCDocParser {
    var <>root;
    var tree;
    var stack;
    var current;
    var singleline;
    var level;
    var modalTag;
    var lastTagLine;
    var afterClosing;
    var isWS;
    var stripFirst;
    var proseDisplay;
    var <>currentFile;
    var <methodList, <keywordList;
    classvar copyMethodCache;

    init {
        root = tree = List.new;
        stack = List.new;
        stack.add([tree,0,nil]);
        current = nil;
        singleline = false;
        level = 0;
        modalTag = nil;
        isWS = false;
        afterClosing = false;
        stripFirst = false;
        proseDisplay = \block;
//        doingInlineTag = false;
    }

//    *new {|filename|
//        ^super.newCopyArgs(filename).init;
//    }
//    *new {
//        ^super.new.init;
//    }
//    isTag {|word| ^"^(::[a-zA-Z]+|[a-zA-Z]+::)$".matchRegexp(word)}
//    isOpeningTag {|word| ^"^[a-zA-Z]+::$".matchRegexp(word)}
//    isClosingTag {|word| ^"^::[a-zA-Z]+$".matchRegexp(word)}
    leaveLevel {|n|
        var p;
        while({level>=n},{
            p = stack.pop;
            tree = p[0];
            level = p[1];
        });
    }

    popTree {
        var p = stack.pop;
        p !? {
            tree = p[0];
            level = p[1];
            p[2] !? {proseDisplay = p[2].display};
        };
    }

    pushTree {
        stack.add([tree,level,nil]);
    }

    setTopNode {|n|
        stack[stack.size-1][2] = n;
    }

    enterLevel {|n|
        this.leaveLevel(n);
        this.pushTree;
        level = n;
    }

    endCurrent {
        current !? {
            proseDisplay = current.display;
            current = nil;
        };
    }

    addTag {|tag, text="", children=false, display=\block|
        var node;
        this.endCurrent;
        tag = tag.asString.drop(-2).asSymbol;
        current = node = (tag:tag, display:display, text:text, children:if(children,{List.new},{nil}));
        tree.add(current);
        if(children, {tree = current.children}); //recurse into children list
        if(text.isNil) {this.endCurrent}; //we don't have any text field to add to for this tag, so start fresh..
        ^node;
    }

    handleWord {|word,lineno,wordno|
        var tag = word.toLower.asSymbol;
        var t,x;
        var simpleTag = {
            singleline = true;
            this.addTag(tag);
            stripFirst = true;
        };
        var noNameSection = {
            singleline = true; //this doesn't actually matter here since we don't have a text field?
            this.enterLevel(1);
            this.setTopNode(this.addTag(tag,nil,true));
        };
        var namedSection = {|lev|
            {
                singleline = true;
                this.enterLevel(lev);
                this.setTopNode(this.addTag(tag,"",true));
                stripFirst = true;
            }
        };
        var modalRangeTag = {
            singleline = false;
            if(current.isNil) {
                tree.add((tag:\prose,display:proseDisplay,text:"",children:nil));
            };
            this.addTag(tag);
            lastTagLine = lineno;
            modalTag = '::';
        };
        var listEnter = {
            singleline = false; //this doesn't actually matter here since we don't have a text field?
            this.pushTree;
            this.setTopNode(this.addTag(tag,nil,true));
            lastTagLine = lineno;
        };

        // modal tags ignore all other tags until their closing tag occurs.
        // here we check if we are in a modal tag (code, emphasis, link, teletype) and then
        // if we got the closing tag.
        if(modalTag.notNil, {
            //only allow modal block tags to be closed with the closing tag as the first word on a line
            if((tag==modalTag) and: ((wordno==0) or: (lastTagLine==lineno)),{
                current !? {
                    current.display = if(lastTagLine==lineno,\inline,\block);
                };
                this.endCurrent;
                modalTag = nil;
                afterClosing = true;
            },{
                if(("[^\\\\]::$".matchRegexp(word)) and: (lastTagLine==lineno), { //split unhandled tag-like word
                    this.addText(word.drop(-2));
                    this.handleWord("::",lineno,wordno+1);
                },{
                    this.addText(word.replace("\\::","::"));
                });
            });
        },{
            switch(tag,
                'description::',        noNameSection, //level 1
                'classmethods::',       noNameSection,
                'instancemethods::',    noNameSection,
                'examples::',           noNameSection,
                'section::',            namedSection.(1),
                'subsection::',         namedSection.(2),
                'method::',             namedSection.(3),
                'copymethod::',         {
                    this.enterLevel(3);
                    simpleTag.value;
                },
                'keyword::',            simpleTag,
                'argument::',           namedSection.(4),
                'returns::',            {
                    singleline = true; //this doesn't actually matter here since we don't have a text field?
                    this.enterLevel(4);
                    this.setTopNode(this.addTag(tag,nil,true));
                },
                'discussion::',            {
                    singleline = true; //this doesn't actually matter here since we don't have a text field?
                    this.enterLevel(4);
                    this.setTopNode(this.addTag(tag,nil,true));
                },
                'class::',              simpleTag,
                'redirect::',           simpleTag,
                'title::',              simpleTag,
                'summary::',            simpleTag,
                'related::',            simpleTag,
//                'headerimage::',        simpleTag,
                'categories::',         simpleTag,
//                'note::',               simpleTag,
//                'warning::',            simpleTag,
                'private::',            simpleTag,
                'classtree::',          simpleTag,

                'code::',               modalRangeTag,
                'formula::',            modalRangeTag,
                'emphasis::',           modalRangeTag,
                'teletype::',           modalRangeTag,
                'strong::',             modalRangeTag,
                'link::',               modalRangeTag,
                'anchor::',             modalRangeTag,
                'image::',              modalRangeTag,
                'soft::',               modalRangeTag,

                'note::',               { listEnter.(); proseDisplay=\inline },
                'warning::',            { listEnter.(); proseDisplay=\inline },

                'list::',               listEnter,
                'tree::',               listEnter,
                'numberedlist::',       listEnter,
                'definitionlist::',     listEnter,
                'table::',              listEnter,
                'footnote::',           {
                    singleline = false;
                    current !? { //strip trailing whitespace from previous text..
                        t=current.text;
                        x=t.size-1;
                        while({(t[x]==$\n) or: (t[x]==$\ )},{x=x-1});
                        current.text = t.copyRange(0,x);
                    };
                    this.pushTree;
                    this.setTopNode(this.addTag(tag,nil,true,\inline));
                    lastTagLine = lineno;
                    proseDisplay = \inline;
                },
                '##', {
                    singleline = false;
                    this.addTag('##::',nil,false,\inline); //make it look like an ordinary tag since we drop the :: in the output tree
                },
                '||', {
                    singleline = false;
                    this.addTag('||::',nil,false,\inline);
                },
                '\\||', {
                    this.addText("||");
                },
                '::', { //ends tables and lists
                    this.endCurrent;
                    this.popTree;
                },
                '\\::', {
                    this.addText("::");
                },

                { //default case
                    if("^[a-zA-Z]+://.+".matchRegexp(word),{ //auto link URIs
                        this.addTag('link::',word++" ",false,\inline);
                        this.endCurrent;
                    },{
                        if(("[^\\\\]::$".matchRegexp(word)) and: (lastTagLine==lineno), { //split unhandled tag-like word
                            this.addText(word.drop(-2));
                            this.handleWord("::",lineno,wordno+1);
                        },{
                            this.addText(word); //plain text, add the word.
                        });
                    });
                }
            );
        });
    }

    addText {|word|
        if(stripFirst, {
            stripFirst = false;
            word = word.stripWhiteSpace;
        });
        if(current.notNil, { // add to current element text
            current.text = current.text ++ word
        },{ // no current element, so add to new 'prose' element
            if((isWS.not) or: (afterClosing), { //don't start a new prose element with whitespace
                afterClosing = false;
                singleline = false;
                this.addTag('prose::', word, false, proseDisplay);
            });
        });
    }

    endLine {
        if(singleline,{this.endCurrent});
        // pass through newlines for vari-line tags.
        current !? {current.text = current.text ++ "\n"};
    }

    parse {|string|
        var lines = string.replace("\r","").split($\n); //split lines
//        var lines = string.findRegexp("[^\n]+").flop[1]; //doesn't work for empty lines

        var w, split, split2, word;
        this.init;
        lines.do {|line,l|
            split = line.findRegexp("\\S+::\\S+::|\\S+::|::|\\s+|\\S+");
            w = 0;
            split.do {|e|
                word = e[1];
                split2 = word.findRegexp("([a-zA-Z]+::)(\\S+)(::)")[1..]; //split stuff like::this::...
                if(split2.isEmpty,{
                    isWS = "^\\s+$".matchRegexp(word);
                    this.handleWord(word,l,w);
                    if(isWS.not,{w=w+1});
                },{
                    split2.do {|e2|
                        isWS = "^\\s+$".matchRegexp(e2[1]);
                        this.handleWord(e2[1],l,w);
                        w=w+1;
                    };
                });
            };
            if(modalTag.isNil and: split.isEmpty, { this.endCurrent; proseDisplay=\block; }); //force a new prose on double blank lines
            this.endLine;
        };
        this.handleCopyMethod;
    }

    parseFile {|filename|
        var file = File.open(filename,"r");
        currentFile = filename;
        this.parse(file.readAllString);
        file.close;
    }

    merge {|p2|
        var n;
        var sects = IdentitySet[\classmethods,\instancemethods,\section,\subsection,\examples];
        var do_children = {|dest,children|
            var res;
            children !? {
                children.do {|node|
                    if(sects.includes(node.tag)) {
                        n = dest.detect {|x| (x.tag==node.tag) and: {x.text==node.text}};
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
        root = do_children.(root,p2.root);
    }

    parseMetaData {|path|
        var line, file, tag, text, match;
        var tags = IdentitySet[\class,\title,\summary,\categories,\related,\redirect];
        var pfx = ".";
        var methods = Array.new;
        var keywords = Array.new;
        var inHeader = true;
        var class = nil;
        var cmets = IdentitySet.new;
        var imets = IdentitySet.new;
        var m, mets, f, l;

        this.init;
        file = File.open(path,"r");
        block {|break|
            loop {
                line = file.getLine;
                if(line.isNil) {break.value};
                match = line.findRegexp("([a-zA-Z]+::)\\s*(\\S.*\\S|\\S)?").flop[1];
                if(match.notNil) {
                    tag = match[1].toLower.drop(-2).asSymbol;
                    text = match[2];
                    if(inHeader and: {tags.includes(tag)}) {
                        root.add((tag:tag, text:text));
                        if(tag==\class) {
                            class = text.asSymbol.asClass;
                        };
                    } {
                        inHeader = false;
                        switch(tag,
                            \description,       {pfx="."},
                            \section,           {pfx="."},
                            \examples,          {pfx="."},
                            \instancemethods,   {pfx="-"},
                            \classmethods,      {pfx="*"},
                            \method, {
                            //FIXME:
                            // - m.isExtensionOf(c) (perhaps not very important, we can see this in the class doc)
                                match = text.findRegexp("\\(.*\\)|[^ ,]+").flop[1].reject{|x|x[0]==$(};
                                match.do {|name|
                                    if("^[a-z][a-zA-Z0-9_]*|[-<>@|&%*+/!?=]+$".matchRegexp(name).not) {
                                        warn("Methodname not valid: '"++name++"' in"+path);
                                    } {
                                        m = name.asSymbol;
                                        if(m.isSetter) {
                                            warn("Methods should be given as getters, without the _ suffix:"+name+"in"+path);
                                            m = m.asGetter;
                                        };
                                        methods = methods.add("_"++pfx++m);
                                        switch(pfx,
                                            "*", {cmets.add(m)},
                                            "-", {imets.add(m)}
                                        );
                                    };
                                };
                            },
                            \copymethod, {
                                match = text.findRegexp("[^ ,]+").flop[1][1];
                                if(match.notNil) {
                                    m = match.drop(1).asSymbol.asGetter;
                                    methods = methods.add("_"++pfx++m);
                                    switch(pfx,
                                        "*", {cmets.add(m)},
                                        "-", {imets.add(m)}
                                    );
                                }
                            },
                            \private, {
                                match = text.findRegexp("[^ ,]+").flop[1];
                                match.do {|name|
                                    m = name.asSymbol.asGetter;
                                    switch(pfx,
                                        "*", {cmets.add(m)},
                                        "-", {imets.add(m)}
                                    );
                                };
                            },
                            \keyword, {
                                match = text.findRegexp("[^ ,]+").flop[1];
                                keywords = keywords ++ match;
                            }
                        );
                    };
                };
            };
        };
        file.close;
        // Add undocumented methods
        if(class.notNil) {
            f = {|c,docmets,pfx|
                l = Array.new;
                (mets = c.methods) !? {
                    //ignore these methods by default. Note that they can still be explicitly documented.
                    docmets = docmets | IdentitySet[\categories, \init, \checkInputs, \new1, \argNamesInputsOffset];
                    mets.collectAs({|m|m.name.asGetter},IdentitySet).do {|name|
                        if(docmets.includes(name).not) {
                            l = l.add("_"++pfx++name.asString);
                        }
                    };
                };
                l;
            };
            methods = methods ++ f.(class,imets,"-") ++ f.(class.class,cmets,"*");
        };
        methodList = methods;
        keywordList = keywords;
    }

    *getMethodDoc {|classname,methodname|
        var a, p, src, node = nil, findparent, parent;
        var findmet = {|children|
            children !? {
                children.do {|n|
                    switch(n.tag,
                        \instancemethods, {parent = n.tag},
                        \classmethods, {parent = n.tag},
                        \method, {
                            if(parent == findparent
                            and: {n.text.findRegexp("[^ ,]+").flop[1].indexOfEqual(methodname).notNil}) {
                                node = n;
                            }
                        }
                    );
                    if(n.children.size>0 and: {node.isNil}) { findmet.(n.children) };
                }
            }
        };

        if(copyMethodCache.isNil) {
            copyMethodCache = Array.new(4); //create an empty cache with room for 4 class docs
        };
        a = copyMethodCache.detect{|a|a.key==classname}; // check if class was already in cache
        if(a.notNil) {
            p = a.value;
        } {
            block {|break|
                src = nil;
                SCDoc.helpSourceDirs.do {|dir|
                    var x = dir+/+"Classes"+/+classname++".schelp";
                    if(File.exists(x)) {
                        src = x;
                        break.value;
                    };
                };
            };
            if(src.isNil) {
                warn("SCDoc: copymethod:: could not find class doc"+classname);
                ^nil;
            };
            p = SCDocParser.new;
            p.parseFile(src);
            // add to cache, possibly removing the oldest one
            if(copyMethodCache.size >= copyMethodCache.maxSize) {
                copyMethodCache.removeAt(0);
            };
            copyMethodCache.add(classname -> p);
        };

        findparent = switch(methodname[0],
            $*, \classmethods,
            $-, \instancemethods);
        methodname = methodname.drop(1);

        findmet.(p.root);
        if(node.isNil) {
            warn("SCDoc: copymethod:: could not find"+methodname+"in"+findparent+"of class doc"+classname);
        };
        ^node;
    }

    handleCopyMethod {
        var name, met, node;
        var do_children = {|children|
            children !? {
                children.do {|n,i|
                    if(n.tag==\copymethod) {
                        #name, met = n.text.findRegexp("[^ ,]+").flop[1];
                        node = this.class.getMethodDoc(name,met);
                        if(node.notNil) {
                            children[i]=node;
                        } {
                            children[i]=(tag:\method, text:met.drop(1), children:[(tag:\prose, text:"(copymethod::"+n.text+"failed)", display:\block)]);
                        };
                    };
                    if(n.children.size>0) { do_children.(n.children) };
                };
            };
        };
        do_children.(root);
    }

    // FIXME: move to renderer?
    generateUndocumentedMethods {|class,node,title|
        var syms, name, mets, l = Array.new;
        var docmets = IdentitySet.new;

        var addMet = {|n|
            n.text.findRegexp("\\(.*\\)|[^ ,]+").flop[1].do {|m|
                if(m[0] != $() {
                    docmets.add(m.asSymbol.asGetter);
                };
            };
        };

        var do_children = {|children|
            children !? {
                children.do {|n|
                    switch(n.tag,
                        \method, { addMet.(n) },
                        \private, { addMet.(n) },
                        \subsection, { do_children.(n.children) }
                    );
                };
            };
        };

        if(class.isNil, {^nil});

        do_children.(node.children);

        (mets = class.methods) !? {
            //ignore these methods by default. Note that they can still be explicitly documented.
            docmets = docmets | IdentitySet[\categories, \init, \checkInputs, \new1, \argNamesInputsOffset];
            syms = mets.collectAs({|m|m.name.asGetter},IdentitySet);
            syms.do {|name|
                if(docmets.includes(name).not) {
                    l = l.add((tag:\method, text:name.asString));
                }
            };
        };

        ^ if(l.notEmpty,
        {
            (tag:\subsection,
            text:title,
            children:l)
        },
            nil
        );
    }

    dumpSubTree {|t,i="",lev=1|
        t.do {|e|
            "".postln;
            (i++"TAG:"+e.tag+"( level"+lev+e.display+")").postln;
            e.text !? {
                (i++"TEXT: \""++e.text++"\"").postln;
            };
            e.children !? {
                (i++"CHILDREN:").postln;
                this.dumpSubTree(e.children,i++"    ",lev+1);
            };
        }
    }

    dump {
        this.dumpSubTree(root);
        ^nil;
    }

    findNode {|tag,rootNode=nil|
        var sym = tag.asSymbol;
        ^((rootNode ?? { root }).detect {|n| n.tag == tag} ?? {
            (tag:nil, text:"", children:[]);
        });
    }

/*
    makeCategoryTree {|catMap,node,filter=nil,toc=false|
        var a, p, e, n, l, m, kinds, folder, v, dumpCats, sorted;
        var tree = Dictionary.new;

        catMap.pairsDo {|cat,files|
            p=tree;
            l=cat.split($>);
            if(filter.isNil or: {filter.matchRegexp(l.first)}, {
                l.do {|c|
                    if(p[c].isNil,{
                        p[c]=Dictionary.new;
                        p[c][\subcats] = Dictionary.new;
                        p[c][\entries] = List.new;
                    });
                    e=p[c];
                    p=p[c][\subcats];
                };
                a=e[\entries];
                files.do {|f| a.add(f)};
            });
        };


        dumpCats = {|x,l,y|
            var ents = x[\entries];
            var subs = x[\subcats];
            var c, z;

            if(ents.notEmpty, {
                ents.sort {|a,b| a.path.basename < b.path.basename}.do {|doc|
                    folder = doc.path.dirname;
                    folder = if(folder==".", {""}, {" ["++folder++"]"});
                    l.add((tag:'##'));
                    l.add((tag:'link', text:doc.path++"##"++doc.title));
                    l.add((tag:'prose', text:" - "++doc.summary));
                    l.add((tag:'soft', text:folder));
                    switch(doc.installed,
                        \extension, { l.add((tag:'soft', text:" (+)")) },
                        \missing, { l.add((tag:'strong', text:" (not installed)")) }
                    );
/*                    if(doc.path.dirname=="Classes") {
                        c = doc.path.basename.asSymbol.asClass;
                        if(c.notNil) {
                            if(c.filenameSymbol.asString.beginsWith(thisProcess.platform.classLibraryDir).not) {
                                l.add((tag:'soft', text:" (+)"));
                            };
                        } {
                            l.add((tag:'strong', text:" (not installed)"));
                        };
                    };
*/
                };
            });

            subs.keys.asList.sort {|a,b| a<b}.do {|k|
                z = SCDocRenderer.simplifyName(y++">"++k);
                l.add((tag:'##'));
                l.add((tag:\anchor, text:z));
                l.add((tag:\strong, text:k));
                l.add((tag:\tree, children:m=List.new));
                dumpCats.value(subs[k],m,z);
            };
        };

        sorted = tree.keys.asList.sort {|a,b| a<b};

        if(toc) {
            node.add((tag:'prose', text:"Jump to: ", display:\block));
            sorted.do {|k,i|
                if(i!=0, {node.add((tag:'prose', text:", ", display:\inline))});
                node.add((tag:'link', text:"#"++SCDocRenderer.simplifyName(k)++"#"++k));
//                node.add((tag:'prose', text:" ", display:\inline));
            };
        };

        sorted.do {|k|
            node.add((tag:\section, text:k, children:m=List.new));
            m.add((tag:\tree, children:l=List.new));
            dumpCats.(tree[k],l,k);
        };
    }

    overviewCategories {|catMap|
        var r = List.new;
        r.add((tag:'title', text:"Document Categories"));
        r.add((tag:'summary', text:"All documents by categories"));
        r.add((tag:'related', text:"Overviews/Documents, Browse, Search"));

        this.makeCategoryTree(catMap,r,toc:false);

        root = r;
    }
*/
/*
    overviewAllClasses {|docMap|
        var name, doc, link, n, r = List.new, cap, old_cap=nil, sortedKeys;
        r.add((tag:'title', text:"Classes"));
        r.add((tag:'summary', text:"Alphabetical index of all classes"));
        r.add((tag:'related', text:"Overviews/ClassTree, Overviews/Methods"));

        sortedKeys = Class.allClasses.reject {|c| c.name.asString.find("Meta_")==0};

        old_cap = nil;
        r.add((tag:'prose', text:"Jump to: ", display:\block));
        sortedKeys.do {|c|
            name = c.name.asString;
            cap = name.first.toUpper;
            if(cap!=old_cap, {
                r.add((tag:'link', text:"#"++cap.asString));
                r.add((tag:'prose', text:" ", display:\inline));
                old_cap = cap;
            });
        };

        old_cap = nil;
        sortedKeys.do {|c|
            name = c.name.asString;
            link = "Classes" +/+ name;
            doc = docMap[link];
            if(doc.notNil) {
                cap = name.first.toUpper;
                if(cap!=old_cap, {
                    r.add((tag:'section', text:cap.asString, children:n=List.new));
                    n.add((tag:'list', children:n=List.new));
                    old_cap = cap;
                });
                n.add((tag:'##'));
                n.add((tag:'link', text: link));
                n.add((tag:'prose', text: " - "++ if(doc.notNil, {doc.summary}, {""})));
                switch(doc.installed,
                    \extension, { n.add((tag:'soft', text:" (+)")) },
                    \missing, { n.add((tag:'strong', text:" (not installed)")) }
                );
            };
        };
        root = r;
    }
*/
/*
    overviewAllMethods {|docMap|
        var name, n, r = List.new, cap, old_cap, t, m, ext, sortedKeys, pfx;
        r.add((tag:'title', text:"Methods"));
        r.add((tag:'summary', text:"Alphabetical index of all methods"));
        r.add((tag:'related', text:"Overviews/ClassTree, Overviews/Classes"));

        r.add((tag:'prose', text:"This is an alphabetical list of all implemented methods, including private and undocumented methods.", display:\block));
        r.add((tag:'prose', text:"The classnames are prefixed by * for classmethods and postfixed by + for extensionmethods.", display:\block));

        t = IdentityDictionary.new;

        Class.allClasses.do {|c|
            name = c.name.asString;
            c.methods.do {|x|
                if(t[x.name]==nil, {t[x.name] = List.new});

                t[x.name].add([name,x.isExtensionOf(c)]);
            };
        };

        sortedKeys = t.keys.asList.sort {|a,b| a<b};

        old_cap = nil;
        r.add((tag:'prose', text:"Jump to: ", display:\block));
        sortedKeys.do {|k|
            name = k.asString;
            cap = name.first.toLower;
            if(cap!=old_cap, {
                r.add((tag:'link', text:"#"++cap.asString));
                r.add((tag:'prose', text:" ", display:\inline));
                old_cap = cap;
            });
        };

        old_cap = nil;
        sortedKeys.do {|k|
            name = k.asString;
                cap = name.first.toLower;
                if(cap!=old_cap, {
                    r.add((tag:'section', text:cap.asString, children:n=List.new));
                    n.add((tag:'definitionlist', children:m=List.new));
                    old_cap = cap;
                });

            m.add((tag:'##'));
            m.add((tag:'anchor', text:name));
            m.add((tag:'code', text:name));
            m.add((tag:'||'));
            if(name.last==$_, {name=name.drop(-1)});
            t[k].do {|c,i|
                n = c[0];
                pfx = "-";
                if(i!=0, {m.add((tag:'prose', text:", ", display:\inline))});
                if(n.find("Meta_")==0, {
                    n = n.drop(5);
                    m.add((tag:'prose', text:"*", display:\inline));
                    pfx = "*";
                });
//                m.add((tag:'link', text: "Classes" +/+ n ++ "#" ++ SCDocRenderer.simplifyName(name)));
                m.add((tag:'link', text: "Classes" +/+ n ++ "#" ++ pfx ++ name));
                if(c[1], {m.add((tag:'prose', text:"+", display:\inline))});
            };
        };

        root = r;
        ^t;
    }
*/
/*    overviewAllDocuments {|docMap|
        var kind, name, doc, link, n, r = List.new, cap, old_cap, sortedKeys;
        r.add((tag:'title', text:"Documents"));
        r.add((tag:'summary', text:"Alphabetical index of all documents"));
        r.add((tag:'related', text:"Overviews/Categories"));

        sortedKeys = docMap.keys.asList.sort {|a,b| a.split($/).last < b.split($/).last};

        old_cap = nil;
        r.add((tag:'prose', text:"Jump to: ", display:\block));
        sortedKeys.do {|link|
            name = link.split($/).last;
            cap = name.first.toUpper;
            if(cap!=old_cap, {
                r.add((tag:'link', text:"#"++cap.asString));
                r.add((tag:'prose', text:" ", display:\inline));
                old_cap = cap;
            });
        };

        old_cap = nil;
        sortedKeys.do {|link|
            doc = docMap[link];
            name = link.split($/).last;
            kind = link.dirname;
            kind = if(kind==".", {""}, {" ["++kind++"]"});
            cap = name.first.toUpper;
            if(cap!=old_cap, {
                r.add((tag:'section', text:cap.asString, children:n=List.new));
                n.add((tag:'list', children:n=List.new));
                old_cap = cap;
            });
            n.add((tag:'##'));
            n.add((tag:'link', text: link++"##"++doc.title));
//            n.add((tag:'||'));
            n.add((tag:'prose', text: " - "++if(doc.notNil, {doc.summary}, {""})));
//            n.add((tag:'||'));
            n.add((tag:'soft', text: kind));
        };
        root = r;
    }
    */

/*    overviewServer {|catMap|
        var r = List.new;
        r.add((tag:'title', text:"Server stuff"));
        r.add((tag:'summary', text:"Overview of server related stuff"));

        this.makeCategoryTree(catMap,r,"^Server$");
        this.makeCategoryTree(catMap,r,"^UGens$");
        root = r;
    }*/
}

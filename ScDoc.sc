ScDocParser {
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

    init {
        root = tree = List.new;
        stack = List.new;
        stack.add([tree,0]);
        current = nil;
        singleline = false;
        level = 0;
        modalTag = nil;
        isWS = false;
        afterClosing = false;
        stripFirst = false;
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
        tree = p[0];
        level = p[1];
    }

    enterLevel {|n|
        this.leaveLevel(n);
        stack.add([tree,level]);
        level = n;
    }

/*    stripWhiteSpace {|str|
        var a=0, b=str.size-1;
        //FIXME: how does this handle strings that are empty or single characters?
        while({(str[a]==$\n) or: (str[a]==$\ )},{a=a+1});
        while({(str[b]==$\n) or: (str[b]==$\ )},{b=b-1});
        ^str.copyRange(a,b);
    }
*/    
    endCurrent {
        if(current.notNil,{
//            if(current.text.notNil, {
//                current.text = this.stripWhiteSpace(current.text);
//            });
            current = nil;
        });
    }

    addTag {|tag, text="", children=false, display=\block|
        this.endCurrent;
        tag = tag.asString.drop(-2).asSymbol;
        current = (tag:tag, display:display, text:text, children:if(children,{List.new},{nil}));
//        current = (tag:tag, text:text, children:if(children,{List.new},{nil}));
        tree.add(current);
        if(children, {tree = current.children}); //recurse into children list
        if(text.isNil, {this.endCurrent}); //we don't have any text field to add to for this tag, so start fresh..    
    }

    handleWord {|word,lineno,wordno|
        var tag = word.toLower.asSymbol;
        var simpleTag = {
            singleline = true;
            this.addTag(tag);
            stripFirst = true;
        };
        var noNameSection = {
            singleline = true; //this doesn't actually matter here since we don't have a text field?
            this.enterLevel(1);
            this.addTag(tag,nil,true);
        };
        var namedSection = {|lev|
            {
                singleline = true;
                this.enterLevel(lev);
                this.addTag(tag,"",true);
                stripFirst = true;
            }        
        };
        var modalRangeTag = {
            singleline = false;
            this.addTag(tag);
            lastTagLine = lineno;
            modalTag = '::';
        };
        var listEnter = {
            singleline = false; //this doesn't actually matter here since we don't have a text field?
//            this.enterLevel(10);
            stack.add([tree,level]);
            this.addTag(tag,nil,true);
            lastTagLine = lineno;
        };

        // modal tags ignore all other tags until their closing tag occurs.
        // here we check if we are in a modal tag (code, emphasis, link) and then
        // if we got the closing tag.
        if(modalTag.notNil, {
            //only allow modal block tags to be closed with the closing tag as the first word on a line
            if((tag==modalTag) and: ((wordno==0) or: (lastTagLine==lineno)),{
                if(current.notNil, {
                    current.display = if(lastTagLine==lineno,\inline,\block);
                });
                this.endCurrent;
                modalTag = nil;
                afterClosing = true;
            },{
                if(("[^ ]+::".matchRegexp(word)) and: (lastTagLine==lineno), { //split unhandled tag-like word
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
                'argument::',           namedSection.(4),
                'class::',              simpleTag,
                'title::',              simpleTag,
                'summary::',            simpleTag,
                'related::',            simpleTag,
                'keywords::',           simpleTag,
                'categories::',         simpleTag,
//                'doctype::',            simpleTag,
                'note::',               simpleTag,
                'warning::',            simpleTag,
                
                'code::',               modalRangeTag,
                'emphasis::',           modalRangeTag,
                'strong::',             modalRangeTag,
                'link::',               modalRangeTag,
                'anchor::',             modalRangeTag,
                'image::',              modalRangeTag,
                'soft::',               modalRangeTag,

                'list::',               listEnter,
                'tree::',               listEnter,
                'numberedlist::',       listEnter,
                'definitionlist::',     listEnter,
                'table::',              listEnter,
                'footnote::',           listEnter,
//                'row::', {
//                    singleline = false;
//                    this.enterLevel(11);
//                    this.addTag(tag,nil,true);
//                },
                '##', {
                    singleline = false;
                    this.addTag('##::',nil,false,\inline); //make it look like an ordinary tag since we drop the :: in the output tree
                },
                '||', {
                    singleline = false;
                    this.addTag('||::',nil,false,\inline);
                },
                '::', { //ends tables and lists
                    this.popTree;
                    if(current.notNil, {
                        current.display = if(lastTagLine==lineno,\inline,\block);
                    });
                    this.endCurrent;
                },
                '\\::', {
                    this.addText("::");
                },
                
                { //default case
                    if("[a-zA-Z]+://.+".matchRegexp(word),{ //auto link URIs
                        this.addTag('link::',word,false);
                        if(current.notNil, {
                            current.display = \inline;
                        });
                        this.endCurrent;
                    },{
                        this.addText(word); //plain text, add the word.
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
                this.addTag('prose::', word);
            });
        });
    }

    endLine {
        if(singleline,{this.endCurrent});
        // pass through newlines for vari-line tags.
        if(current.notNil,{current.text = current.text ++ "\n"});
    }
   
/*    parse {|string|
        var lines = string.split($\n);
        this.init;
        lines.do {|line,l|
            var words = line.split($\ );
            var w2=0;
            words.do {|word,w|
//                var split = word.findRegexp("([a-z]+\\[\\[)(.+)(\\]\\])(.*)")[1..];
                var split = word.findRegexp("([a-z]+::)(.+)(::)(.*)")[1..];
                if(split.isEmpty, {
                    this.handleWord(word,l,w2);
                    if(word.isEmpty.not,{w2=w2+1});
                },{
                    split.do {|x|
                        if(x[1].isEmpty.not,{this.handleWord(x[1],l,w)});
                    };
                });
            };
            this.endLine;
        };
    }*/

    parse {|string|
        var lines = string.split($\n); //split lines
        var w, split, split2, word;
        this.init;
        lines.do {|line,l|
            split = line.findRegexp("[a-zA-Z]+::[^ \n\t]+::|[a-zA-Z]*::|[ \n\t]+|[^ \n\t]+"); //split words and tags and ws
            w = 0;
            split.do {|e|
                word = e[1];
                split2 = word.findRegexp("([a-zA-Z]+::)([^ \n\t]+)(::)")[1..]; //split stuff like::this::...
                if(split2.isEmpty,{
                    isWS = "^[ \n\t]+$".matchRegexp(word);
                    this.handleWord(word,l,w);
                    if(isWS.not,{w=w+1});
                },{
                    split2.do {|e2|
                        isWS = "^[ \n\t]+$".matchRegexp(e2[1]);
                        this.handleWord(e2[1],l,w);
                        w=w+1;
                    };
                });
            };
            if(modalTag.isNil and: split.isEmpty, { this.endCurrent }); //force a new prose on double blank lines
//            this.handleWord(" ",l,-1);
            this.endLine;
        };
    }

    parseFile {|filename|
        var file = File.open(filename,"r");
        this.parse(file.readAllString);
        file.close;
    }

    dumpSubTree {|t,i="",lev=1|
        t.do {|e|
            "".postln;
            (i++"TAG:"+e.tag+"( level"+lev+e.display+")").postln;
            if(e.text.notNil, {
                (i++"TEXT: \""++e.text++"\"").postln;
            });
            if(e.children.notNil, {
                (i++"CHILDREN:").postln;
                this.dumpSubTree(e.children,i++"    ",lev+1);
            });
        }
    }
    
    dump {
        this.dumpSubTree(root);
        ^nil;
    }
    
    findNode {|tag,rootNode=nil|
        var res = nil;
        if(rootNode.isNil, { rootNode=root });
        rootNode.do {|n|
            if(n.tag == tag.asSymbol, { res = n});
        };
        if(res.notNil, {
            ^res;
        }, {
            ^(tag:nil, text:"", children:[]);
        });
    }
    
    dumpClassTree {|node,c|
        var n;
        if(c.name.asString.find("Meta_")==0, {^nil});
        node.children.add((tag:'##'));
        node.children.add((tag:'link', text:"Classes/"++c.name.asString));
        
        if(c.subclasses.notNil, {
            n = (tag:'tree', children:List.new);
            node.children.add(n);
            c.subclasses.copy.sort {|a,b| a.name < b.name}.do {|x|
                this.dumpClassTree(n,x);
            };
        });
    }
    
    overviewClassTree {
        var r = List.new;
        var n = (tag:'tree', children:List.new);
        r.add((tag:'title', text:"Class Tree"));
        r.add((tag:'summary', text:"All classes by inheritance tree"));
        r.add((tag:'related', text:"Overviews/Classes, Overviews/Categories, Overviews/Methods"));
//        r.add((tag:'categories', text:"Classes"));
        r.add(n);
        this.dumpClassTree(n,Object);
        root = r;
    }
    
    makeCategoryTree {|catMap,node,filter=nil|
        var a, p, e, n, l, m, kinds, folder, v, dumpCats;
        var tree = Dictionary.new;
        
        catMap.pairsDo {|cat,files|
            p=tree;
            l=cat.split($>);
            if(if(filter.notNil, {filter.matchRegexp(l.first)}, {true}), {
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
            var z;

            if(ents.notEmpty, {
                ents.sort {|a,b| a.path<b.path }.do {|doc|
                    folder = doc.path.dirname;
                    folder = if(folder==".", {""}, {" ["++folder++"]"});
                    l.add((tag:'##'));
                    l.add((tag:'link', text:doc.path));
                    l.add((tag:'prose', text:" - "++doc.summary));
                    l.add((tag:'soft', text:folder));
                };
            });
                        
            subs.keys.asList.sort {|a,b| a<b}.do {|k|
                z = ScDocRenderer.simplifyName(y++">"++k);
                l.add((tag:'##'));
                l.add((tag:\anchor, text:z));
                l.add((tag:\strong, text:k));
                l.add((tag:\tree, children:m=List.new));
                dumpCats.value(subs[k],m,z);
            };    
        };
        
        tree.keys.asList.sort {|a,b| a<b}.do {|k|
            node.add((tag:\section, text:k, children:m=List.new));
            m.add((tag:\tree, children:l=List.new));
            dumpCats.(tree[k],l,k);
        };
    }
    
    overviewCategories {|catMap|
        var r = List.new;
//        var a, p, e, n, l, m, kinds, folder, v, tree, dumpCats;
        r.add((tag:'title', text:"Document Categories"));
        r.add((tag:'summary', text:"All documents by categories"));
        r.add((tag:'related', text:"Overviews/Documents"));
       
        this.makeCategoryTree(catMap,r);
        
        // kind - category
/*        kinds = Dictionary.new;
        catMap.pairsDo {|k,v|
            v.do {|file|
                folder = file.path.dirname;
                if(folder!=".", {
                    if(kinds[folder].isNil, { kinds[folder] = Dictionary.new });
                    if(kinds[folder][k].isNil, { kinds[folder][k] = List.new });
                    kinds[folder][k].add(file);
                });
            };
        };
        kinds.keys.asList.sort {|a,b| a<b}.do {|k|
            v = kinds[k];
            r.add((tag:'section', text:k, children:n=List.new));
            v.keys.asList.sort {|a,b| a<b}.do {|cat|
                n.add((tag:'subsection', text:cat, children:m=List.new));
                m.add((tag:'list', children:l=List.new));
                v[cat].do {|doc|
                    l.add((tag:'##'));
                    l.add((tag:'link', text:doc.path));
                    l.add((tag:'prose', text:" - "++doc.summary));
                };
            };
        };*/

        root = r;
    }
    
    overviewAllClasses {|docMap|
        var name, doc, link, n, r = List.new, cap, old_cap=nil;
        r.add((tag:'title', text:"Classes"));
        r.add((tag:'summary', text:"Alphabetical index of all classes"));
        r.add((tag:'related', text:"Overviews/ClassTree, Overviews/Categories, Overviews/Methods"));
//        r.add((tag:'categories', text:"Classes"));
        Class.allClasses.do {|c|
            name = c.name.asString;
            if(name.find("Meta_")!=0, {
                link = "Classes" +/+ name;
                doc = docMap[link];
                doc = if(doc.notNil, {doc.summary}, {""});
                cap = name.first;
                if(cap!=old_cap, {
                    r.add((tag:'section', text:cap.asString, children:n=List.new));
                    n.add((tag:'list', children:n=List.new));
                    old_cap = cap;
                });
                n.add((tag:'##'));
                n.add((tag:'link', text: link));
//                n.add((tag:'||'));
                n.add((tag:'prose', text: " - "++doc));
            });
        };
        root = r;
    }

    overviewAllMethods {|docMap|
        var name, n, r = List.new, cap, old_cap=nil, t, m, meta;
        r.add((tag:'title', text:"Methods"));
        r.add((tag:'summary', text:"Alphabetical index of all methods"));
        r.add((tag:'related', text:"Overviews/ClassTree, Overviews/Classes"));
        
        t = Dictionary.new;

        Class.allClasses.do {|c|
            name = c.name.asString;
            c.methods.do {|x|
                if(t[x.name]==nil, {t[x.name] = List.new});
                t[x.name].add(name);
            };
        };

        t.keys.asList.sort {|a,b| a<b}.do {|k|
            name = k.asString;
                cap = name.first;
                if(cap!=old_cap, {
                    r.add((tag:'section', text:cap.asString, children:n=List.new));
                    n.add((tag:'definitionlist', children:m=List.new));
                    old_cap = cap;
                });

            m.add((tag:'##'));
            m.add((tag:'prose', text:name));
            m.add((tag:'||'));
            if(name.last==$_, {name=name.drop(-1)});
            t[k].do {|c,i|
                if(c.find("Meta_")==0, {c = c.drop(5)});
                if(i!=0, {m.add((tag:'prose', text:", "))});
                m.add((tag:'link', text: "Classes" +/+ c ++ "#" ++ ScDocRenderer.simplifyName(name)));
            };
        };
        
        root = r;
    }
    
    overviewAllDocuments {|docMap|
        var kind, name, doc, link, n, r = List.new, cap, old_cap=nil;
        r.add((tag:'title', text:"Documents"));
        r.add((tag:'summary', text:"Alphabetical index of all documents"));
        r.add((tag:'related', text:"Overviews/Categories"));
        docMap.keys.asList.sort {|a,b| a.split($/).last<b.split($/).last}.do {|link|
            doc = docMap[link];
            doc = if(doc.notNil, {doc.summary}, {""});
            name = link.split($/).last;
            kind = link.dirname;
            kind = if(kind==".", {""}, {" ["++kind++"]"});
            cap = name.first;
            if(cap!=old_cap, {
                r.add((tag:'section', text:cap.asString, children:n=List.new));
                n.add((tag:'list', children:n=List.new));
                old_cap = cap;
            });
            n.add((tag:'##'));
            n.add((tag:'link', text: link));
//            n.add((tag:'||'));
            n.add((tag:'prose', text: " - "++doc));
//            n.add((tag:'||'));
            n.add((tag:'soft', text: kind));
        };
        root = r;
    }
    
    overviewServer {|catMap|
        var r = List.new;
        r.add((tag:'title', text:"Server stuff"));
        r.add((tag:'summary', text:"Overview of server related stuff"));

        this.makeCategoryTree(catMap,r,"^Server$"); 
        this.makeCategoryTree(catMap,r,"^UGens$");
        root = r;
    }
}

ScDocRenderer {
    var <>parser;

    var last_display;
    var currentClass;
    var collectedArgs;
    var dirLevel;
    var baseDir;
    var footNotes;
    
    *new {|p=nil|
        ^super.newCopyArgs(p).init;
    }

    init {
    }
    
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
                last_display = \inline;
                file.write("<a name='footnote_"++(i+1)++"'/><div class='footnote'>");
                file.write("("++(i+1)++") - ");
                n.children.do(this.renderHTMLSubTree(file,_));
                file.write("</div>");
            };
            file.write("</div>");
        };
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

    renderHTMLSubTree {|file,node,parentTag=false|
        var f, m, n, mname, args, split, mstat;

        var do_children = {|p=false|
            if(node.children.notNil, {
                node.children.do {|e| this.renderHTMLSubTree(file,e,if(p,{node.tag},{parentTag})) };
            });
        };

        switch(node.tag,
            'prose', {
                if(last_display == \block, {
                    file.write("<p>"++this.escapeSpecialChars(node.text));
                }, {
                    file.write(this.escapeSpecialChars(node.text));                
                });
            },
            'section', {
                file.write("<a name='"++ScDocRenderer.simplifyName(node.text)++"'><h2>"++this.escapeSpecialChars(node.text)++"</h2></a>\n");
                do_children.();
            },
            'subsection', {
                file.write("<a name='"++ScDocRenderer.simplifyName(node.text)++"'><h3>"++this.escapeSpecialChars(node.text)++"</h3></a>\n");
                do_children.();
            },
            'classmethods', {
                file.write("<a name='classmethods'><h2>Class Methods</h2></a>\n<div id='classmethods'>");
                do_children.(true);
                file.write("</div>");
            },
            'instancemethods', {
                file.write("<a name='instancemethods'><h2>Instance Methods</h2></a>\n<div id='instancemethods'>");
                do_children.(true);
                file.write("</div>");
            },
            'method', {
                //for multiple methods with same signature and similar function:
                //ar kr (x = 42, y = 123)
                f = node.text.findRegexp(" *([^(]+) *(\\(.*\\))?");
//                args = f[2][1];
                args = "";
//FIXME: handle overridden argumentnames/defaults?
//perhaps we should check argument names at least? and only use overriding for "hidden" default values?
//also, perhaps better to read the default values from the argument tags?
//ignore markup-provided arguments for now..
                split = f[1][1].findRegexp("[^ ,]+");
                split.do {|r|
                    mstat = 0;
                    mname = r[1];
/*                    if(parentTag==\instancemethods,{
                        m = currentClass.findRespondingMethodFor(mname.asSymbol);
                    },{
                        m = currentClass.class.findRespondingMethodFor(mname.asSymbol);
                    });*/
                    m = if(parentTag==\instancemethods,{currentClass},{currentClass.class}).findRespondingMethodFor(mname.asSymbol);
                    if(m.notNil, {
                        mstat = mstat | 1;
//                        args = (m.argumentString ? "").replace("this, ","").replace("this","");
//                        if(args.notEmpty,{ args = " ("++args++")" });
                        args = ScDocRenderer.makeArgString(m);
                    });
                    //check for setter
                    m = if(parentTag==\instancemethods,{currentClass},{currentClass.class}).findRespondingMethodFor((mname++"_").asSymbol);
                    if(m.notNil, { mstat = mstat | 2 });

                    switch (mstat,
                        // getter only
                        1, { file.write("<a name='"++mname++"'><h3 class='methodname'>"++this.escapeSpecialChars(mname)++" "++args++"</h3></a>\n"); },
                        // setter only
                        2, { file.write("<a name='"++mname++"'><h3 class='methodname'>"++this.escapeSpecialChars(mname)++" = value</h3></a>\n"); },
                        // getter and setter
                        3, { file.write("<a name='"++mname++"'><h3 class='methodname'>"++this.escapeSpecialChars(mname)++" [= value]</h3></a>\n"); },
                        // method not found
                        0, { file.write("<a name='"++mname++"'><h3 class='methodname'>"++this.escapeSpecialChars(mname)++": METHOD NOT FOUND!</h3></a>\n"); }
                    );
                };

                file.write("<div class='method'>");
                collectedArgs = [];
                do_children.();
                file.write("<table class='arguments'>\n");
                collectedArgs.do {|a|
                    file.write("<tr><td class='argumentname'>"+a.text+"<td>");
                    a.children.do {|e| this.renderHTMLSubTree(file,e,a.tag) };
                };
                file.write("</table>");
                
                file.write("</div>");
            },
            'argument', {
//                file.write("<h4 class='argumentname'>"++node.text++"</h4>\n");
//                file.write("<div class='argument'>");
//                do_children.();
//                file.write("</div>");
                collectedArgs = collectedArgs.add(node);
            },
            'description', {
                file.write("<a name='description'><h2>Description</h2></a>\n<div id='description'>");
                do_children.();
                file.write("</div>");
            },
            'examples', {
                file.write("<a name='examples'><h2>Examples</h2></a>\n<div id='examples'>");
                do_children.();
                file.write("</div>");
            },
            'note', {
                file.write("<div class='note'><span class='notelabel'>NOTE:</span> "++this.escapeSpecialChars(node.text)++"</div>");
            },
            'warning', {
                file.write("<div class='warning'><span class='warninglabel'>WARNING:</span> "++this.escapeSpecialChars(node.text)++"</div>");
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
                if("[a-zA-Z]+://.+".matchRegexp(node.text),{
                    file.write("<a href=\""++node.text++"\">"++this.escapeSpecialChars(node.text)++"</a>");
                },{
                    f = node.text.split($#);
                    m = if(f[1].notNil, {"#"++f[1]}, {""});
//                    n = if(f[1].notNil, {" ["++f[1]++"]"}, {""});
//                    file.write("<a href=\""++baseDir +/+ f[0]++".html"++m++"\">"++f[0].split($/).last++" "++m++"</a>");
                    file.write("<a href=\""++baseDir +/+ f[0]++".html"++m++"\">"++f[0].split($/).last++"</a>");
                });
            },
            'anchor', {
                file.write("<a name='"++node.text++"'</a>");
            },
            'code', {
                if(node.display == \block, {
                    file.write("<pre>"++this.escapeSpecialChars(node.text)++"</pre>\n");
                }, {
                    file.write("<code>"++this.escapeSpecialChars(node.text)++"</code>\n");
                });
            },
            'image', {
//                if(node.display == \block, {
                f = node.text.split($#);
                    file.write("<div class='image'><img src='"++f[0]++"'/>");
                    if(f[1].notNil, {
                        file.write("<br><b>"++f[1]++"</b>");
                    });
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
                last_display = node.display = \inline;
                footNotes.add(node);
                file.write("<a class='footnote' href='#footnote_"++footNotes.size++"'><sup>"++footNotes.size++"</sup></a> ");
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
            'related', { },
            'categories', { },
            
            'root', {
                do_children.();
            },
            
            { //unhandled tag
//                file.write("(TAG:"++node.tag++")");
                if(node.text.notNil,{file.write(this.escapeSpecialChars(node.text))});
            }
        );
        last_display = node.display;
    }
    
    renderHTMLHeader {|f,name,type,folder|
        var x, cats, m;
        var style = baseDir +/+ "scdoc.css";
        f.write("<html><head><title>"++name++"</title><link rel='stylesheet' href='"++style++"' type='text/css' /></head><body>");

//        cats = ScDoc.splitList(parser.findNode(\categories).text);
//        cats = if(cats.notNil, {cats.join(", ")}, {""});
        if(folder==".",{folder=""});
        f.write("<div class='header'>");
        f.write("<div id='label'><a href='"++baseDir+/+"Help.html"++"'>SuperCollider</a> "++folder.asString.toUpper++"</div>");
        x = parser.findNode(\categories);
        if(x.text.notEmpty, {
            f.write("<div id='categories'>");
//            f.write("Categories: ");
            f.write(ScDoc.splitList(x.text).collect {|r|
//                "<a href='"++baseDir +/+ "Overviews/Categories.html#"++ScDocRenderer.simplifyName(r).split($>).first++"'>"++r++"</a>"
                "<a href='"++baseDir +/+ "Overviews/Categories.html#"++ScDocRenderer.simplifyName(r)++"'>"++r++"</a>"
            }.join(", "));
            f.write("</div>");
        });    
        
        f.write("<h1>"++name++"</h1>");
        x = parser.findNode(\summary);
        f.write("<div id='summary'>"++this.escapeSpecialChars(x.text)++"</div>");
        f.write("</div>");

        f.write("<div class='subheader'>\n");
        
        if(type==\class,{
            m = currentClass.filenameSymbol.asString;
            f.write("<div id='filename'>Location: "++m.dirname++"/<a href='file://"++m++"'>"++m.basename++"</a></div>");
            if(currentClass != Object, {
                f.write("<div class='inheritance'>");
                f.write("Inherits from: ");
                f.write(currentClass.superclasses.collect {|c|
                    "<a href=\"../Classes/"++c.name++".html\">"++c.name++"</a>"
                }.join(" : "));
                f.write("</div>");
            });
            if(currentClass.subclasses.notNil, {
                f.write("<div class='inheritance'>");
                f.write("Subclasses: ");
                f.write(currentClass.subclasses.collect {|c|
                    "<a href=\"../Classes/"++c.name++".html\">"++c.name++"</a>"
                }.join(", "));
                f.write("</div>");
            });
        });
        
        x = parser.findNode(\related);
        if(x.text.notEmpty, {
            f.write("<div id='related'>");
            f.write("See also: ");
            f.write(ScDoc.splitList(x.text).collect {|r|
                "<a href=\""++baseDir +/+ r++".html\">"++r.split($/).last++"</a>"
            }.join(", "));
            f.write("</div>");
        });
        f.write("</div>");
    }

    renderHTML {|filename, folder="."|
        var f,x,name;

        ("mkdir -p"+filename.dirname.escapeChar($ )).systemCmd;
                
        f = File.open(filename, "w");
        x = parser.findNode(\class);
        
        last_display = \block;

        //folder is the directory path of the file relative to the help tree,
        //like 'Classes' or 'Tutorials'.
        dirLevel = folder.split($/).reject {|y| (y.size<1) or: (y==".")}.size;
        baseDir = ".";
        dirLevel.do { baseDir = baseDir +/+ ".." };

        footNotes = List.new;
        
//        ("'"++baseDir++"'").postln;

        if(x.text.notEmpty, {
            name = x.text.stripWhiteSpace;
            currentClass = name.asSymbol.asClass;

            this.renderHTMLHeader(f,name,\class,folder);

            x = parser.findNode(\description);
            this.renderHTMLSubTree(f,x);

            x = parser.findNode(\classmethods);
            this.renderHTMLSubTree(f,x);
//crashes here for generated autodocs on IOStream and String, why?
            x = parser.findNode(\instancemethods);
            this.renderHTMLSubTree(f,x);

            x = parser.findNode(\examples);
            this.renderHTMLSubTree(f,x);
        },{
            x = parser.findNode(\title);
            name = x.text.stripWhiteSpace;
            this.renderHTMLHeader(f,name,\other,folder);
            this.renderHTMLSubTree(f,(tag:'root',children:parser.root));
        });

        this.renderFootNotes(f);

        f.write("</body></html>");
        f.close;
    }
}

ScDoc {
    var <>helpTargetDir;
    var <>helpSourceDir;
    var <categoryMap;
    var <docMap;
    
    *new {
        ^super.new.init;
    }
    
    *splitList {|txt|
        ^txt.findRegexp("[-_>a-zA-Z0-9]+[-_>/a-zA-Z0-9 ]*[-_>/a-zA-Z0-9]+").flop[1];
    }

    init {
        helpTargetDir = thisProcess.platform.userAppSupportDir +/+ "/Help";
        helpSourceDir = thisProcess.platform.systemAppSupportDir +/+ "/HelpSource";
    }
        
    makeOverviews {
        var p = ScDocParser.new;
        var r = ScDocRenderer.new;
        /* TODO:
          All documents alphabetically
          All classes alphabetically
          All classes by categories
          All methods index
        */

        "Generating ClassTree...".postln;
        r.parser = p.overviewClassTree;
        r.renderHTML(helpTargetDir +/+ "Overviews/ClassTree.html","Overviews");

        "Generating Class overview...".postln;        
        r.parser = p.overviewAllClasses(docMap);
        r.renderHTML(helpTargetDir +/+ "Overviews/Classes.html","Overviews");

        "Generating Methods overview...".postln;
        r.parser = p.overviewAllMethods(docMap);
        r.renderHTML(helpTargetDir +/+ "Overviews/Methods.html","Overviews");

        "Generating Documents overview...".postln;
        r.parser = p.overviewAllDocuments(docMap);
        r.renderHTML(helpTargetDir +/+ "Overviews/Documents.html","Overviews");

        "Generating Categories overview...".postln;
        r.parser = p.overviewCategories(categoryMap);
        r.renderHTML(helpTargetDir +/+ "Overviews/Categories.html","Overviews");

        "Generating Server overview...".postln;
        r.parser = p.overviewServer(categoryMap);
        r.renderHTML(helpTargetDir +/+ "Overviews/Server.html","Overviews");
    }
    
    makeMethodList {|c,n|
        var l, mets, name;

        mets = c.class.methods;
        if(mets.notNil, {
            n.add((tag:\classmethods, children:l=List.new));
            mets.do {|m|
                name = m.name.asString;
                l.add((tag:\method, text:name));
            };
        });

        mets = c.methods;
        if(mets.notNil, {                
            n.add((tag:\instancemethods, children:l=List.new));    
            mets.do {|m|
                name = m.name.asString;
//                if(name.last!=$_, { //what if the class has a setter only?
                    l.add((tag:\method, text:name));
//                });
            };
        });
    }
    
    classHasArKrIr {|c|
        ^[\ar,\kr,\ir].collect {|m| c.class.findRespondingMethodFor(m).notNil }.reduce {|a,b| a or: b};
    }
    
    handleUndocumentedClasses {|force=false|
        var p = ScDocParser.new;
        var r = ScDocRenderer.new;
        var n, m, name, cats;
        var src, dest;
        ("Checking for undocumented classes...").postln;
        Class.allClasses.do {|c|
            name = c.name.asString;
            src = helpSourceDir +/+ "Classes" +/+ name++".schelp";
            dest = helpTargetDir +/+ "Classes" +/+ name ++ ".html";
            if(File.exists(src).not and: (name.find("Meta_")!=0), {
//                ("Undocumented class: "++name).postln;
                
                n = List.new;
                n.add((tag:\class, text:c.name.asString));
                n.add((tag:\summary, text:""));
                
                cats = "Undocumented classes";
                
                if(this.classHasArKrIr(c), {cats = cats ++ ", UGens>Undocumented"});
                
                n.add((tag:\categories, text:cats));
                n.add((tag:\description, children:m=List.new));
                
                m.add((tag:\prose, text:"This class is missing documentation. "));
                m.add((tag:\prose, text:"Please create and edit "++src));
                
                p.root = n;
//                this.addToCategoryMap(p, "Classes" +/+ name);
                this.addToDocMap(p, "Classes" +/+ name);
                if((force or: File.exists(dest).not), {
                    ("Generating doc for class: "++name++" -> "++dest).postln;
                    this.makeMethodList(c,n);
                    r.parser = p;
                    r.renderHTML(dest,"Classes");
                });
            });
        };
    }
    
    addToDocMap {|parser, path|
        docMap[path] = (
            path:path,
            summary:parser.findNode(\summary).text,
            categories:parser.findNode(\categories).text
        );
    }
    
    makeCategoryMap {
        var cats;
        ("Creating category map...").postln;
        categoryMap = Dictionary.new;
        docMap.pairsDo {|k,v|
            cats = ScDoc.splitList(v.categories);
            cats = cats ? ["Uncategorized"];
            cats.do {|cat|
                if(categoryMap[cat].isNil, {
                    categoryMap[cat] = List.new;
                });
                categoryMap[cat].add(v);
            };
        };
    }
/*    
    addToCategoryMap {|parser, path|
        var cats = parser.findNode(\categories).text;
        cats = ScDoc.splitList(cats);
        cats = cats ? ["Uncategorized"];
        cats.do {|cat|
            if(categoryMap[cat].isNil, {
                categoryMap[cat] = List.new;
            }, {
                categoryMap[cat] = categoryMap[cat].reject {|t| t.path==path}.asList; //remove old
            });
            categoryMap[cat].add((path:path, summary:parser.findNode(\summary).text));
        };
    }
    
    makeDocMap {
        docMap = Dictionary.new;
        categoryMap.pairsDo {|k,v|
            v.do {|f|
                docMap[f.path] = f;
            };
        };
    }

    readCategoryMap {
        var path = helpTargetDir +/+ "scdoc_category_cache";
        var file;
        if(File.exists(path), {
            file = File.open(path,"r");
            categoryMap = file.readAllString.interpret;
            file.close;
            ^false;
        }, {
            categoryMap = Dictionary.new;
            ^true;
        });
    }

    writeCategoryMap {
        var path = helpTargetDir +/+ "scdoc_category_cache";
        var file = File.open(path,"w");
        file.write(categoryMap.asCompileString);
        file.close;
    }
*/
    readDocMap {
        var path = helpTargetDir +/+ "scdoc_cache";
        var file;
        if(File.exists(path), {
            file = File.open(path,"r");
            docMap = file.readAllString.interpret;
            file.close;
            ^false;
        }, {
            docMap = Dictionary.new;
            ^true;
        });
    }

    writeDocMap {
        var path = helpTargetDir +/+ "scdoc_cache";
        var file = File.open(path,"w");
        file.write(docMap.asCompileString);
        file.close;
    }

    updateAll {|force=false|
        var p = ScDocParser.new;
        var r = ScDocRenderer.new;

        if(force.not, {
            force = this.readDocMap;
        }, {
            docMap = Dictionary.new;
        });

        PathName(helpSourceDir).filesDo {|path|
            var source = path.fullPath;
            var lastDot = source.findBackwards(".");
            var subtarget = source.copyRange(helpSourceDir.size+1,lastDot-1);
            var target = helpTargetDir +/+ subtarget ++".html";
            var folder = target.dirname;
            var ext = source.copyToEnd(lastDot);
//            PathName(folder).makeDir;
            //FIXME: if source is newer than target:
            if(ext == ".schelp", {
                if(force or: (("test"+source.escapeChar($ )+"-ot"+target.escapeChar($ )).systemCmd!=0), { //update only if needed
                    ("Rendering" + source + "to" + target).postln;
                    r.parser = p.parseFile(source);
//                    this.addToCategoryMap(p,subtarget); //we need to parse it to get the categories and stuff..
                    this.addToDocMap(p,subtarget);
                    r.renderHTML(target,subtarget.dirname);
                });
            }, {
                ("Copying" + source + "to" + folder).postln;
                ("mkdir -p"+folder.escapeChar($ )).systemCmd;
                ("cp" + source.escapeChar($ ) + folder.escapeChar($ )).systemCmd;
            });
        };
//        this.writeCategoryMap;
        this.writeDocMap;
        this.handleUndocumentedClasses(force);
        this.makeCategoryMap;
        this.makeOverviews;
        "Done".postln;
    }
}

+ String {
    stripWhiteSpace {
        var a=0, b=this.size-1;
        //FIXME: how does this handle strings that are empty or single characters?
        while({(this[a]==$\n) or: (this[a]==$\ )},{a=a+1});
        while({(this[b]==$\n) or: (this[b]==$\ )},{b=b-1});
        ^this.copyRange(a,b);
    }
}


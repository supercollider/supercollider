ScDocParser {
    var <root;
    var tree;
    var stack;
    var current;
    var singleline;
    var level;
    var modalTag;
    var lastTagLine;
    var isWS;

    init {
        root = tree = List.new;
        stack = List.new;
        stack.add([tree,0]);
        current = nil;
        singleline = false;
        level = 0;
        modalTag = nil;
        isWS = false;
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

    addTag {|tag, text="", children=false|
        this.endCurrent;
        tag = tag.asString.drop(-2).asSymbol;
        current = (tag:tag, display:\block, text:text, children:if(children,{List.new},{nil}));
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
            },{
                if(word == ("\\"++modalTag.asString),{ //allow escaped end-tag
                    this.addText(word.drop(1))
                },{
                    if(("[a-zA-Z]+::".matchRegexp(word)) and: (lastTagLine==lineno), { //split unhandled tag-like word
                        this.addText(word.drop(-2));
                        this.handleWord("::",lineno,wordno+1);
                    },{
                        this.addText(word);
                    });
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
                'link::',               modalRangeTag,

                'list::',               listEnter,
                'tree::',               listEnter,
                'numberedlist::',       listEnter,
                'definitionlist::',     listEnter,
                'table::',              listEnter,
//                'row::', {
//                    singleline = false;
//                    this.enterLevel(11);
//                    this.addTag(tag,nil,true);
//                },
                '##', {
                    singleline = false;
                    this.addTag('##::',nil,false); //make it look like an ordinary tag since we drop the :: in the output tree
                },
                '||', {
                    singleline = false;
                    this.addTag('||::',nil,false);
                },
                '::', { //ends tables and lists
//                    this.leaveLevel(10);
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
        if(current.notNil, { // add to current element text
            current.text = current.text ++ word
        },{ // no current element, so add to new 'prose' element
            if(isWS.not, { //don't start a new prose element with whitespace
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
                    isWS = "[ \n\t]+".matchRegexp(word);
                    this.handleWord(word,l,w);
                    if(isWS.not,{w=w+1});
                },{
                    split2.do {|e2|
                        this.handleWord(e2[1],l,w);
                        w=w+1;
                    };
                });
            };
            if(modalTag.isNil and: split.isEmpty, { this.endCurrent }); //force a new prose on double blank lines
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
        r.add((tag:'related', text:"Overviews/Classes Overviews/ClassesByCategories Overviews/Methods"));
        r.add((tag:'categories', text:"Classes"));
        r.add(n);
        this.dumpClassTree(n,Object);
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

    *new {|p=nil|
        ^super.newCopyArgs(p).init;
    }

    init {
    }

    renderHTMLSubTree {|file,node,parentTag=false|
        var f, m, mname, args, split;

        var do_children = {
            if(node.children.notNil, {
                node.children.do {|e| this.renderHTMLSubTree(file,e,node.tag) };
            });
        };

        switch(node.tag,
            'prose', {
                if(last_display == \block, {
                    file.write("<p>"++node.text);
                }, {
                    file.write(node.text);                
                });
            },
            'section', {
                file.write("<h2>"++node.text++"</h2>\n");
                do_children.();
            },
            'subsection', {
                file.write("<h3>"++node.text++"</h3>\n");
                do_children.();
            },
            'classmethods', {
                file.write("<a name='classmethods'><h2>Class Methods</h2></a>\n<div id='classmethods'>");
                do_children.();
                file.write("</div>");
            },
            'instancemethods', {
                file.write("<a name='instancemethods'><h2>Instance Methods</h2></a>\n<div id='instancemethods'>");
                do_children.();
                file.write("</div>");
            },
            'method', {
                //for multiple methods with same signature and similar function:
                //ar kr (x = 42, y = 123)
                
                f = node.text.findRegexp(" *([^(]+) *(\\(.*\\))?");
                args = f[2][1];
                split = f[1][1].findRegexp("[^ ,]+");
                split.do {|r|
                    mname = r[1];

                    if(args.isEmpty, {
                        args = "NOT FOUND";
                        if(parentTag==\instancemethods,{
                            m = currentClass.findRespondingMethodFor(mname.asSymbol);
                        },{
                            m = currentClass.class.findRespondingMethodFor(mname.asSymbol);
                        });
                        if(m.notNil, { args = m.argumentString.replace("this, ","").replace("this","") });
                        if(args.notEmpty,{ args = " ("++args++")" });
                    });
                                    
                    file.write("<a name='"++mname++"'><h3 class='methodname'>"++mname++args++"</h3></a>\n");
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
                file.write("<div class='note'><span class='notelabel'>NOTE:</span> "++node.text++"</div>");
            },
            'warning', {
                file.write("<div class='warning'><span class='warninglabel'>WARNING:</span> "++node.text++"</div>");
            },
            'emphasis', {
                file.write("<em>"++node.text++"</em>");
            },
            'link', {
                if("[a-zA-Z]+://.+".matchRegexp(node.text),{
                    file.write("<a href=\""++node.text++"\">"++node.text++"</a>");
                },{
                    file.write("<a href=\""++baseDir +/+ node.text++".html\">"++node.text.split($/).last++"</a>");
                    //FIXME: need to have relative uri's
                    //best would be to keep track, have a currentDir class variable.
                });
            },
            'code', {
                if(node.display == \block, {
                    file.write("<pre>"++node.text++"</pre>\n");
                }, {
                    file.write("<code>"++node.text++"</code>\n");
                });
            },
            
            'list', {
                file.write("<ul>\n");
                do_children.();
                file.write("</ul>\n");
            },
            'tree', {
                file.write("<ul class='tree'>\n");
                do_children.();
                file.write("</ul>\n");
            },
            'definitionlist', {
                file.write("<dl>\n");
                do_children.();
                file.write("</dl>\n");
            },
            'numberedlist', {
                file.write("<ol>\n");
                do_children.();
                file.write("</ol>\n");
            },
            'table', {
                file.write("<table>\n");
                do_children.();
                file.write("</table>\n");
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
                if(node.text.notNil,{file.write(node.text)});
            }
        );
        last_display = node.display;
    }
    
    renderHTMLHeader {|f,name,type,folder|
        var x, cats;
        var style = baseDir +/+ "scdoc.css";
//        dirLevel.do { style = style ++ "../" };
//        style = style ++ "scdoc.css";
        f.write("<html><head><title>"++name++"</title><link rel='stylesheet' href='"++style++"' type='text/css' /></head><body>");

        cats = parser.findNode(\categories).text.findRegexp("[^ ,]+").flop[1];
        cats = if(cats.notNil, {cats.join(", ")}, {""});
        if(folder==".",{folder="SuperCollider"});
        f.write("<div class='header'>");
        f.write("<div id='label'>"++folder.asString.toUpper++"</div>");
//        f.write(if(type==\class,{"SC CLASS"},{"SC DOC"}));
        f.write("<div id='categories'>"++cats++"</div>");
        f.write("<h1>"++name++"</h1>");
        x = parser.findNode(\summary);
        f.write("<div id='summary'>"++x.text++"</div>");
        f.write("</div>");

        f.write("<div class='subheader'>\n");
        if(type==\class,{
            f.write("<div id='inheritance'>");
            f.write("Inherits from ");
            currentClass.superclasses.do {|c|
                f.write(": <a href=\"../Classes/"++c.name++".html\">"++c.name++"</a> ");
            };
            f.write("</div>");
        });
        
        x = parser.findNode(\related);
        if(x.text.notEmpty, {
            f.write("<div id='related'>");
            f.write("See also: ");
            x.text.findRegexp("[^ ,]+").flop[1].do {|r,i|
                //FIXME: ignore superclasses? since they are already in "inherits from"...
                if(i>0, {f.write(", ")});
                f.write("<a href=\""++baseDir +/+ r++".html\">"++r.split($/).last++"</a>");
            };
            f.write("</div>");
        });
        f.write("</div>");
    }

    renderHTML {|filename, folder="."|
        var f,x,name;

        ("mkdir -p"+filename.dirname).systemCmd;
                
        f = File.open(filename, "w");
        x = parser.findNode(\class);
        
        last_display = \block;

        //folder is the directory path of the file relative to the help tree,
        //like 'Classes' or 'Tutorials'.
        dirLevel = folder.split($/).reject {|y| (y.size<1) or: (y==".")}.size;
        baseDir = ".";
        dirLevel.do { baseDir = baseDir +/+ ".." };
        
//        ("'"++baseDir++"'").postln;
        
        if(x.text.notEmpty, {
            name = x.text.stripWhiteSpace;
            currentClass = name.asSymbol.asClass;
            this.renderHTMLHeader(f,name,\class,folder);
                    
            x = parser.findNode(\description);
            this.renderHTMLSubTree(f,x);

            x = parser.findNode(\classmethods);
            this.renderHTMLSubTree(f,x);

            x = parser.findNode(\instancemethods);
            this.renderHTMLSubTree(f,x);

            x = parser.findNode(\examples);
            this.renderHTMLSubTree(f,x);
            
            f.write("</body></html>");
        },{
            x = parser.findNode(\title);
            name = x.text.stripWhiteSpace;
            this.renderHTMLHeader(f,name,\other,folder);
        
            this.renderHTMLSubTree(f,(tag:'root',children:parser.root));
            f.write("</body></html>");
        });
        f.close;
    }
}

ScDoc {
    var <>helpTargetDir;
    var <>helpSourceDir;
    
    *new {
        ^super.new.init;
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
          All documents by categories
          All classes alphabetically
          All classes by categories
          All undocumented classes
          All ugens alphabetically
          All ugens by categories
          All methods index
        */

        r.parser = p.overviewClassTree;
        r.renderHTML(helpTargetDir +/+ "Overviews/ClassTree.html","Overviews");
    }

    updateAll {
        var p = ScDocParser.new;
        var r = ScDocRenderer.new;
        PathName(helpSourceDir).filesDo {|path|
            var source = path.fullPath;
            var lastDot = source.findBackwards(".");
            var subtarget = source.copyRange(helpSourceDir.size+1,lastDot-1)++".html";
            var target = helpTargetDir +/+ subtarget;
            var folder = target.dirname;
            var ext = source.copyToEnd(lastDot);
//            PathName(folder).makeDir;
//            ("mkdir -p"+folder).systemCmd;
            //FIXME: if source is newer than target:
            if(ext == ".schelp", {
                ("Rendering" + source + "to" + target).postln;
                r.parser = p.parseFile(source);
                r.renderHTML(target,subtarget.dirname);
            }, {
                ("Copying" + source + "to" + folder).postln;
                ("cp" + source + folder).systemCmd;
            });
        };
        this.makeOverviews;
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


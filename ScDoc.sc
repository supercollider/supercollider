ScDocParser {
    var <root;
    var tree;
    var stack;
    var current;
    var singleline;
    var level;
    var modalTag;
    var lastTagLine;
    var doingInlineTag;
    
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
        while({level>=n},{
            var p = stack.pop;
            tree = p[0];
            level = p[1];
        });
    }

    enterLevel {|n|
        this.leaveLevel(n);
        stack.add([tree,level]);
        level = n;
    }

    stripWhiteSpace {|str|
        var a=0, b=str.size-1;
        //FIXME: how does this handle strings that are empty or single characters?
        while({(str[a]==$\n) or: (str[a]==$\ )},{a=a+1});
        while({(str[b]==$\n) or: (str[b]==$\ )},{b=b-1});
        ^str.copyRange(a,b);
    }
    
    endCurrent {
        if(current.notNil,{
            if(current.text.notNil, {
                current.text = this.stripWhiteSpace(current.text);
            });
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
            singleline = true;
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
        var rangeTag = {
            singleline = false;
            this.addTag(tag);
            lastTagLine = lineno;
            doingInlineTag = true;
        };
        var closingTag = {
            this.endCurrent;
        };
        var listEnter = {
            singleline = false;
//            this.enterLevel(10);
            stack.add([tree,level]);
            this.addTag(tag,nil,true);
            lastTagLine = lineno;
        };
//        var listLeave = {
//            this.leaveLevel(10);
//            this.endCurrent;
//        };

        // modal tags ignore all other tags until their closing tag occurs.
        if(modalTag.notNil, {
//            current.display = if(lastTagLine==lineno,\inline,\block);
//            if((tag==modalTag) and: ((wordno==0) or: (current.display==\inline)),{
//                this.endCurrent;
//                modalTag = nil;
            if(tag==modalTag, {
                if(lastTagLine==lineno,{
                    current.display = \inline;
                    this.endCurrent;
                    modalTag = nil;
                },{
                    if(wordno==0,{
                        current.display = \block;
                        this.endCurrent;
                        modalTag = nil;
                    });
                }); 
            },{
                if(word == ("\\"++modalTag.asString),
                    {this.addText(word.drop(1))},
                    {this.addText(word)});
            });
        },{
            switch(tag,
                'description::',        noNameSection, //level 1
                'methods::',            noNameSection,
                'examples::',           noNameSection,
//                'introduction::',       noNameSection,
                'section::',            namedSection.(1),
                'subsection::',         namedSection.(2),
                'classmethod::',        namedSection.(3),
                'instancemethod::',     namedSection.(3),
                'argument::',           namedSection.(4),
                'class::',              simpleTag,
                'title::',              simpleTag,
                'summary::',            simpleTag,
                'related::',            simpleTag,
                'keywords::',           simpleTag,
                'doctype::',            simpleTag,
                'note::',               simpleTag,
                'warning::',            simpleTag,

                'code::', {
                    singleline = false;
                    this.addTag(tag);
                    modalTag = '::';
                    lastTagLine = lineno;
                },
//                'code[[', {
//                    singleline = false;
//                    this.addTag(tag);
//                    modalTag = ']]';
//                },
                
                'list::',               listEnter,
//                '::list',               listLeave,
                'numberedlist::',       listEnter,
//                '::numberedlist',       listLeave,
                'table::',              listEnter,
//                '::table',              listLeave,
                'row::', {
                    singleline = false;
//                    this.enterLevel(11);
//                    this.addTag(tag,nil,true); //with children
                    this.addTag(tag,nil,false); //as separator
                },
                '##', {
                    singleline = false;
//                    this.enterLevel(12);
//                    this.addTag('##::',nil,true); //make it look like an ordinary tag since we drop the :: in the output tree
                    this.addTag('##::',nil,false); //make it look like an ordinary tag since we drop the :: in the output tree
                },

                'emphasis::',           rangeTag,
                'link::',               rangeTag,
                '::', {
/*                    if(lastTagLine==lineno,{
                        current.display = \inline;
                        this.endCurrent;
                    },{
                        if(wordno==0, { //perhaps not needed since code is modal
                            current.display = \block;
                            this.endCurrent;
                        },{
                            this.addText("::");
                        });
                    });*/
//                    this.leaveLevel(10); //this is not right, we should not leave a list if this was closing an emphasis or something.. need to use the stack!
                    if(doingInlineTag,{
                        doingInlineTag = false;
                    },{
                        var p = stack.pop;
                        tree = p[0];
                        level = p[1];
                    });
                    current.display = if(lastTagLine==lineno,\inline,\block);
                    this.endCurrent;
                },

                '\\::', {
                    this.addText("::");
                },
                
                { //default
                    if("[a-zA-Z]+://.+".matchRegexp(word),{
                        this.addTag('link::',word,false);
                        current.display = \inline;
                        this.endCurrent;
                    },{
                        this.addText(word);
                    });
                }
            );
        });
    }
    
    addText {|word|
        // add to current element text, or if no current element: add to new 'prose' element
        if(current.notNil,
            {current.text = current.text + word},
            {
//                if(level==0,{ //if we're before first section, add an introduction section
//                    this.enterLevel(1);
//                    this.addTag('introduction::',nil,true);
//                });
                singleline = false;
                this.addTag('prose::', word)
            }
        );
    }

    endLine {
        if(singleline,{this.endCurrent});
        // pass through newlines for vari-line tags.
        if(current.notNil,{current.text = current.text ++ "\n"});
    }
    
    init {
        root = tree = List.new;
        stack = List.new;
        stack.add([tree,0]);
        current = nil;
        singleline = false;
        level = 0;
        modalTag = nil;
        doingInlineTag = false;
    }
    
    parse {|string|
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
    }

    parseFile {|filename|
        var file = File.open(filename,"r");
        this.parse(file.readAllString);
        file.close;
    }

    dumpSubTree {|t,i="",lev=1|
        t.do {|e|
            "".postln;
            (i++"LEVEL:"+lev).postln;
            (i++"TAG:"+e.tag).postln;
            (i++"DISPLAY:"+e.display).postln;
            (i++"TEXT:"+e.text).postln;
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

    renderHTML {|filename|

    }
}


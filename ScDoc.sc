ScDocParser {
    var <filename;

    var <root;
    var tree;
    var stack;
    var current;
    var singleline;
    var level;
    
    *new {|filename|
        ^super.newCopyArgs(filename).init;
    }
    
//    isTag {|word| ^"^(::[a-zA-Z]+|[a-zA-Z]+::)$".matchRegexp(word)}
//    isOpeningTag {|word| ^"^[a-zA-Z]+::$".matchRegexp(word)}
//    isClosingTag {|word| ^"^::[a-zA-Z]+$".matchRegexp(word)}

    enterLevel {|n|
        while({level>=n},{
            tree = stack.pop;
            level = level - 1;
        });
        stack.add(tree);
        level = n;
    }
    
    endCurrent {
        if(current.notNil,{
            if(current.text.notNil, {
//                current.text = current.text.replace("^[ \n]+|[ \n]+$","");
// too bad, replace doesn't take regexp.. we need a strip-whitespace function.
            });
            current = nil;
        });
    }
    
    addTag {|tag, text="", children=false|
        current = (tag:tag, text:text, children:if(children,{List.new},{nil}));
        tree.add(current);
        if(children, {tree = current.children}); //recurse into children list
        if(text.isNil, {this.endCurrent}); //we don't have any text field to add to for this tag, so start fresh..    
    }

    handleWord {|word,ofs=0|
        var tag = word.toLower.asSymbol;
        var docAttribute = {
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

        switch(tag,
            'description::',        noNameSection,
            'methods::',            noNameSection,
            'classmethod::',        namedSection.(2),
            'instancemethod::',     namedSection.(2),
            'argument::',           namedSection.(3),
            'section::',            namedSection.(1),
            'subsection::',         namedSection.(2),
            'class::',              docAttribute,
            'title::',              docAttribute,
            'summary::',            docAttribute,
            
            { //default
                this.addText(word);
            }
        );
    }
    
    addText {|word|
        // add to current element text, or if no current element: add to new 'prose' element
        if(current.notNil,
            {current.text = current.text + word},
            {singleline = false; this.addTag('prose::', word)}
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
        stack.add(tree);
        current = nil;
        singleline = false;
        level = 0;
    }
    
    parse {
        var file = File.open(filename,"r");
        var lines = file.readAllString.split($\n);
        lines.do {|line|
            var words = line.split($\ );
            words.do {|word,i|
                this.handleWord(word,i);
            };
            this.endLine;
        }
    }

    dumpSubTree {|t,i=""|
        t.do {|e|
            (i++"TAG:"+e.tag).postln;
            (i++"TEXT:"+e.text).postln;
            if(e.children.notNil, {
                (i++"CHILDREN:").postln;
                this.dumpSubTree(e.children,i++"    ");
            });
        }
    }
    
    dump {
        this.dumpSubTree(root);
        ^nil;
    }
}


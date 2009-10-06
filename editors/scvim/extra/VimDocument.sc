Vim {
    *new { this.shouldNotImplement(thisMethod) }
    *serverlist { ^"vim --serverlist".unixCmdGetStdOut }
    *remote_send { | servername keys | ^("vim --servername" + servername + "--remote-send" + "'" ++ keys ++ "'").unixCmdGetStdOut }
    *remote_expr { | servername expr | ^("vim --servername" + servername + "--remote-expr" + "'" ++ expr ++ "'").unixCmdGetStdOut }
}

VimDocument : Document {
    classvar idx = 0;
    var <servername, title, <file;

    *initClass{
        "rm -f /tmp/SCVIM*".systemCmd;
        Document.implementationClass = VimDocument;
        Document.autoRun = false;
        this.startup;
    }

    *startup { 
        var post;
        super.startup; 
        fork({ 
            0.2.wait; 
            post = this.listener; 
            if(post.notNil) { post.name_(" post ") } 
        }, AppClock);
        this.setTheme('default');
    }

    *new { | title="Untitled" string="" makeListener=false |
        ^super.prBasicNew.initByString(title, string.asString, makeListener);
    }

    *postColor_ { }

    initLast { }
    front { }
    unfocusedFront { }
    alwaysOnTop_ { |boolean=true| }
    alwaysOnTop { ^false }
    syntaxColorize { }
    removeUndo { }
    promptToSave_ { | bool | }
    promptToSave { ^true }    
    underlineSelection { }
    isEdited { ^true }
    

    // if range is -1 apply to whole doc
    setFont { | font, rangeStart= -1, rangeSize=100 | 
		//XXX this only works for the whole server as that is the way vim works
		//only works with graphical vim
		Vim.remote_send(servername, "<Esc>:set guifont=" ++ 
			font.replace(" ", "\\\\\\ ")  ++ "<CR>");
	}
    setTextColor { | color,  rangeStart = -1, rangeSize = 0 | }
    
    text {
        var string
			= Vim.remote_expr(servername, "getbufline(bufnr(\"" ++ file ++ "\"), 1, \"$\")");
        ^string.copyRange(0, (string.size - 2));
    }

    selectedText { ^nil }
    selectUnderlinedText { | clickPos | }
    selectedRangeLocation { ^0 }
    selectedRangeSize { ^0 }
    
    rangeText { | rangestart=0, rangesize=1 |
        var string = 
			Vim.remote_expr(servername, "getbufline(bufnr(\"" ++ file ++ "\"), " ++ 
			(rangestart + 1) ++ ", " ++
			(rangestart + rangesize) ++ ")");
        ^string.copyRange(0, (string.size - 2));
    }
    
    insertTextRange { | string, rangestart, rangesize | }
    linkAtClickPos { | clickPos | ^nil }

    // private //
    *prnumberOfOpen { ^0 }
    *prGetIndexOfListener { ^0 }
    *prSetSyntaxColorTheme{ |textC, classC, stringC, symbolC, commentC, numberC| }

    propen { | path selectionStart=0 selectionLength=0 |
        var rcfile = this.prfindRcFile;
		file = path;
        servername = "scvim";//this.prnextVimServerName;
		("vim --servername " + servername + " --remote-tab " + file).unixCmd;
		//XXX I'm not sure what this means
        dataptr = true;
    }
    prclose { }

    prinitByString { | title str makeListener |
        var fifo;
		file = this.prGetTmpFile;
        servername = "scvim";
        File.open(file, "w").write(str).close;
		this.propen(file);
		//XXX I'm not sure what this means
        dataptr = true;
    }

    prinitByIndex { | idx | }
    prGetLastIndex { ^idx }
    prIsEditable_{ | editable=true | }
    prSetTitle { | argName | title = argName }
    prGetTitle { ^title }  
    prGetFileName { ^nil }
    prSetFileName { | apath | }

    prSelectLine { | line | }
    prinsertText { | dataPtr, txt | }

    prGetBounds { | argBounds | ^nil }
    prSetBounds { | argBounds | }
    prSetBackgroundColor { | color | }
    prGetBackgroundColor { | color | ^nil }
    prSetSelectedBackgroundColor { | color | }
    prGetSelectedBackgroundColor { | color | ^nil }

	prGetTmpFile {
		var tmp = "/tmp/scvimdocument-tmp" ++ idx ++ ".sc";
		idx = idx + 1;
		^tmp;
	}

    prfindRcFile {
        var homercfile = "~/.scvimrc";
        var localrcfile = "/usr/local/share/scvim/scvimrc";
        var usrrcfile = "/usr/share/scvim/scvimrc";

        if (File.exists(homercfile)) {
            ^homercfile;
        } {
            if (File.exists(localrcfile)) {
                ^localrcfile;
            } {
                if (File.exists(usrrcfile)) {
                    ^usrrcfile;
                } {
                    ^nil;
                }
            }
        }
    }
}

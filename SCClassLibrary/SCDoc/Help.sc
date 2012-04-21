Help {
	// These classvars and vars are part of the deprecated Help.tree stuff. When that stuff is removed from the classlib, this can be removed too.
	classvar <global, categoriesSkipThese;
	classvar <filterUserDirEntries, <>cachePath;
	var	tree, fileslist, <root;



	*gui {
		HelpBrowser.instance.goHome;
	}
	*dir {
		^SCDoc.helpTargetDir
	}
    *methodArgs {|string|
        var class, meth, f, m;
        f = string.findRegexp("(\\w*)\\.(\\w+)").flop[1];
        if(f.notNil) {#class, meth = f[1..]} {
            if(string[0].isUpper) {
                class = string;
                meth = \new;
            } {
                meth = string;
            }
        };
        f = {|m,c|
            class = (c ?? {m.ownerClass}).name;
            class = if(class.isMetaClassName) {class.asString[5..]++" *"} {class.asString++" -"};
            class++m.name++" ("++m.argNames[1..].collect {|n,i|
                n.asString++":"+m.prototypeFrame[i+1];
            }.join(", ")++")";
        };
        class = class.asSymbol.asClass;
        if(class.notNil) {
            m = class.class.findRespondingMethodFor(meth.asSymbol);
            ^if(m.notNil) {f.value(m,class.class)} {""};
        } {
            ^Class.allClasses.reject{|c|c.isMetaClass}.collect {|c|
                c.findMethod(meth.asSymbol);
            }.reject{|m|m.isNil}.collect {|m|
                f.value(m);
            }.join($\n);
        }
    }

}


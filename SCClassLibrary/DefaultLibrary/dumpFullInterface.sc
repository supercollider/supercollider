+ Class {

	dumpFullInterface {

		("\nFull Interface for " ++ this.name).postln;

		// post the superclasses
		("\nSuperclasses: " ++ this.superclasses).postln;

		// Instance methods
		this.dumpAllMethods;

		// Class methods
		this.class.dumpAllMethods;
	}

	dumpAllMethods {
		var methodlist, superclasses, prependString, superPrependString, name;
		methodlist = IdentitySet[];
		if(this.isMetaClass,
			{	prependString = "\nClass Methods for ";
				superPrependString = "\nClass Methods inherited from ";
				name = this.asString.copyToEnd(5);
				superclasses = name.asSymbol.asClass.superclasses;
			},
			{	prependString = "\nInstance Methods for ";
				superPrependString = "\nInstance Methods inherited from ";
				name = this.name;
				superclasses =  this.superclasses;
			}
		);
		(prependString ++ name ++ "\n").postln;
		this.methods.do({ arg meth;
			var numargs, methname;
			methname = meth.name;
			methodlist.add(methname);
			numargs = meth.argNames.size - 1;
			"   ".post;
			methname.post;
			" ( ".post;
			meth.argNames.do({ arg name, i;
				if (i > 0, { // skip 'this'
					name.post;
					if (i < numargs, {
						", ".post;
					});
				});
			});
			" )\n".post;
		});
		// Methods for superclasses
		superclasses.do({ arg superclass, superobject, supername;
			if(this.isMetaClass,
				{
					superobject = superclass.class;
				},
				{
					superobject = superclass;
				}
			);
			supername = superobject.asString;
			if(supername.containsStringAt(0, "Meta_"), { supername = supername.copyToEnd(5) });
			(superPrependString ++ supername ++ "\n").postln;
			superobject.methods.do({ arg meth;
				var numargs, methname;
				methname = meth.name;
				if(methodlist.includes(methname).not, {
					methodlist.add(methname);
					numargs = meth.argNames.size - 1;
					"   ".post;
					methname.post;
					" ( ".post;
					meth.argNames.do({ arg name, i;
						if (i > 0, { // skip 'this'
							name.post;
							if (i < numargs, {
								", ".post;
							});
						});
					});
					" )\n".post;
				});
			});

		});
		// include methods for Class

		if(this.isMetaClass, {"\nMethods inherited from Class\n".postln; Class.dumpInterface; });

	}

	dumpMethodList {
		var	mList, sc;

		mList = IdentityDictionary.new;		// repository for methods
		this.collectMethods(mList);			// get them

		sc = this;	// to print superclass chain
		{ sc != Object }.while({
			(sc.name ++ " : ").post;
			sc = sc.superclass;
		});
		"Object".postln;

		mList.asSortedArray.do({ |pair|
			(pair[0] ++ " <" ++ pair[1].ownerClass.name ++ "-"
				++ pair[0] ++ ">").post;
			(pair[1].argNames.size > 1).if({
				" (".post;
				pair[1].argNames.do({ |argname, i|
					(i > 1).if({ ", ".post });
					(i > 0).if({ argname.post; });
				});
				")".post;
			});
			"".postln;
		});
	}

	collectMethods { arg list;
			// only collect if not Object or Class
		((this.name != \Object) && (this.name != \Class)).if({			this.methods.do({ |meth|
					// if keys already includes methodname,
					// then a subclass has overridden this superclass method, so don't add
				list.keys.includes(meth.name).not.if({
					list.put((meth.name.asString).asSymbol, meth);
				});
			});
			superclass.asClass.collectMethods(list);  // go up a level
		});
	}

	helpFileForMethod {
		arg methodSymbol;
		this.findRespondingMethodFor(methodSymbol).ownerClass.help;
	}

		// show all subclasses of this class sorted in alpha order (not tree order)
	dumpSubclassList {
		var list, listCollector;
			// recursive function to collect class objects
		listCollector = { arg node, l;
			l.add(node);
			node.subclasses.do({ arg n; listCollector.value(n, l) });
		};
		list = List.new;
		listCollector.value(this, list);	// do the recursion
		list.sort({ arg a, b; a.name < b.name })	// sort it
			.do({ arg n;		// and iterate to post the class names (w/ supers)
			n.name.post;
			n.superclasses.do({ arg s; (" : " ++ s.name).post; });
			"\n".post;
		});
		("\n" ++ list.size.asString ++ " classes listed.").postln;
	}

}

+ Class {
	
	dumpFullInterface {
		
		("Full Interface for " ++ this.name).postln;
		
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
				if(methodlist.includes.not, {
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
}
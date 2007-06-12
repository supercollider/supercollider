
CXObjectInspector : ObjectGui {

	
	guify { arg layout,title,width,height;
		^super.guify(layout,title,width ? 800,height ? 800)
	}
	writeName { arg layout;
		ClassNameLabel.newBig(model.class,layout);
		CXLabel(layout,model.asString,300,30)
			.font_(Font("Helvetica-Bold",18));

		/*
		ActionButton(layout,"-> a-z",{
			GetStringDialog("assign to interpreter variable a-z","x",{ arg ok,string;
				if(ok,{
					thisProcess.interpreter.performList((string ++ "_").asSymbol,model);
					this.newErrorWindow;
					"".postln;
					string.postln;
				})
			})
		});
		*/
	}

	guiBody { arg layout;
		var vert;
		this.instVarsGui(layout);
		
		// slotAt
		if(model.isArray,{
			vert = model.slotSize < 26;
			min(model.slotSize,300).do({arg i;
				var iv;
				if(vert or: {i % 4 == 0},{ layout.startRow; });
				
				CXLabel(layout,"@" ++ i,maxx: 40);
				iv=model.slotAt(i);
//				ActionButton(layout,"code->",{
//					GetStringDialog("enter code to compile and insert at slot " 
//						+ i ,"",
//					{ arg ok,string;
//						if(ok,{
//							model.slotPut(i,  string.interpret)
//						})
//					})
//				});
	
				//ClassNameLabel(iv.class,layout);
				InspectorLink(iv,layout);
			});
			if(model.slotSize > 300,{ 
				CXLabel(layout,"... slotSize is" ++ model.slotSize.asString,maxx:210).bold;
			});
		});
		this.dependantsGui(layout);
		this.actionsGui(layout);
	}
	
	instVarsGui { arg layout;
	
		var iNames;

		//instVars
		iNames=model.class.instVarNames;
		if(iNames.notNil,{
			iNames.do({arg v,i;
				var iv;
				layout.startRow;
				VariableNameLabel(v,layout);
				/*
				ActionButton(layout,"code->",{
					GetStringDialog("enter code to compile and insert to " 
							+ v.asString,"",
					{ arg ok,string;
						if(ok,{
							model.instVarPut(i,  string.interpret)
						})
					})
				});
				*/
				iv=model.instVarAt(i);
				//ClassNameLabel(iv.class,layout);
				InspectorLink(iv,layout,300);
			});
		});
	}
	dependantsGui { arg layout;
		layout.hr;
		// dependants
		CXLabel(layout.startRow,"dependants:",maxx:210).bold;
		model.dependants.do({ arg d;
			InspectorLink(d,layout);
		});
		// uniqueMethods
	}
	actionsGui { arg layout;	
		CXLabel(layout.startRow,"actions:",maxx:210).bold;
		ActionButton(layout,"gui",{ model.topGui });
		ActionButton(layout,"post asCompileString",{ model.asCompileString.postln });
	}
}

ClassGui : CXObjectInspector { // ClassGui

	writeName {}

	guiBody { arg layout;
	
		var iNames,supers,scale;
		layout.hr;
	
		// you are here
		InspectorLink.big(model,layout.startRow,maxx:200);

		supers = model.superclasses;
		if(supers.notNil,{
			scale = supers.size;
			supers.do({ arg sup,i;
				ClassNameLabel(sup,layout,100);//.labelColor_(Color.grey(1.0 - (i / scale))
			})
		});


		ActionButton(layout,"VIEW SOURCE",{
			model.openCodeFile;
		}).font_(Font("Monaco",9.0));	
		ActionButton(layout,"openHelpFile",{ model.openHelpFile });
	 	//ActionButton(layout,"dumpInterface",{ 
	 	//	model.newErrorWindow.dumpInterface 
	 	//});

		// subclasses
		// needs a scroll view
		layout.indent(1);
		this.displaySubclassesOf(model,layout,0,50);
		layout.indent(-1);
		
		layout.hr;		
		
		// explicit references
		/*
		CXLabel(layout.startRow,"classes explicitly referenced in source:");
		layout.startRow;
		model.explicitClassReferences.do({ arg c;
			ClassNameLabel(c.asClass,layout,200);
		});
		*/
		
		
		// classVarnames 
		CXLabel(layout.startRow,"classvars:",maxx:210).bold;
		model.classVarNames.size.do({ arg cvi;
			var iv;
			VariableNameLabel(model.classVarNames.at(cvi),layout.startRow);
			iv=model.classVars.at(cvi);
			//ClassNameLabel(iv.class,layout);
			InspectorLink(iv,layout);
		});
		
		// meta_class methods
		CXLabel(layout.startRow,"class methods:",maxx:210).bold;
		model.class.methods.size.do({arg cmi;
			MethodLabel(model.class.methods.at(cmi),layout.startRow,maxx:210);
		});
	
		
		//instance vars
		CXLabel(layout.startRow,"instance vars:",maxx:210).bold;
		model.instVarNames.size.do({ arg ivi;
			VariableNameLabel(model.instVarNames.at(ivi),layout.startRow,maxx:210);
			// iprototype
		});
		
		// cprototype
		// filenameSymbol
		// MethodBrowser(class)
		// show up to 50 methods, up the superclass chain (stop at object ?)
		// show when methods are overriden by subclass
		if(model.methods.size < 50,{
			this.displayMethodsOf(model,layout,true);
		},{
			ActionButton(layout.startRow,"display instance methods (" + model.methods.size + ")",{
				this.displayMethodsOf(model);
			})
		});
		
		this.dependantsGui(layout);
	}
	
	
	displayMethodsOf { arg class,f,withoutClass = false;
		f = f.asFlowView;//PageLayout.guify(f,"instance methods of " ++ class.name.asString);
		CXLabel(f.startRow,"instance methods:",maxx:210).bold;
		if(class.methods.notNil,{
			class.methods.do({ arg meth;
				if(withoutClass,{
					MethodLabel.withoutClass(meth,f.startRow,maxx:210)
				},{
					MethodLabel(meth,f.startRow,maxx:210);
				});
				class.superclasses.do({ arg superclass;
					var supermethod;
					supermethod = superclass.findMethod(meth.name);
					if(supermethod.notNil,{
						MethodLabel(supermethod,f,maxx:210)
					});
				})
			})
		});
		if(class.superclass.notNil,{ // not Object
			ActionButton(f.startRow,"display methods of superclass " ++ class.superclass.name.asString,{
				this.displayMethodsOf(class.superclass,f)
			})
		});
	}
	
	
	displaySubclassesOf { arg class,layout,shown,limit;
		var subclasses;
		if(class.subclasses.notNil,{
			subclasses = class.subclasses.as(Array)
				.sort({arg a,b; a.name.asString < b.name.asString});
			shown = shown + subclasses.size;
			subclasses.do({ arg c,i;
					layout.startRow;
					ClassNameLabel(c,layout,200);
					layout.indent(1);
					
					if(c.subclasses.size + shown < limit,{
						this.displaySubclassesOf(c,layout,shown,limit);
					},{
						if(c.subclasses.size > 0,{
							CXLabel(layout,c.subclasses.size.asString 
								+ " subclasses",maxx:80);
						});
					});
					layout.indent(-1);
				})
		});
	}

}


MethodGui : ObjectGui {

//	writeName { arg layout;
//		Tile(model.class,layout);
//		CXLabel(layout,model.asString);
//	}
	
	guiBody { arg layout;
		var prototypeFrame;
		var started=false,supers;
		
		layout.hr;
		
		// link to superclass implementations
		supers = model.ownerClass.superclasses;
		if(supers.notNil,{
			supers.reverse.do({ arg class;
				var supermethod;
				supermethod = class.findMethod(model.name);
				ClassNameLabel(class,layout.startRow,maxx:200);
				if(supermethod.notNil,{
				//	started = true;
					MethodLabel(supermethod,layout,maxx:200);
				});

			})
		});		
		
		layout.hr;
		ClassNameLabel(model.ownerClass,layout.startRow,maxx:200);
		MethodLabel(model,layout,maxx:200);

		// show subclass implementations
		// other class implementations of this message

		layout.hr;
		
		// all this stuff easiest seen from the source anyway
		
		// argNames
		prototypeFrame = model.prototypeFrame ?? {[]};
		
		CXLabel(layout.startRow,"args and defaults:",maxx:210).bold;
		(model.numArgs - 1).do({ arg i;// first arg is 'this'
			// TODO ellipsis ... 
			if(model.argNames.notNil,{
				ArgNameLabel(model.argNames.at(i + 1),layout.startRow);
				CXLabel(layout,prototypeFrame.at(i ));
			});
		});
		
		CXLabel(layout.startRow,"primitiveName:",maxx:210).bold;
		CXLabel(layout,model.primitiveName);
		
		layout.hr;
		
		ActionButton(layout.startRow,"VIEW SOURCE...",{ 
			model.openCodeFile;
		});
	}

}


DictionaryGui : ObjectGui {

	guiBody { arg layout;
		model.keysValuesDo({ arg k,v,i;
			CXLabel(layout.startRow,k,maxx: 100);
			Tile(v,layout,200);
		})
	}

}



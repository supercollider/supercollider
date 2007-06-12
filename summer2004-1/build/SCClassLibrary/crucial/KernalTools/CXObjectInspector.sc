
CXObjectInspector : ObjectGui {

	writeName { arg layout;
		ClassNameLabel.newBig(model.class,layout);
		//CXLabel(layout,model.asString,500,30)
		SCDragSource(layout,Rect(0,0,500,30))
			.object_(model)
			.background_(Color.white)
			.align_(\center)
			.font_(Font("Helvetica-Bold",18));
	}

	guiBody { arg layout;
		var vert;
		this.instVarsGui(layout);
		
		// slotAt
		if(model.isArray,{
			vert = model.slotSize < 26;
			min(model.slotSize,300).do({arg i;
				var iv;
				if(vert or: {i % 3 == 0},{ layout.startRow; });
				
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
				// drag in / out here
				InspectorLink(iv,layout,maxx:200);
			});
			if(model.slotSize > 300,{ 
				CXLabel(layout,"... slotSize is" ++ model.slotSize.asString,maxx:160).bold;
			});
		});
		this.dependantsGui(layout);
		this.actionsGui(layout);
	}
	
	instVarsGui { arg layout;
		var iNames;
		//identify which classes they come from, color code by fading
		iNames=model.class.instVarNames;
		if(iNames.notNil,{
			iNames.do({arg v,i;
				var iv;
				layout.startRow;
				VariableNameLabel(v,layout);
				/*ActionButton(layout,"code->",{
					GetStringDialog("enter code to compile and insert to " 
							+ v.asString,"",
					{ arg ok,string;
						if(ok,{
							model.instVarPut(i,  string.interpret)
						})
					})
				});*/
				iv=model.instVarAt(i);
				//ClassNameLabel(iv.class,layout);
				InspectorLink(iv,layout,400);
			});
		});
	}
	dependantsGui { arg layout;
		layout.hr;
		// dependants
		CXLabel(layout.startRow,"dependants:",maxx:160).bold;
		model.dependants.do({ arg d;
			InspectorLink(d,layout);
		});
		// uniqueMethods
	}
	actionsGui { arg layout;
		layout.startRow;
		ActionButton(layout,"gui",{ model.topGui });
		ActionButton(layout,"post",{ model.asCompileString.postln });
		ActionButton(layout,"dump",{ model.dump });
		ActionButton(layout,":=> var x",{
			var string = "x";
			//GetStringDialog("assign to interpreter variable a-z","x",{ arg ok,string;
				//if(ok,{
					thisProcess.interpreter.performList((string ++ "_").asSymbol,model);
					//this.newErrorWindow;
					//"".postln;
					//string.postln;
				//})
			//})
		});
		ActionButton(layout,"open class file",{
			model.class.openCodeFile;
		});
	}
}

ClassGui : CXObjectInspector { // ClassGui

	writeName {}

	guiBody { arg layout;
	
		var iNames,supers,scale;
		layout.hr;
	
		// you are here
		InspectorLink.big(model,layout.startRow,maxx:200);
		CXLabel(layout,":",y: 30);
		supers = model.superclasses;
		if(supers.notNil,{
			scale = supers.size;
			supers.do({ arg sup,i;
				ClassNameLabel(sup,layout,100,30);//.labelColor_(Color.grey(1.0 - (i / scale))
			})
		});


		layout.startRow;
		//CXLabel(layout,PathName(model.filenameSymbol.asString).asRelativePath);
		ActionButton(layout, "Source",{
			model.openCodeFile;
		}).font_(Font("Monaco",9.0));	
		ActionButton(layout,"Help",{ 
			var path;
			// wtf ?  this works in the interpreter but not here
			//path = model.name.asString.findHelpFile;
			//path.insp;
			model.openHelpFile;
			//{ model.asClass.insp.openHelpFile; nil; }.defer;
		});

		// subclasses
		// needs a scroll view
		this.displaySubclassesOf(model,layout,0,50);
		
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
		if(model.classVarNames.size > 0,{
			CXLabel(layout.startRow,"classvars:",maxx:160).bold;
			model.classVarNames.size.do({ arg cvi;
				var iv;
				VariableNameLabel(model.classVarNames.at(cvi),layout.startRow);
				iv=model.classVars.at(cvi);
				//ClassNameLabel(iv.class,layout);
				InspectorLink(iv,layout);
			});
		});
				
		//instance vars
		if(model.instVarNames.size > 0,{
			CXLabel(layout.startRow,"vars:",maxx:160).bold;
			model.instVarNames.size.do({ arg ivi;
				if(ivi % 8 ==0,{ layout.startRow });
				VariableNameLabel(model.instVarNames.at(ivi),layout,maxx:100);
				// iprototype
			});
		});		

		// meta_class methods
		if(model.class.methods.size > 0,{
			model.class.methods.size.do({arg cmi;
				MethodLabel.classMethod(model.class.methods.at(cmi),layout.startRow,maxx:160);
			});
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
		ActionButton(layout,"find method...",{
			GetStringDialog("find method...","",{ arg ok,string;
				var class,method;
				string = string.asSymbol;
				class = model;
				while({ class != Meta_Object },{
					method = class.findMethod(string);
					if(method.notNil,{ 
						method.gui;
						class = Meta_Object
					 },{
					 	class = class.superclass;
					 });
				});
			})
		});		
				
		this.dependantsGui(layout);
	}
	
	
	displayMethodsOf { arg class,f,withoutClass = false;
		f = f.asFlowView;
		if(class.methods.notNil,{
			class.methods.do({ arg meth;
				if(withoutClass,{
					MethodLabel.withoutClass(meth,f.startRow,maxx:160)
				},{
					MethodLabel(meth,f.startRow,maxx:200);
				});
				class.superclasses.do({ arg superclass;
					var supermethod;
					supermethod = superclass.findMethod(meth.name);
					if(supermethod.notNil,{
						MethodLabel(supermethod,f,maxx:200)
					});
				})
			})
		});
		if(class.superclass.notNil,{ // not Object
			ActionButton(f.startRow,"Methods of" + class.superclass.name + "...",{
				Sheet({ arg f;
					this.displayMethodsOf(class.superclass,f)
				},"Methods of" + class.superclass.name);
			})
		});
	}
	
	
	displaySubclassesOf { arg class,layout,shown,limit,indent = 50;
		var subclasses;
		if(class.subclasses.notNil,{
			subclasses = class.subclasses.as(Array)
				.sort({arg a,b; a.name.asString < b.name.asString});
			shown = shown + subclasses.size;
			subclasses.do({ arg c,i;
					layout.startRow;
					CXLabel(layout,"",maxx: indent).background = Color.clear;
					ClassNameLabel(c,layout,200);
					indent = indent + 50;
					//layout.indent(1);
					
					if(c.subclasses.size + shown < limit,{
						this.displaySubclassesOf(c,layout,shown,limit,indent);
					},{
						if(c.subclasses.size > 0,{
							CXLabel(layout,c.subclasses.size.asString 
								+ " subclasses",maxx:80);
						});
					});
					indent = indent - 50;
					//layout.indent(-1);
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
		
		ActionButton(layout,"Source",{ 
			model.openCodeFile;
		});
		ActionButton(layout,"Help",{ 
			model.openHelpFile;
		});
	
		
		layout.hr;
		ClassNameLabel(model.ownerClass,layout.startRow,maxx:200);
		MethodLabel(model,layout,maxx:200);

		// argNames
		prototypeFrame = model.prototypeFrame ?? {[]};
		
		CXLabel(layout.startRow,"args and defaults:",maxx:160).bold;
		(model.numArgs - 1).do({ arg i;// first arg is 'this'
			// TODO ellipsis ... 
			if(model.argNames.notNil,{
				ArgNameLabel(model.argNames.at(i + 1),layout.startRow);
				CXLabel(layout,prototypeFrame.at(i ));
			});
		});
		
		if(model.primitiveName.notNil,{
			CXLabel(layout.startRow,"primitiveName:",maxx:160).bold;
			CXLabel(layout,model.primitiveName);
		});
		

		layout.hr;
		
		// link to superclass implementations
		CXLabel(layout,"Superclass implementations");
		layout.startRow;
		supers = model.ownerClass.superclasses;
		if(supers.notNil,{
			supers.reverse.do({ arg class;
				var supermethod;
				supermethod = class.findMethod(model.name);
				ClassNameLabel(class,layout.startRow,maxx:300);
				if(supermethod.notNil,{
					//	started = true;
					MethodLabel(supermethod,layout,maxx:300);
				});

			})
		});	
		
		
		// show subclass implementations
		// other class implementations of this message
		layout.hr;
		
		

	}

}

//
//DictionaryGui : ObjectGui {
//
//	guiBody { arg layout;
//		model.keysValuesDo({ arg k,v,i;
//			CXLabel(layout.startRow,k,maxx: 100);
//			Tile(v,layout,200);
//		})
//	}
//
//}
//


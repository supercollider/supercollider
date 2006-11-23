
CXObjectInspector : ObjectGui {

	writeName { arg layout;
		ClassNameLabel.newBig(model.class,layout);
		SCDragSource(layout,Rect(0,0,500,30))
			.object_(model)
			.background_(Color.white)
			.align_(\center)
			.font_(Font("Helvetica-Bold",18));
	}

	guiBody { arg layout;
		var vert,list,listItems,actions;
		listItems = List.new;
		actions = List.new;
		
		this.instVarsGui(listItems,actions);
		
		// slotAt
		if(model.isArray,{
			//vert = model.slotSize < 26;
			min(model.slotSize,2048).do({arg i;
				var iv;
				//if(vert or: {i % 3 == 0},{ layout.startRow; });
				
				listItems.add( ("@" ++ i).as(Array).extend(25,$ ).as(String) ++"=  " + model.slotAt(i).asString );
				actions.add({ model.slotAt(i).insp(model,"@"++i) });
				
				//CXLabel(layout,"@" ++ i,minWidth: 40);
				//iv=model.slotAt(i);
				//ClassNameLabel(iv.class,layout);
				// drag in / out here
				//InspectorLink(iv,layout,minWidth:200);
			});
			if(model.slotSize > 300,{ 
				CXLabel(layout,"... slotSize is" ++ model.slotSize.asString,minWidth:160).bold;
			});
		});
		
		list = SCListView(layout,500@600);
		list.font = Font("Courier",10);
		list.background = Color(0.65,0.75,0.65,0.15);
		list.items = listItems.array.add("");
		list.action = { 
			actions[list.value].value };
		list.value = list.items.size - 1;
		this.dependantsGui(layout);
		this.actionsGui(layout);
	}
	
	instVarsGui { arg listItems,actions;
		var iNames;
		//identify which classes they come from, color code by fading
		iNames=model.class.instVarNames;
		if(iNames.notNil,{
			iNames.do({arg v,i;
				var iv;
				//layout.startRow;
				//VariableNameLabel(v,layout);
				iv=model.instVarAt(i);
				listItems.add( v.asString.as(Array).extend(25,$ ).as(String) ++ "=  " + iv);
				actions.add({ iv.insp(model,v) });
								
				/*ActionButton(layout,"code->",{
					GetStringDialog("enter code to compile and insert to " 
							+ v.asString,"",
					{ arg ok,string;
						if(ok,{
							model.instVarPut(i,  string.interpret)
						})
					})
				});*/
				//iv=model.instVarAt(i);
				//ClassNameLabel(iv.class,layout);
				//InspectorLink(iv,layout,400);
			});
		});
	}
	dependantsGui { arg layout;
		layout.hr;
		// dependants
		CXLabel(layout.startRow,"dependants:",minWidth:160).bold;
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
			thisProcess.interpreter.performList('x_',model);
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
		InspectorLink.big(model,layout.startRow,minWidth:200);
		CXLabel(layout,":",y: 30);
		supers = model.superclasses;
		if(supers.notNil,{
			scale = supers.size;
			supers.do({ arg sup,i;
				ClassNameLabel(sup,layout,100,30);
			})
		});

		layout.startRow;
		ActionButton(layout, "Source",{
			model.openCodeFile;
		}).font_(Font("Monaco",9.0));	
		ActionButton(layout,"Help",{ 
			var path;
			model.openHelpFile;
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
			CXLabel(layout.startRow,"classvars:",minWidth:160).bold;
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
			CXLabel(layout.startRow,"vars:",minWidth:160).bold;
			model.instVarNames.size.do({ arg ivi;
				if(ivi % 8 ==0,{ layout.startRow });
				VariableNameLabel(model.instVarNames.at(ivi),layout,minWidth:100);
				// iprototype
			});
		});		

		// meta_class methods
		if(model.class.methods.size > 0,{
			model.class.methods.size.do({arg cmi;
				MethodLabel.classMethod(model.class.methods.at(cmi),layout.startRow,minWidth:160);
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
					MethodLabel.withoutClass(meth,f.startRow,minWidth:160)
				},{
					MethodLabel(meth,f.startRow,minWidth:200);
				});
				class.superclasses.do({ arg superclass;
					var supermethod;
					supermethod = superclass.findMethod(meth.name);
					if(supermethod.notNil,{
						MethodLabel(supermethod,f,minWidth:200)
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
					CXLabel(layout,"",minWidth: indent).background = Color.clear;
					ClassNameLabel(c,layout,200);
					indent = indent + 50;
					//layout.indent(1);
					
					if(c.subclasses.size + shown < limit,{
						this.displaySubclassesOf(c,layout,shown,limit,indent);
					},{
						if(c.subclasses.size > 0,{
							CXLabel(layout,c.subclasses.size.asString 
								+ " subclasses",minWidth:80);
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
	
		
		// from Object down...		
		layout.startRow;
		supers = model.ownerClass.superclasses;
		if(supers.notNil,{
			supers.reverse.do({ arg class;
				var supermethod;
				supermethod = class.findMethod(model.name);
				ClassNameLabel(class,layout.startRow,minWidth:300);
				if(supermethod.notNil,{
					//	started = true;
					MethodLabel(supermethod,layout,minWidth:300);
				});

			})
		});	
		
		//layout.hr;
		ClassNameLabel(model.ownerClass,layout.startRow,minWidth:300);
		MethodLabel(model,layout,minWidth:300);

		// show subclass implementations
		// other class implementations of this message (command-y)
		CXLabel(layout.startRow,"References to this message:");
		Class.findAllReferences(model.name).do({ |r|
			//r.gui(layout.startRow);
			//r.debug;
			if(r.isKindOf(FunctionDef),{
				InspectorLink(r,layout.startRow);
			},{
				MethodLabel(r,layout.startRow);
			});
		});
		// would rather look at the source code for most things
	}
}

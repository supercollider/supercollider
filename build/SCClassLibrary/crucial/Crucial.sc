
Crucial {

	classvar menu;
		
	*initClass {
		
		// SETTINGS
		PathName.scroot =
				"/Volumes/Macintosh HD/Users/cruxxial/SuperCollider3/build/";

		Instr.instrDirectory = "Instr/"; // or ~/Documents/SC3/Instr/


		// hint: Lib menu, post... color
			//not working yet
			
		//tibook g4		
		PageLayout.screenWidth = 1100;
		PageLayout.screenHeight = 700;

		PageLayout.bgcolor = Color.new255(226,240,223);
		PageLayout.focuscolor = Color.new255(255,25,33);
		PageLayout.hrcolor = Color.new255(226,230,209);
		
		ActionButton.offcolor = Color.new255(112, 128, 144);


		CXLabel.bgcolor = Color.new255(250,250,240);
/*
		Tile.bgcolor =   Color.new255(231,250,178);
		InspectorLink.bgcolor =   Color.new255(151,145,255);
		MethodLabel.bgcolor =   Color.new255(160,206,147);
		ClassNameLabel.bgcolor =   Color.new255(133,193,230);
		ArgNameLabel.bgcolor =   Color.new255(255,221,97);
		VariableNameLabel.bgcolor =   Color.new255(255,221,97);
		SelectorLabel.bgcolor =   Color.new255(190,206,147);
		
		ToggleButton.offcolor =   Color.new255(189,151,70);
		ToggleButton.bgcolor =   Color.new255(104,244,50);
		BoolDisplay.offcolor =   Color.new255(189,151,70);
		BoolDisplay.bgcolor =   Color.new255(104,244,50);
		
		SelectButtonSet.defaultColor =   Color.new255(254,247,253);
		SelectButtonSet.defaultSelectedColor =   Color.new255(151,151,255);
*/
		// force to init first
		Class.initClassTree(Warp);
		Class.initClassTree(Spec);
		//Class.initClassTree(SC2compat);
		this.initSpecs;
		Class.initClassTree(Library);
		this.initLibraryItems;	
	}

	*initSpecs {
		Spec.specs.putAll(			
		  IdentityDictionary[
			'envasr'->EnvSpec.new,
			'env3'->EnvSpec.new,
			'audio'->AudioSpec.new,
			'lofreq'->ControlSpec.new(0.1, 100, 'exp', 0, 6),
			'env3sustain'->EnvSpec.new,
			'fenv'->EnvSpec.new,
			'rq'->ControlSpec.new(0.001, 2, 'exp', 0, 0.707),
			'rqenv'->EnvSpec.new,
			'envpercshort'->EnvSpec.new,
			'boostcut'->ControlSpec.new(-20, 20, 'lin', 0, 0),
			'bw'->ControlSpec.new(0, 1, 'lin', 0, 0.5),
			'octave'->ControlSpec.new(-6, 10, 'lin', 1, 2),
			'sampleStart'->ControlSpec.new(0, 4, 'lin', 0.25, 2),
			'amp'->ControlSpec.new(0.001, 1, 'exp', 0, 1),
			'degree'->ControlSpec.new(0, 11, 'lin', 1, 6),
			'ffreq4'->ControlSpec.new(0, 1, 'lin', 0, 0.5),
			'sdetune'->ControlSpec.new(0, 1, 'lin', 0, 0.5),
			'slewRise'->ControlSpec.new(10, 10000, 'lin', 0, 5005),
			'slewFall'->ControlSpec.new(10, 10000, 'lin', 0, 5005),
			'beats'->ControlSpec.new(0, 128, 'lin', 0, 4),
			'pchRatio'->ControlSpec.new(-4.0, 4.0, 'lin', 0, 2),
			'pitch'->ControlSpec.new(-4, 4, 'lin', 0, 0),

			'trig'->TrigSpec.new(0, 1, 'lin', 0, 0.0),
			'gate'->TrigSpec.new(0, 1, 'lin', 0, 0.0),
			//'gate'->ControlSpec.new(0, 1, 'lin', 0, 0.5),

			'legato'->StaticSpec.new(0.01, 4, 'lin', 0, 2.005),
			'release'->ControlSpec.new(0, 16, 'lin', 0, 8),
			'db'->ControlSpec.new(-130, 12, 'lin', 0, -59),
			'pan'->ControlSpec.new(-1, 1, 'lin', 0, 0),
			'bicoef'->ControlSpec.new(-1, 1, 'lin', 0, 0),
			'freq2'->ControlSpec.new(40, 5000, 'exp', 0, 447.214),
			'rq8'->ControlSpec.new(0.0001, 8, 'lin', 0, 4.00005),
			'safeffreq'->ControlSpec.new(200, 16000, 'exp', 0, 1788.85),
			'saferq'->ControlSpec.new(0.001, 0.7, 'lin', 0, 0.3505),
			'chaos'->ControlSpec.new(0, 5, 'lin', 0, 2.5),
			'freqOffset'->ControlSpec.new(-4000, 4000, 'lin', 0, 0),
			'bwr'->ControlSpec.new(0, 1, 'lin', 0, 0.5),//bandwithratio
			'delayDecay'->ControlSpec.new(-2, 2, 'lin', 0, 0),
			'binstretch'->ControlSpec.new(0, 1, 'lin', 0, 0.5),
			'binshift'->ControlSpec.new(0, 16, 'lin', 0, 8),
			'lfo'->ControlSpec.new(0, 3000, 'lin', 0, 1500),
			'width'->ControlSpec.new(0, 1, 'lin', 0, 0.5),
			'radians'->ControlSpec.new(0, 6.28319, 'lin', 0, 3.14159),
			'numChannels'->StaticSpec.new(1, 8, 'lin', 1, 5),
			'detune'->ControlSpec.new(-4000, 4000, 'lin', 0, 0),
			'freqScale'->ControlSpec.new(0.01, 10, 'lin', 0, 5.005),
			'qnty0'->StaticSpec.new(0, 20, 'lin', 1, 10),
			'ffreqMul'->ControlSpec.new(0.1, 16000, 'exp', 0, 40),
			'freq'->ControlSpec.new(20, 20000, 'exp', 0, 440),
			'phase'->ControlSpec.new(0, 6.28319, 'lin', 0, 3.14159),
			'ffreqAdd'->ControlSpec.new(0.1, 16000, 'exp', 0, 40),
			'ffreq'->ControlSpec.new(60, 20000, 'exp', 0, 1095.45),
			'velocity'->ControlSpec.new(0, 127, 'lin', 0, 63.5),
			'vibRate'->ControlSpec.new(0, 20, 'lin', 0, 10),
			'vibDepth'->ControlSpec.new(0, 1, 'lin', 0, 0.5),
			'tremRate'->ControlSpec.new(0, 20, 'lin', 0, 10),
			'tremDepth'->ControlSpec.new(0, 1, 'lin', 0, 0.5),
			'panRate'->ControlSpec.new(0, 20, 'lin', 0, 10),
			'panDepth'->ControlSpec.new(0, 1, 'lin', 0, 0.5),
			'thru'->ControlSpec.new(0, 1, 'lin', 0, 0.5),
			'off'->ControlSpec.new(0, 1, 'lin', 0, 0.5),
			'revTime'->ControlSpec.new(0, 16, 'lin', 0, 8),
			'taps'->StaticSpec.new(1, 10, 'lin', 1, 6),
			'combs'->StaticSpec.new(1, 10, 'lin', 1, 6),
			'unipolar'->ControlSpec.new(0, 1, 'lin', 0, 0.5),
			'microDelay'->ControlSpec.new(0.0001, 0.05, 'lin', 0, 0.02505),
			'microAttack'->ControlSpec.new(0.0001, 0.2, 'lin', 0, 0.10005),
			'microDecay'->ControlSpec.new(0.0001, 0.2, 'lin', 0, 0.10005),
			'combSelect'->StaticSpec.new(0, 5, 'lin', 1, 3),
			'medianLength'->StaticSpec.new(0, 15, 'lin', 1, 8),
			'uzi'->StaticSpec.new(1, 16, 'lin', 1, 9),
			'numharms'->ControlSpec.new(1, 100, 'lin', 0, 50.5),
			'sustain'->ControlSpec.new(0, 16, 'lin', 0, 8),
			'sensitivity'->ControlSpec.new(0, 12, 'lin', 0, 6),
			'gain'->ControlSpec.new(0.000001, 4, 'exp', 0, 2),
			'duration'->ControlSpec.new(0.125, 16, 'lin', 0.125, 8.125),
			'dur'->ControlSpec.new(0, 16, 'lin', 0, 8),
			'density'->ControlSpec.new(0, 30, 'lin', 0, 15),
			'qnty'->StaticSpec.new(1, 24, 'lin', 1, 13),
			'winSize'->StaticSpec.new(0.01, 4, 'lin', 0, 2.005),
			'pchDispersion'->ControlSpec.new(0, 4, 'lin', 0, 2),
			'timeDispersion'->ControlSpec.new(0, 3, 'lin', 0, 1.5),
			'maxBeats'->StaticSpec.new(0.125, 8, 'lin', 0, 4.0625),
			'drive'->ControlSpec.new(0, 10, 'lin', 0, 5),
			'feedback'->ControlSpec.new(0, 1, 'lin', 0, 0.5),
			'overlap'->ControlSpec.new(0, 12, 'lin', 0, 6),
			'maxDelay'->StaticSpec.new(0.005, 1, 'lin', 0, 0.5025),
			'speed'->ControlSpec.new(0.001, 8, 'lin', 0, 4.0005),
			'root'->ControlSpec.new(0, 11, 'lin', 1, 6),
			'bidecay'->ControlSpec.new(-10, 10, 'lin', 0, 0),
			'midinote'->ControlSpec.new(0, 127, 'lin', 1, 64),
			'env'->EnvSpec.new,
			'timeScale'->ControlSpec.new(0.1, 10, 'lin', 0, 5.05),
			'pmindex'->ControlSpec.new(0, 20, 'lin', 0, 10),
			'rate'->ControlSpec.new(0.125, 8, 'exp', 0, 1),
			'beat'->ControlSpec.new(0.001, 1, 'lin', 0.125, 0.5),
			'decay'->ControlSpec.new(0, 16, 'lin', 0, 8),
			'mix'->ControlSpec.new(0, 1, 'lin', 0, 0.5),
			'post'->ControlSpec.new(0, 1, 'lin', 0, 0.5),
			'lag'->ControlSpec.new(0, 1, 'lin', 0, 0.5),
			'stretch'->ControlSpec.new(0.0125, 4, 'lin', 0, 2.00625),
			'tempoFactor'->ControlSpec.new(0.125, 4, 'lin', 0.125, 2.125),
			'coef'->ControlSpec.new(0, 1, 'lin', 0, 0.5),
			'bipolar'->ControlSpec.new(-1, 1, 'lin', 0, 0),
			'widefreq'->ControlSpec.new(0.1, 20000, 'exp', 0, 440),
			'attack'->ControlSpec.new(0, 16, 'lin', 0, 8),
			'chaosParam'->ControlSpec.new(1, 30, 'lin', 0, 15.5),
			'dt'->ControlSpec.new(0.01, 0.04, 'lin', 0, 0.025),
			'iseed'->StaticSpec.new(1, 1000000, 'lin', 1, 500001),
			'midi'->ControlSpec.new(0, 127, 'lin', 0, 64),
			'imod'->StaticSpec.new(1, 1000000, 'lin', 1, 500001),
			'fdrive'->ControlSpec.new(0, 20, 'lin', 0, 10),
			'audio1'->AudioSpec.new,
			'pre'->ControlSpec.new(0, 1, 'lin', 0, 0.5),
			'audio2'->AudioSpec.new,
			'midivelocity'->ControlSpec.new(1, 127, 'lin', 0, 64),
			'envlinen'->EnvSpec.new,
			'envtriangle'->EnvSpec.new,
			'delay'->ControlSpec.new(0.005, 1, 'lin', 0, 0.5025),
			'envsine'->EnvSpec.new,
			'envadsr'->EnvSpec.new,
			'envperc'->EnvSpec.new,
			\in->AudioSpec.new,
			\k->ControlSpec(-6.0,6.0)
		  ]
		);
	}	
	
	*initLibraryItems {

		Library.put(\menuItems,'introspection','ClassBrowser',{
			Object.gui
		});
		
		// tools
		Library.put(\menuItems,\load,'browse for objects...',{
			GetFileDialog({ arg ok,loadPath;
				if(ok,{
					loadPath.loadPath.topGui;
				});
			})
		});
		
		Library.put(\menuItems,\test,'audioIn Test',{
			{AudioIn.ar([1,2])}.play
		});
		Library.put(\menuItems,\test,'simple audio test',{
			{SinOsc.ar(500,0,0.1)}.play
		});
		Library.put(\menuItems,\test,'run Class unit tests',{
			TestCase.runAll;
		});
//		
//		Library.put(\menuItems,\test,'midiCC test',{var w;
//			w = PageLayout.new("jlcooper",200,150);
//			8.do({arg i;
//			SliderView.new( w.window, w.layRight(10,100), "SliderView", 0.409449, 0, 1, 0, 'lin')
//				.prSetMIDIMapping(176, 1, 50 + i)
//			})
//		});
//		
//		Library.put(\menuItems,\post,'post keydown...',{
//			KeyDown.tester
//		});
//		
//		
//		Library.put(\menuItems,\post,'post ascii,keyCode,modifier...',{
//			Sheet({ arg l;
//				ActionButton.new(l,"while focused on this button, press keys and modifiers to post a code template").focus
//					.keyDownAction_({arg a,c,m;
//									[a,c,m].postln;
//								});
//			})
//		});
//
//		Library.put(\menuItems,\post,'post keyCode',{
//			Sheet({ arg l;
//				ActionButton.new(l,"while focused on this button, press keys and modifiers to post a code template").focus
//					.keyDownAction_({arg a,c,m;
//									c.post;
//									",".post;
//								});
//			})
//		});

	
		
//		Library.put(\menuItems,\post,'post color...',{
//			GetColorDialog("Color",Color.white,{ arg ok,color;
//				if(ok,{ color.post;})
//			});
//		});
		
		Library.put(\menuItems,\post,'post path...',{
			GetFileDialog({ arg ok,loadPath;
				if(ok,{
					PathName(loadPath).asRelativePath.asCompileString.post;
				});
			})
		});
		Library.put(\menuItems,\post,'post array of paths...',{
			CocoaDialog.getPaths({ arg paths;
				Post <<<  paths.collect({ arg p; 
							PathName(p).asRelativePath })   
					<< Char.nl;
			})
		});
		
//		Library.put(\menuItems,\introspection,'gcInfo',{
//			this.gcInfo
//		});
//		Library.put(\menuItems,\introspection,'Interpreter-inspect',{
//			thisProcess.interpreter.inspect;
//		});
//		Library.put(\menuItems,\introspection,'Interpreter-clearAll',{
//			thisProcess.interpreter.clearAll;
//		});

//		Library.put(\menuItems,\introspection,\findClassByPartialName,{
//			GetStringDialog("Classname or partial string","",{
//				arg ok,string;
//				var matches,f,classes;
//				matches = IdentitySet.new;
//				if(ok,{
//					classes = Class.allClasses.reject({ arg cl; cl.class === Class });
//					classes.do({ arg cl;
//						if(cl.name.asString.containsi(string),{
//							matches = matches.add(cl);
//						});
//					});
//
//					Sheet({ arg f;
//						matches.do({ arg cl;
//							ActionButton(f.startRow,cl.name,{
//								cl.gui;
//							},200);
//						});
//					});
//				})
//			});
//		});
//		Library.put(\menuItems,\tools,\methodfinder,{
//			GetStringDialog("methodname or partial string","",{
//				arg ok,string;
//				var matches,f,classes;
//				matches = IdentitySet.new;
//				if(ok,{
//					classes = Class.allClasses.reject({ arg cl; cl.class === Class });
//					classes.do({ arg cl;
//						if(cl.name.asString.containsi(string),{
//							matches = matches.add(cl);
//						});
//					});
//
//					f = PageLayout.new;
//					matches.do({ arg cl;
//						ActionButton(f.startRow,cl.name,{
//							cl.gui;
//							//cl.newErrorWindow.name.postln;
//						},200);
//					});
//					f.resizeWindowToFit;
//				})
//			});
//		});
		

//		
//		Library.put(\menuItems,\introspection,\findReferencesToClass,{
//			GetStringDialog("Class name:","",{
//				arg ok,string;
//				var fn;
//				fn = { arg class;
//						var found;
//						found = Array(8);
//						Class.allClasses.do({ arg eachClass;
//							if(eachClass.explicitClassReferences.any({ arg c; c===class }),{
//								found = found.add(eachClass);
//							})
//						});
//						found
//					};
//				if(ok,{
//					this.newErrorWindow;
//					fn.value(string.asSymbol.asClass).do({ arg c;
//						c.postln;
//					});
//				})
//			});
//		});

		//needs a tree browser
		Library.put(\menuItems,\introspection,'non-nil class variables',{
			Sheet({ arg f;
				Object.allSubclasses.do({ arg c,i;
					if(c.classVarNames.size > 0,{
						ClassNameLabel(c,f.hr);
						c.classVars.do({ arg cv,cvi;
							var iv;
							if(cv.notNil,{
								VariableNameLabel(c.classVarNames.at(cvi),
									f.startRow);
								//ClassNameLabel(cv.class,f);
								InspectorLink(cv,f);
							})
						});
					})		
				})
			},"Classvars")
		});
	
		// browse all currently loaded Instr
//		Library.put(\menuItems,\sounds,\orcs,{ arg onSelect;
//			// if there's nothing in Instr it does the top
//			MLIDbrowser(\Instr,onSelect).gui
//		});
		
		Library.put(\menuItems,\post,'post Instr address',{
			MLIDbrowser(\Instr,{ arg instr;
				instr.name.asCompileString.post; 
			}).gui;
		});
		
		Library.put(\menuItems,\sounds,'make new Patch',{
			MLIDbrowser(\Instr,{ arg instr;
				Patch(instr.name).topGui
			}).gui;
		});

		Library.put(\menuItems,\tools,\Specs,{
			Sheet({ arg f;
				Spec.specs.keys.asList.asArray.sort.do({arg spn;
					var sp;
					sp=Spec.specs.at(spn);
					f.startRow;
					CXLabel(f,spn,maxx:100);
					sp.gui(f);
				})
			},"Specs")
		});
		//should be a toggle button
		/*Library.put(\menuItems,\tools,'Server dump',{
			Server.local.dumpOSC(1);
		});
		Library.put(\menuItems,\tools,'Server stop dump',{
			Server.local.dumpOSC(0);
		});*/
		
//		Library.put(\menuItems,\settings,'set max PageLayout size ...',{
//			// useful when using video projectors with small screen size
//			Sheet({ arg f;
//					ActionButton(f,"set and post screen size",{
//					var b;
//					b = f.window.bounds;
//					PageLayout.screenWidth = b.width;
//					PageLayout.screenHeight = b.height;
//				
//					("PageLayout.screenWidth = " ++ b.width.asCompileString ++ ";").postln;
//					("PageLayout.screenHeight = " ++ b.height.asCompileString ++ ";").postln;
//				});
//			})
//		})
	
	}

	*libraryMenu { arg resize=true;
		// this is just your Library(\menuItems) functions put up on a menu
		
		var dic,c;
		if(menu.notNil,{ menu.close });
		
		menu=PageLayout("LibraryFunctions",220,800,metal: true);
		dic=Library.at(\menuItems);
		if(dic.notNil,{
			dic.keys.asList.sort.do({arg k,ki;
				var subdic;
				subdic=Library.at(\menuItems,k);
				CXLabel(menu,k.asString,maxx:200)
					.backColor_(Color.new255(112, 128, 144))
					.view.stringColor_(Color.white);
				subdic.keys.asList.sort.do({arg k;
					ActionButton(menu.startRow,k,{
						subdic.at(k).value
					},200).backColor_(Color.new255(245, 245, 245))
				});
			});			
		});
			
		// if you have a wacom tablet  you could uncomment these
		//ToggleButton(menu.startRow,"TabletTracking",{ 
		//	TZM.tracking = true; },{ TZM.tracking = false }, 
		//TZM.tracking ,140);
			
		ToggleButton(menu.startRow,"Server dumpOSC",{
			Server.local.dumpOSC(1)
		},{
			Server.local.dumpOSC(0)
		},Server.local.dumpMode != 0 );
		
		TempoGui.setTempoKeys;
		Tempo.gui(menu.startRow);

		if(resize,{
			menu.resizeWindowToFit;
		});
	}
	
}

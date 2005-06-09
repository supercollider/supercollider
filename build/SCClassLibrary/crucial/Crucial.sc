
Crucial {

	classvar menu, debugNodeWatcher;
		
	*initClass {
		// force to init first
		Class.initClassTree(Warp);
		Class.initClassTree(Spec);
		this.initSpecs;
		Class.initClassTree(Library);
		this.initLibraryItems;	
	}

	*initSpecs {
		// this will be moved somewhere more polite
		Spec.specs.putAll(			
		  IdentityDictionary[
			'audio'->AudioSpec.new,
			//'lofreq'->ControlSpec.new(0.1, 100, 'exp', 0, 6),
			//'rq'->ControlSpec.new(0.001, 2, 'exp', 0, 0.707),
			//'boostcut'->ControlSpec.new(-20, 20, 'lin', 0, 0),
			'bw'->ControlSpec.new(0, 1, 'lin', 0, 0.5),
			'octave'->ControlSpec.new(-6, 10, 'lin', 1, 2),
			'sampleStart'->ControlSpec.new(0, 4, 'lin', 0.25, 2),
			'degree'->ControlSpec.new(0, 11, 'lin', 1, 6),
			'ffreq4'->ControlSpec.new(0, 1, 'lin', 0, 0.5),
			'sdetune'->ControlSpec.new(0, 1, 'lin', 0, 0.5),
			'slewRise'->ControlSpec.new(10, 10000, 'lin', 0, 5005),
			'slewFall'->ControlSpec.new(10, 10000, 'lin', 0, 5005),
			'pchRatio'->ControlSpec.new(-4.0, 4.0, 'lin', 0, 2),
			'pitch'->ControlSpec.new(-4, 4, 'lin', 0, 0),

			'trig'->TrigSpec.new(0, 1, 'lin', 0, 0.0),
			'gate'->TrigSpec.new(0, 1, 'lin', 0, 0.0),

			'legato'->StaticSpec.new(0.01, 4, 'lin', 0, 0.9),
			'release'->ControlSpec.new(0, 16, 'lin', 0, 0.5),
			'bicoef'->ControlSpec.new(-1, 1, 'lin', 0, 0.2),
			'freq2'->ControlSpec.new(40, 5000, 'exp', 0, 447.214),
			'rq8'->ControlSpec.new(0.0001, 8, 'lin', 0, 1.0),
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
			'numChannels'->StaticSpec.new(1, 8, 'lin', 1, 2),
			'freqScale'->ControlSpec.new(0.01, 10, 'lin', 0, 1.0),
			'qnty0'->StaticIntegerSpec.new(0, 20, 'lin', 1, 10),
			'ffreqMul'->ControlSpec.new(0.1, 16000, 'exp', 0, 2),
			'freq'->ControlSpec.new(20, 20000, 'exp', 0, 440),
			//'phase'->ControlSpec.new(0, 6.28319, 'lin', 0, 3.14159),
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
			'off'->NoLagControlSpec.new(0, 1, 'lin', 0, 0.5),
			'revTime'->ControlSpec.new(0, 16, 'lin', 0, 8),
			'taps'->StaticIntegerSpec.new(1, 10, 'lin', 1, 6),
			'combs'->StaticIntegerSpec.new(1, 10, 'lin', 1, 6),
			'unipolar'->ControlSpec.new(0, 1, 'lin', 0, 0.5),
			'microDelay'->ControlSpec.new(0.0001, 0.05, 'lin', 0, 0.02505),
			'microAttack'->ControlSpec.new(0.0001, 0.2, 'lin', 0, 0.10005),
			'microDecay'->ControlSpec.new(0.0001, 0.2, 'lin', 0, 0.10005),
			'combSelect'->StaticIntegerSpec.new(0, 5, 'lin', 1, 3),
			'medianLength'->StaticIntegerSpec.new(0, 15, 'lin', 1, 8),
			'uzi'->StaticIntegerSpec.new(1, 16, 'lin', 1, 9),
			'numharms'->ControlSpec.new(1, 100, 'lin', 0, 50.5),
			'sustain'->ControlSpec.new(0, 16, 'lin', 0, 8),
			'sensitivity'->ControlSpec.new(0, 12, 'lin', 0, 6),
			'gain'->ControlSpec.new(0.000001, 4, 'exp', 0, 2),
			'dur'->ControlSpec.new(0, 16, 'lin', 0, 1),
			'density'->ControlSpec.new(0, 30, 'lin', 0, 1.0),
			'qnty'->StaticIntegerSpec.new(1, 24, 'lin', 1, 4),
			'winSize'->StaticSpec.new(0.01, 4, 'lin', 0, 0.2),
			'pchDispersion'->ControlSpec.new(0, 4, 'lin', 0, 0.05),
			'timeDispersion'->ControlSpec.new(0, 3, 'lin', 0, 0.05),
			'maxBeats'->StaticSpec.new(0.125, 8, 'lin', 0, 4),
			'drive'->ControlSpec.new(0, 10, 'lin', 0, 1.5),
			'feedback'->ControlSpec.new(0, 1, 'lin', 0, 0.7),
			'overlap'->ControlSpec.new(0, 12, 'lin', 0, 6),
			'maxDelay'->StaticSpec.new(0.005, 1, 'lin', 0, 0.5),
			'speed'->ControlSpec.new(0.001, 8, 'lin', 0, 4.0),
			'root'->ControlSpec.new(0, 11, 'lin', 1, 0),
			'bidecay'->ControlSpec.new(-10, 10, 'lin', 0, 0.05),
			'midinote'->ControlSpec.new(0, 127, 'lin', 1, 64),
			'note'->ControlSpec.new(0, 11, 'lin', 1, 0),
			'timeScale'->ControlSpec.new(0.1, 10, 'lin', 0, 1.0),
			'pmindex'->ControlSpec.new(0, 20, 'lin', 0, 10),
			//'rate'->ControlSpec.new(0.125, 8, 'exp', 0, 1),
			'beat'->ControlSpec.new(0.001, 1, 'lin', 0.125, 0.5),
			'decay'->ControlSpec.new(0, 16, 'lin', 0, 1.5),
			'mix'->ControlSpec.new(0, 1, 'lin', 0, 0.5),
			'post'->ControlSpec.new(0, 1, 'lin', 0, 1.0),
			'lag'->ControlSpec.new(0, 1, 'lin', 0, 0.1),
			'stretch'->ControlSpec.new(0.0125, 4, 'lin', 0, 1.0),
			'tempoFactor'->ControlSpec.new(0.125, 4, 'lin', 0.125, 1.0),
			'coef'->ControlSpec.new(0, 1, 'lin', 0, 0.5),
			'bipolar'->ControlSpec.new(-1, 1, 'lin', 0, 0),
			'widefreq'->ControlSpec.new(0.1, 20000, 'exp', 0, 440),
			'attack'->ControlSpec.new(0, 16, 'lin', 0, 0.1),
			'chaosParam'->ControlSpec.new(1, 30, 'lin', 0, 2.0),
			'dt'->ControlSpec.new(0.01, 0.04, 'lin', 0, 0.025),
			'iseed'->StaticSpec.new(1, 1000000, 'lin', 1, 500001),
			'midi'->ControlSpec.new(0, 127, 'lin', 0, 64),
			'imod'->StaticSpec.new(1, 1000000, 'lin', 1, 500001),
			'fdrive'->ControlSpec.new(0, 20, 'lin', 0, 10),
			'audio1'->AudioSpec.new,
			'pre'->ControlSpec.new(0, 1, 'lin', 0, 0.5),
			'audio2'->AudioSpec.new,
			'midivelocity'->ControlSpec.new(1, 127, 'lin', 0, 64),
			//'delay'->ControlSpec.new(0.005, 1, 'lin', 0, 0.5025),
			\in->AudioSpec.new,
			\k->ControlSpec(-6.0,6.0),
			\stepsPerOctave->ControlSpec(0.5,128.0),
			\mul -> ControlSpec(0,1),
			\add -> ControlSpec(0,1)
		  ]
		);
	}	
	*menu {
	
		// this is everything in Library(\menuItems) functions put up on a menu
		
		var a,rec,pause;
		if(menu.notNil,{ menu.close });
		
		menu = MultiPageLayout.new("-Library-");
		
		Server.default.gui(menu);
		menu.startRow;

		a = ActionButton(menu.startRow,"     Library Menu Items...     ",{
			MLIDbrowser(\menuItems)
				.onSelect_({ arg f; f.value })
		},minWidth: 250)
		.background_(Color.new255(112, 128, 144))
		.labelColor_(Color.white);

		ToggleButton(menu.startRow,"Server dumpOSC",{
			Server.default.stopAliveThread;
			Server.default.dumpOSC(1)
		},{
			Server.default.startAliveThread;
			Server.default.dumpOSC(0)
		},Server.default.dumpMode != 0 ,minWidth: 250);
		
		
		if(debugNodeWatcher.isNil,{
			debugNodeWatcher = AnnotatedDebugNodeWatcher(Server.default);
		});
		ToggleButton(menu.startRow,"DebugNodeWatcher",{
			debugNodeWatcher.start;
		},{
			debugNodeWatcher.stop;
		},debugNodeWatcher.isWatching,minWidth: 250);

		ActionButton(menu.startRow,"Query All Nodes",{
			Library.at(\menuItems,\tools,'Server Node Report').value;		},minWidth: 250);

		ActionButton(menu.startRow,"kill all",{
			Server.killAll;
		},minWidth: 250);
			
			
	
		TempoGui.setTempoKeys;
		Tempo.default.gui(menu.startRow);

		menu.resizeToFit.front;
		
		a.focus;
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
			{SinOsc.ar([500,550],0,0.1)}.play
		});
		
		Library.put(\menuItems,\test,'run Class unit tests',{
			TestCase.runAll;
		});
			
		Library.put(\menuItems,\post,'post [char,unicode,modifier,keycode]...',{
			Sheet({ arg l;
				ActionButton.new(l,"press keys and modifiers").focus
					.keyDownAction_({arg v,c,m,u,k;
									[c,m,u,k].postln;
								});
			})
		});

		/*Library.put(\menuItems,\post,'post color...',{
			GetColorDialog("Color",Color.white,{ arg ok,color;
				if(ok,{ color.post;})
			});
		});*/
		
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
		Library.put(\menuItems,\post,'post Unicode...',{
			UnicodeResponder.tester;
		});
				
		Library.put(\menuItems,\introspection,'find class...',{
			GetStringDialog("Classname or partial string","",{
				arg ok,string;
				var matches,f,classes;
				matches = IdentitySet.new;
				if(ok,{
					classes = Class.allClasses.reject({ arg cl; cl.class === Class });
					classes.do({ arg cl;
						if(cl.name.asString.containsi(string),{
							matches = matches.add(cl);
						});
					});

					Sheet({ arg f;
						matches.do({ arg cl;
							ClassNameLabel(cl,f.startRow,200);
							ActionButton(f,"source",{
								cl.openCodeFile;
							},60);
							ActionButton(f,"help",{
								cl.openHelpFile;
							},60);
						});
						if(matches.isEmpty,{
							CXLabel(f,"No matches found");
						});
					},"matches" + string);
				})
			});
		});
		/*
		Library.put(\menuItems,\introspection,\methodfinder,{
			GetStringDialog("methodname or partial string","",{
				arg ok,string;
				var matches,f,classes;
				matches = IdentitySet.new;
				if(ok,{
					Class.allClasses.do({ arg cl;
						if(cl.isMetaClass.not and: {cl.name.asString.containsi(string)},{
							matches = matches.add(cl);
						});
					});

					f = PageLayout.new;
					matches.do({ arg cl;
						CXLabel(f.startRow,cl.name,minWidth:200);
						ActionButton(f.startRow,"source",{
							cl.openCodeFile;
						},60);
						ActionButton(f,"help",{
							if(cl.hasHelpFile,{
								cl.openHelpFile;
							},{
								cl.ownerClass.openHelpFile;
							});
						},60);
						ActionButton(f,"browser",{
							cl.gui;
						},60);
					});
					f.resizeToFit;
				})
			});
		});
		*/

		Library.put(\menuItems,\introspection,\findReferencesToClass,{
			GetStringDialog("Class name:","",{ arg ok,string;
				Class.findAllReferences(string.asSymbol).do({ arg ref;
					ref.postln;
				});
			});
		});

		Library.put(\menuItems,\introspection,'find class-not-found',{

				Class.allClasses.do({ arg class;
					(class.methods).do({ arg method;
						var selectors;
						selectors = method. selectors;
						selectors.do({ arg lit;
							if(lit.isKindOf(Symbol) and: { lit.isClassName },{
								if(lit.asClass.isNil,{
									lit.post;
									(30 - lit.asString.size).do({ " ".post; });
									(class.name.asString ++ "-" ++ 
										method.name.asString).postln;
								})
							})
						})
					})
				});
		});
	
		Library.put(\menuItems,\introspection,'find method-not-found',{
			/*
				search for potential method not found errors
			
				look through all methods for method selectors that
					are not defined for any class
				
				this finds many false cases,
				but i found a good 7 or 8 typos in mine and others code
			
				// could reject any spec names
				// any instr names
				// sound file types
			*/
		
			var allMethodNames;
			allMethodNames = IdentityDictionary.new;
		
			Class.allClasses.do({ arg class;
				(class.methods).do({ arg method;
					allMethodNames.put(method.name,true);
				})
			});
		
			Class.allClasses.do({ arg class;
				(class.methods).do({ arg method;
					var selectors;
					selectors = method.selectors;
					if(selectors.notNil,{
						selectors.do({ arg lit;
							if(lit.isKindOf(Symbol) and: { lit.isClassName.not },{
								if(allMethodNames.at(lit).isNil,{
									lit.post;
									(30 - lit.asString.size).do({ " ".post; });
									(class.name.asString ++ "-" ++ method.name.asString).postln;
								})
							})
						})
					});
				})
			});

		});
	
		//needs a tree browser
		Library.put(\menuItems,\introspection,'non-nil class variables',{
			Sheet({ arg f;
				Object.allSubclasses.do({ arg c,i;
					if(c.classVarNames.size > 0,{
						ClassNameLabel(c,f.hr);
						c.classVars.do({ arg cv,cvi;
							var iv;
							if(cv.notNil,{
								VariableNameLabel(c.classVarNames.at(cvi),f.startRow);
								//ClassNameLabel(cv.class,f);
								InspectorLink(cv,f);
							})
						});
					})		
				})
			},"Classvars")
		});
	
		Library.put(\menuItems,\introspection,'find duplicate variable name errors',{
			var f;
			f = { arg class,found;
				class.subclasses.do({ arg sub;
					var full,myFound;
					myFound = if(found.isNil,{ List.new },{ found.copy });

					if(sub.instVarNames.as(IdentitySet).size != sub.instVarNames.size,{
						full = sub.instVarNames.reject(IsIn(myFound)).as(Array);
						full.removeAll( sub.instVarNames.as(IdentitySet) );
						myFound.addAll(full);
						if(full.notEmpty,{
							Post << sub << " has defined a variable with the same name as a superclass." << Char.nl;
						
							Post << full << Char.nl << Char.nl;
						});
					});
					f.value(sub,myFound);
				});
			};
			
			f.value(Object);		
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
					CXLabel(f,spn,minWidth:100);
					sp.gui(f);
				})
			},"Specs")
		});
		Library.put(\menuItems,\tools,\audioBuses,{
			Sheet({ arg f;
				CXLabel(f.startRow,"address");
				CXLabel(f,"numChannels");
				Server.default.audioBusAllocator.blocks.do({ arg block;
					CXLabel(f.startRow,block.address);
					CXLabel(f,block.size);
				});
			},"allocated AudioBuses on default")
		});
		
		Library.put(\menuItems,\tools,'Annotated Buses Report',{
			var a;
			"Audio Buses:".postln;
			a = Library.at(AbstractPlayer, \busAnnotations, Server.default,\audio);
			if(a.notNil,{
				a.keysValuesDo({ |k,v|
					[k,v].postln
				})
			});
			"Control Buses:".postln;
			a = Library.at(AbstractPlayer, \busAnnotations, Server.default,\control);
			if(a.notNil,{
				a.keysValuesDo({ |k,v|
					[k,v].postln
				})
			});
		});

		/*		
		Library.put(\menuItems,\tools,'Annotated Node Report',{
			var a;
			a = Library.at(AbstractPlayer, \busAnnotations, Server.default,\audio);
			if(a.notNil,{
				a.keysValuesDo({ |k,v|
					[k,v].postln
				})
			});
			a = Library.at(AbstractPlayer, \busAnnotations, Server.default,\control);
			if(a.notNil,{
				a.keysValuesDo({ |k,v|
					[k,v].postln
				})
			});
		});*/
		
		Library.put(\menuItems,\tools,'Server Node Report',{
			var probe,probing,resp,nodes,server,report,indent = 0,order=0;
			var nodeWatcher;
			
			server = Server.default;
			nodeWatcher = NodeWatcher.newFrom(server);
			
			nodes = IdentityDictionary.new;
			probing = List.new;
			
			probe = { arg nodeID;
				probing.add(nodeID);
				server.sendMsg("/n_query",nodeID);
			};
			
			report = { arg nodeID=0;
				var child,node;
				indent.do({ " ".post });
				nodes.at(nodeID).use({
					~order = order;
					if(~isGroup,{ 
						node = nodeWatcher.nodes.at(nodeID);
						if(node.notNil,{
							Post << node << " " << AbstractPlayer.getAnnotation(node)  << Char.nl;
						},{
							("Group(" ++ nodeID ++ ")").postln;
						});
	
						child = ~head;
						indent = indent + 8;
						while({
							child > 0
						},{
							order = order + 1;
							report.value(child);
							child = nodes.at(child).at(\next);
						});
						indent = indent - 8;
					},{
						node = nodeWatcher.nodes.at(nodeID);
						if(node.notNil,{ // get defName if available
							Post << node << " " << AbstractPlayer.getAnnotation(node) << Char.nl;
						},{
							("Synth(" + nodeID + ")").postln;
						});
					});
				});
			};
				
			resp = OSCresponder(server.addr,'/n_info',{ arg a,b,c;
						var cmd,nodeID,parent,prev,next,isGroup,head,tail;
						# cmd,nodeID,parent,prev,next,isGroup,head,tail = c;
						
						//[cmd,nodeID,parent,prev,next,isGroup,head,tail].debug;
						
						nodes.put(nodeID,
							Environment.make({
								~nodeID = nodeID;
								~parent = parent;
								~prev = prev;
								~next = next;
								~isGroup = isGroup == 1;
								~head = head;
								~tail = tail;
							})
						);
						
						if(next > 0,{
							probe.value(next);
						});
						if(isGroup==1,{
							if(head > 0,{
								probe.value(head);
							});
						});
						probing.remove(nodeID);
						if(probing.size == 0,{
							resp.remove;
							report.value;
						});
					}).add;
					
			probe.value(0);

		});
	}
	
}

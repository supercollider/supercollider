

TestCase {
	
	var class,associations;
	var routine;
	
	classvar <>testdir="TestingAndToDo/TestsCases/", <failures;
	
	*new { arg class ... associations;
		^super.newCopyArgs(class,associations)
	}
	
	run { arg resetFailures=true,report=true;
		var askMonkey;
		if(resetFailures,{ this.class.resetFailures });
		askMonkey = { arg message;
			ModalDialog({ arg f;
					CXLabel(f,message);
				},	
				{ true.yield },
				"Human, can you confirm ?",
				{ false.yield }
			);
			routine.stop;
		};
		routine = Routine.new({
			associations.do({ arg ass;
				if(ass.value.value(this,askMonkey).not,{
					this.failed(ass.key);
				})
			});
			if(report,{ this.class.report });
			nil
		}).play(AppClock);
	}
	
	*runAll { arg resetFailures=true,report=true;
		if(resetFailures,{ this.resetFailures });
		Class.allClasses.do({ arg sc;
			if(sc.isMetaClass.not,{
				sc.test(false,false)
			})
		});
		if(report,{ this.report; });
	}
	
	failed { arg name;
		failures = failures.add([class, name]);
	}
	*resetFailures { failures = []; }
	*report {
		Sheet({ arg f;
			if(failures.size > 0,{
				CXLabel(f,"There were failures.");
				failures.do({ arg cn;
					ActionButton(f.startRow,"Class file" + cn.at(0),{
						// could see if the method name matches
						cn.at(0).openCodeFile;
					});
					ActionButton(f, "TestCase file" + cn.at(1),{
						this.pathForClassTest(cn.at(0)).openTextFile;
					});
				})
			},{
				CXLabel(f,"No failures.");
			})
		},"TestCase-report")
	}
	*pathForClassTest { arg class;
		^testdir ++ class.name.asString ++ ".test.rtf"
	}
	
}

/*
TestResult {
	var <class,<name,<message;
	*new { arg class,name="",message="";
		^super.newCopyArgs(class,name,message)
	}
	gui { arg f;
		ActionButton(f.startRow,clas,{
			// could see if the method name matches
			class.openCodeFile;
		});
		ActionButton(f, name,{
			TestCase.pathForClassTest(name).openTextFile;
		});
	}		
}
*/


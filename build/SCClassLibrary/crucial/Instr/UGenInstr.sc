
UGenInstr { // make a virtual Instr by reading the class def

	var <ugenClass,<rate,<specs;
	
	*new { arg ugenClass,rate=\ar;
		^super.new.init(ugenClass,rate)
	}
	init { arg uc,r;
	
		ugenClass = uc.asClass;
		rate = r;
	
		//specs
		specs = this.argNames.collect({ arg ag,i;
			ag.asSpec ?? {
				("UGenInstr-init  spec not found for:" + ag).warn;
			}
		});
	}

	value { arg args;
		^ugenClass.performList(rate,args)	
	}
	valueArray { arg args;
		^ugenClass.performList(rate,args)
	}

	ar { arg ... args; ^this.value(args) }	
	kr { arg ... args; ^this.value(args) }

	funcDef { ^ugenClass.class.findMethod(rate) }
	maxArgs { ^this.argsSize }
	argsSize { ^this.funcDef.argNames.size - 1 }
	argNames { 
		var an;
		an = this.funcDef.argNames;
		^if(an.isNil,{
			[]
		},{
			an.copyRange(1,an.size - 1)
		})
	}	

	//defaultArgs
	defArgs { 
		var nn;
		nn=this.funcDef.prototypeFrame;
		^if(nn.notNil,{nn.copyRange(1,nn.size-1)},{[]});
	}

	initAt { arg i;  ^(this.defArgAt(i) ?? {this.specs.at(i).default}) }
	argNameAt { arg i; 
		var nn;
		nn=this.funcDef.argNames;
		^if(nn.notNil,{nn.at(i)},{nil});
	}
	defArgAt {
		 arg i; 
		var nn;
		nn=this.funcDef.prototypeFrame;
		^if(nn.notNil,{nn.at(i + 1)},{nil});
	}
		
//	guiClass { ^UGenInstrGui }
	asString { ^"UGenInstr." ++ ugenClass.name.asString }
	asInstr { ^this }
	name { ^ugenClass }
	storeParamsOn { arg stream;
		stream << "(" <<< ugenClass << "," <<< rate << ")";
	}
}


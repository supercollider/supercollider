
+ AbstractPlayer {

	state {}
	state_ {}

}	

+ Patch {

	state { 
		var s;
		s = Environment.new;
		args.do({ arg ag,i;
			s.put(instr.argNameAt(i), ag.tryPerform(\state)) // or nil is fine
		});
		^s
	}
	state_ { arg state;
		args.do({ arg ag,i;
			ag.tryPerform(\state_, state.at( instr.argNameAt(i)) )
		});
	}

}

+ Editor {

	state { ^value}
	state_ { arg val; this.value_(val); }

}

+ HasSubject {

	state { ^subject.tryPerform(\state) }
	state_ { arg state; subject.tryPerform(\state_,state) }

}


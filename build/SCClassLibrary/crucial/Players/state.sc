
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

//+ EfxMatrix {
//	/** needs to change to socket/voice **/
//	state { 
//		var path;
//		if(nowPlaying.notNil,{ path = this.pathOf(nowPlaying) });
//		^Environment[
//			\mixes->mixControllers, 
//			\efxpath->path ,
//			\subject->subject.state
//		] 
//	}
//
//	state_ { arg state;
//		if(state.notNil,{ // this is wrong question
//			state.use({
//				if(filtersChangeOnState,{
//					mixControllers = ~mixes.copy;
//					if(~efxpath.notNil,{
//						this.wakeEffectByPath(~efxpath);
//					}); // else all off ?
//				});
//				subject.state_(~subject).changed;
//			})
//		})
//	}
// 
//	setVoiceState { arg vi,state;
//		state.use({
//			// do not recall myself 
//			//mixControllers.at(vi).state =  ~mixes.at(pli);
//			//this.wakeEffectByPath(~efxpath);
//			subject.setVoiceState(vi,~subject).changed;
//		})		
//	}
//	loadForState { arg state;
//		state.use({
//			if(~efxpath.notNil,{
//				this.addPlayer(~efxpath);
//			});
//			subject.loadForState(~subject);
//		})
//	}
//
//}


// + PolyPlayerPool {
	// TODO env not yet supported
//	state { ^Environment[ \paths->this.alignedPlayingPaths] }
//	state_ { arg state; this.playAlignedPlayingPaths(state.at(\paths)) }
//	setVoiceState { arg vi,state;
//		this.select(vi,this.indexOfPlayerAtPath(state.at(\paths).at(vi)))
//	}
//	nameForStateSlot { arg vi;
//		var player;
//		player = this.players.at(selecteds.at(vi));
//		^if(player.isNil,{
//			""
//		},{
//			player.name;
//		})
//	}
// }

/*
AmpsPPP
	state {
		^Environment.make({
			~paths=this.alignedPlayingPaths;
			if(stateSavesAmps,{
				~amps=amps.copy;
			});
			if(~env.notNil,{
				env = ~env.deepCopy;
				sockets.do({ arg ms;
					ms.env = env;
				})
			})
		})	 
	}
	state_ { arg state; // PROBLEM
		state.use({
			if(stateRecallsAmps and: {~amps.notNil},{
				amps = ~amps.copy; // but not setting the plug
				// UNLESS its the same path but the amp is different !!!!
				// then .line it
			});			
			this.playAlignedPlayingPaths(~paths); // this will kill the plug
			~env = env.deepCopy;
		})
	}
	setVoiceState { arg vi,state;
		var path;
		state.use({
			path = ~paths.at(vi);
			if(path.notNil,{
				if(stateSavesAmps and: {~amps.notNil},{
					amps.put(vi, ~amps.at(vi) ? 1.0);
				});
				this.select(vi,this.indexOfPlayerAtPath(path));
			});
		})
	}
	loadForState { arg state;
		state.at(\paths).do({ arg path;
			if(path.notNil,{
				this.addPlayer(path);
			})
		})
	}
*/

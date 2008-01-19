
// these support different ways of specifying the timing of a scheduled event
// see the atTime help file

+ Nil { // now
	schedBundle { arg bundle,server,timeOfRequest;
		bundle.send(server,nil); //0.0
	}
}


+ Float { // relative seconds
	schedBundle {  arg bundle,server,timeOfRequest;
		var now;
		if(timeOfRequest.notNil,{
			now = Main.elapsedTime;
			if((now - timeOfRequest) <= this,{
				bundle.send(server,this);
			},{ // better late than never...
				bundle.send(server,0.0);
			});
		},{
			bundle.send(server,this);
		});
	}
}
/**
  * 1 : bar
  * 2 : half-note
  * 4 : quarter note
  * 8 : 8th note
  * 16 : 16th note
  */
+ Integer { // at the next N beat
	schedBundle { arg bundle,server,timeOfRequest;
		var now,nowRound,latencyBeats,deltaTillSend;
		
		latencyBeats = Tempo.secs2beats(server.latency);
		now = TempoClock.default.elapsedBeats;
		nowRound = now.roundUp(4 / this);
		
		deltaTillSend = (nowRound - now - latencyBeats);
		if(deltaTillSend < 0.05,{
			nowRound = nowRound + (4/this);
			deltaTillSend = (nowRound - now - latencyBeats);
		});
			
		TempoClock.default.sched( deltaTillSend, {
			var lateness,latency;
			// this executes at Server.latency before the event is due.
			// calculate actual latency to the requested time
			latency = Tempo.beats2secs(nowRound -  TempoClock.default.elapsedBeats);

			/*SystemClock.sched(latency,{
				var b;
				b = TempoClock.default.elapsedBeats;
				[b,nowRound - b].debug("actual sched");
				nil;
			});*/
			
			bundle.send(server, latency);
			nil
		});		
	}
}

+ Date {
	// Date raw seconds has to be set correctly !
	// *new won't do this for you
	schedBundle { arg bundle,server,timeOfRequest;
		var delta;
		delta = rawSeconds - this.class.localtime.rawSeconds;
		if(delta >= 0.0,{
			bundle.send(server,delta);
		})
	}
}


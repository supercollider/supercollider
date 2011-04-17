// these support different ways of specifying the timing of a scheduled event
// see the atTime help file

+ Nil { // now
	schedBundle { arg bundle,server,timeOfRequest;
		bundle.send(server,nil,nil); //0.0
	}
}


+ Float { // relative seconds
	schedBundle {  arg bundle,server,timeOfRequest;
		// this also need doPrepare
		bundle.send(server,this,timeOfRequest);
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

		bundle.doPrepare(server,{
			var now,nowRound,latencyBeats,deltaTillSend;
			latencyBeats = Tempo.secs2beats(server.latency ? 0.05);
			now = TempoClock.default.elapsedBeats;
			nowRound = now.roundUp(4 / this);

			deltaTillSend = (nowRound - now - latencyBeats);
			if(deltaTillSend < 0.05,{
				nowRound = nowRound + (4/this);
				deltaTillSend = (nowRound - now - latencyBeats);
			});

			TempoClock.default.sched( deltaTillSend, {
				var lateness,delta;
				// this executes at Server.latency before the event is due.
				// calculate actual latency to the requested time
				delta = Tempo.beats2secs(nowRound -  TempoClock.default.elapsedBeats);

				/*SystemClock.sched(delta,{
					var b;
					b = TempoClock.default.elapsedBeats;
					[b,nowRound - b].debug("actual sched");
					nil;
				});*/

				bundle.prSend(server, delta,Main.elapsedTime);
				nil
			});
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
			// should we prepare this a few seconds ahead of time ?
			bundle.send(server,delta,timeOfRequest);
		});
	}
}


// these support different ways of specifying the timing of a scheduled event
// see the atTime help file

+ Nil { // now
	schedCXBundle { arg bundle,server,timeOfRequest;
		bundle.send(server,nil); //0.0
	}
}


+ Float { // relative seconds
	schedCXBundle {  arg bundle,server,timeOfRequest;
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
	schedCXBundle { arg bundle,server,timeOfRequest;
		var now,nowRound,tdelta,latencyBeats;
		latencyBeats = Tempo.secs2beats(server.latency);
		now = TempoClock.default.elapsedBeats;
		nowRound = (now + latencyBeats).roundUp((this / 4).reciprocal);
		tdelta = Tempo.beats2secs(nowRound - now);

		// for server.latency seconds you are exposed to error if you are
		// changing the tempo during that time
		TempoClock.default.sched( nowRound - now, {
			/*SystemClock.sched(server.latency,{
				var b;
				b = TempoClock.default.elapsedBeats.debug("the beat");
				(b % 4).debug;
				nil;
			});*/
			bundle.send(server, server.latency);
			nil
		});		
	}
}

+ Date {
	// Date raw seconds has to be set correctly !
	// *new won't do this for you
	schedCXBundle { arg bundle,server,timeOfRequest;
		var delta;
		delta = rawSeconds - this.class.localtime.rawSeconds;
		if(delta >= 0.0,{
			bundle.send(server,delta);
		})
	}
}


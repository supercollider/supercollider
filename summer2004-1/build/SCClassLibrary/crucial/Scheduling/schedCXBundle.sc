
+ Nil {
	schedCXBundle { arg bundle,server,timeOfRequest;
		bundle.send(server,0.0);
	}
}


+ Float {
	schedCXBundle {  arg bundle,server,timeOfRequest;
		var now;
		if(timeOfRequest.notNil,{
			now = Main.elapsedTime;
			if((now - timeOfRequest) <= this,{
				bundle.send(server,this);
			},{ //better late than never...
				bundle.send(server,0.0);
			});
		},{
			bundle.send(server,this);
		});
	}
}

+ Integer {
	schedCXBundle { arg bundle,server,timeOfRequest;
		var now,nowRound,tdelta;
		now = TempoClock.default.elapsedBeats;
		nowRound = now.roundUp(this.reciprocal);
		tdelta = Tempo.beats2secs(nowRound - now);
		//[now,nowRound,tdelta,nowRound - Tempo.secs2beats(server.latency)].debug;
		if(tdelta > server.latency,{ // enough time to sched in sclang ?
			TempoClock.default.schedAbs(nowRound - Tempo.secs2beats(server.latency),{
				bundle.send(server, server.latency);
				nil
			});
		},{ // send now
			bundle.send(server, tdelta.max(0.0));
		});
	}
}

+ Date { // raw seconds has to be set correctly !
	schedCXBundle { arg bundle,server,timeOfRequest;
		var delta;
		delta = rawSeconds - this.class.localtime.rawSeconds;
		if(delta >= 0.0,{
			bundle.send(server,delta);
		})
	}
}


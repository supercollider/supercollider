
/*

TO BE DEPRECIATED
ancient history

		OSCresponder.queue([
			[ { server.serverRunning.not },{ server.boot }, '/done'],
			[ { server.audioRunning.not },{ server.start }, '/on'],
			[ { loaded.at(plid).isNil },{ server.load(plid) }, '/done' ],
			[ true ,{ this.sendPlayMsg } ]
		]);

		// each step is
		[
			{ server.serverRunning.not }, // whether to do this step at all
			{ server.boot }, 			// the step itself
			'/done',					// notification to wait for from server
										// if nil, does not wait and moves to next step
										// irrelevant for the last step
			someServer				// server (defaults to Server.default)
		],

	this could be used to start up multiple servers.
	probably a way to do this with Routine so that
		each step would .yield
	todo:  add *clear	to cancel all future steps
*/
OSCResponderQueue {
	var <>queue;
	classvar all;
	*initClass {
		all = List.new;
	}
	*new { arg queue;
		^super.new.queue_(queue)
	}
	value { arg queue;
		var step,doItIf,it,waitForMsg,fromServer;
		if(queue.notEmpty,{
			step = queue.removeAt(0);
			# doItIf, it, waitForMsg,fromServer = step;
			if(doItIf.value,{
				if(queue.notEmpty,{
				 	if(waitForMsg.notNil,{
						this.class.waitFor({this.do(queue)},waitForMsg,fromServer);
					});
				},{
					all.remove(queue);
				});
				it.value;
			})
		})
	}
	*waitFor { arg action,cmd='/done',server;
		var resp;
		[action,cmd,server].postln;
		resp = OSCresponder((server ?? {Server.default}).addr, cmd,
				{ arg time, resp, msg;
					("osc responder recieved:" ++ cmd.asString).postln;
					action.value(time, resp, msg);
					//[time, resp, msg].postln;
					OSCresponder.remove(resp);
				 });
		OSCresponder.add(resp);
	}

}
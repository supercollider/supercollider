// subclass this for building anything that needs
// to modify a synth that is moved into this group 
// or created in this group (see Ensemble)

AbstractEnsemble : Group  {
	
	
	*new { arg target,addAction=\addToHead;
			^super.new(target,addAction);
	}
	
	//here any updating messages can be implemented
	finishBundle { arg msgList, reciever;
		^this.subclassResponsibility(thisMethod)
	}
	
	sendUpdatedMsg { arg msgList, reciever, latency;
		
		this.finishBundle(msgList, reciever);
		//server.sendMsgList(msgList);
		server.listSendBundle(latency, msgList); 
	}
	/*
	*newMsg { arg msgList, target, addAction=\addToHead;
		var res;
		res = super.newMsg(msgList, target,addAction);
		res.updateMsg(msgList);
		^res
	}
	*/
	
	
	moveNodeToHead { arg aNode;
		var msg;
		msg = List.new;
		msg.add(["/g_head", nodeID, aNode.nodeID]);
		this.sendUpdatedMsg(msg, aNode);
	
	}
	
	moveNodeToTail { arg aNode;
		var msg;
		msg = List.new;
		msg.add(["/g_tail", nodeID, aNode.nodeID]);
		
		this.sendUpdatedMsg(msg, aNode);
	
	}
	
	moveNodeBefore { arg movedNode, aNode;
		var msg;
		msg = List.new;
		msg.add(["/n_before", movedNode.nodeID, aNode.nodeID]);
		
		this.sendUpdatedMsg(msg, movedNode);

	}
	
	moveNodeAfter { arg movedNode, aNode;
		var msg;
		msg = List.new;
		msg.add(["/n_after", movedNode.nodeID, aNode.nodeID]);
	
		this.sendUpdatedMsg(msg, movedNode);
	}	
	
	/*
	sendGroupToServer { arg arggroup, addActionNum,targetID;
		var msg;
		
		if(server.serverRunning, {
			
			
			arggroup.register(server);
			
			msg = List.new;
			msg.add(["/g_new", arggroup.nodeID, addActionNum, targetID]);
		
			this.sendUpdatedMsg(msg, arggroup);
		
			//arggroup.isPlaying = true;
			//arggroup.isRunning = true;
		}, { "Server not running".inform });
	
	}
	
	sendSynthToServer { arg argsynth, addActionNum,targetID,args;
		var msg;
		if(server.serverRunning, {
			
			argsynth.register(server);
			
			msg = List.new;
			msg.add(["/s_new", argsynth.defName,argsynth.nodeID, addActionNum, targetID]++ args);
		
			this.sendUpdatedMsg(msg, argsynth);
			//argsynth.isPlaying = true;
			//argsynth.isRunning = true;
		}, { "Server not running".inform });
	}
	*/
}



ModelGroup : AbstractEnsemble {
	var <>updateFunc;
	
	*new { arg updateFunc, target,addAction=\addToHead;
			^super.new(target,addAction).updateFunc_(updateFunc);
	}
	
	//see bundled messages for how to use these
	finishBundle { arg msgList, reciever;
		updateFunc.value(msgList, reciever);
	}

}


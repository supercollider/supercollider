// subclass this for building anything that needs
// to modify a synth that is moved into this group 
// or created in this group (see Ensemble)

AbstractEnsemble : Group  {
	
	
	*new { arg target,addAction=\addToHead;
			^super.new(target,addAction);
	}
	
	//here any updating messages can be implemented
	finishBundle { arg bundle, reciever;
		^this.subclassResponsibility(thisMethod)
	}
	
	sendUpdatedMsg { arg bundle, reciever, latency;
		
		this.finishBundle(bundle, reciever);
		//server.sendMsgList(msgList);
		server.listSendBundle(latency, bundle.postln); 
	}
	
	
	moveNodeToHead { arg aNode;
		var msg;
		aNode.group = this;
		msg = List.new;
		msg.add(["/g_head", nodeID, aNode.nodeID]);
		this.sendUpdatedMsg(msg, aNode);
	
	}
	
	moveNodeToTail { arg aNode;
		var msg;
		aNode.group = this;
		msg = List.new;
		msg.add(["/g_tail", nodeID, aNode.nodeID]);
		
		this.sendUpdatedMsg(msg, aNode);
	
	}
	
	moveNodeBefore { arg movedNode, aNode;
		var msg;
		movedNode.group = this;
		msg = List.new;
		msg.add(["/n_before", movedNode.nodeID, aNode.nodeID]);
		
		this.sendUpdatedMsg(msg, movedNode);

	}
	
	moveNodeAfter { arg movedNode, aNode;
		var msg;
		movedNode.group = this;
		msg = List.new;
		msg.add(["/n_after", movedNode.nodeID, aNode.nodeID]);
	
		this.sendUpdatedMsg(msg, movedNode);
	}	
	
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


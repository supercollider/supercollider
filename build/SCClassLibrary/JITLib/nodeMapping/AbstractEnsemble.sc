// subclass this for building anything that needs
// to modify a synth that is moved into this group 
// or created in this group (see Ensemble)

AbstractEnsemble : Group  {
	
	
	*new { arg  target,addAction=\addToHead;
			^super.new(target,addAction);
	}
	
	//here any updating commands can be implemented
	updateCommand { 
		^this.subclassResponsibility(thisMethod)
	}
	
	sendUpdatedCommand { arg cmd, reciever;
		
		this.updateCommand(cmd, reciever);
		server.sendCmdList(cmd);
	}
	
	moveNodeToHead { arg aNode;
		var cmd;
		cmd = List.new;
		cmd.add(["/g_head", nodeID, aNode.nodeID]);
		this.sendUpdatedCommand(cmd, aNode);
	
	}
	
	moveNodeToTail { arg aNode;
		var cmd;
		cmd = List.new;
		cmd.add(["/g_tail", nodeID, aNode.nodeID]);
		
		this.sendUpdatedCommand(cmd, aNode);
	
	}
	
	moveNodeBefore { arg movedNode, aNode;
		var cmd;
		cmd = List.new;
		cmd.add(["/n_before", movedNode.nodeID, aNode.nodeID]);
		
		this.sendUpdatedCommand(cmd, movedNode);

	}
	
	moveNodeAfter { arg movedNode, aNode;
		var cmd;
		cmd = List.new;
		cmd.add(["/n_after", movedNode.nodeID, aNode.nodeID]);
	
		this.sendUpdatedCommand(cmd, movedNode);
	}	
	
	sendGroupToServer { arg arggroup, addActionNum,targetID;
		var cmd;
		
		if(server.serverRunning, {
			
			
			arggroup.register(server);
			
			cmd = List.new;
			cmd.add(["/g_new", arggroup.nodeID, addActionNum, targetID]);
		
			this.sendUpdatedCommand(cmd, arggroup);
		
			//arggroup.isPlaying = true;
			//arggroup.isRunning = true;
		}, { "Server not running".inform });
	
	}
	
	sendSynthToServer { arg argsynth, addActionNum,targetID,args;
		var cmd;
		if(server.serverRunning, {
			
			argsynth.register(server);
			
			cmd = List.new;
			cmd.add(["/s_new", argsynth.defName,argsynth.nodeID, addActionNum, targetID]++ args);
		
			this.sendUpdatedCommand(cmd, argsynth);
			//argsynth.isPlaying = true;
			//argsynth.isRunning = true;
		}, { "Server not running".inform });
	}
}



ModelGroup : AbstractEnsemble {
	var <>updateFunc;
	
	*new { arg updateFunc, target,addAction=\addToHead;
			^super.new(target,addAction).updateFunc_(updateFunc);
	}
	
	//see bundled commands for how to use these
	updateCommand { arg cmdList, reciever;
		updateFunc.value(cmdList, reciever);
	}

}


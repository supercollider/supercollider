
Node {
	
	var <>nodeID, <>server, <>group;
	var <>isPlaying = false, <>isRunning = false;
	classvar addActions;

	*initClass {
		addActions = (addToHead: 0, addToTail: 1, addBefore: 2, addAfter: 3, addReplace: 4)
	}

	
	*basicNew { arg server, nodeID;
		server = server ? Server.default;
		^super.newCopyArgs(nodeID ?? { server.nextNodeID }, server)
	}

	free { arg sendFlag=true;
		if(sendFlag, {
			server.sendMsg(11, nodeID);  //"/n_free"
		});
		group = nil;
		isPlaying = false;
		isRunning = false;
	}
	freeMsg { ^[11, nodeID] }

	run { arg flag=true;
		server.sendMsg(12, nodeID,flag.binaryValue); //"/n_run"
	}
	
	runMsg { arg flag=true;
		^[12, nodeID,flag.binaryValue]; //"/n_run"
	}
	
	map { arg controlName, index ... args;
		server.sendMsg(14, nodeID, controlName, index, *args); //"/n_map"
	}
	mapMsg { arg controlName, index ... args;
		^[14, nodeID, controlName, index] ++ args; //"/n_map"
	}	
	mapn { arg controlName, index, numControls ... args;
		server.sendMsg(48, nodeID, controlName, numControls, *args); //"/n_mapn"
	}
	mapnMsg { arg controlName, index, numControls ... args;
		^[48, nodeID, controlName, numControls] ++ args; //"/n_mapn"
	}
	// map to Bus objects
	busMap { arg firstControl, aBus ... args;
		var values;
		values = List.new;
		args.pairsDo({ arg control, bus; values.addAll([control, bus.index, bus.numChannels])});
		server.sendMsg(48, nodeID, firstControl, aBus.index, aBus.numChannels, *values);
		//"/n_mapn"
	}
	busMapMsg { arg firstControl, aBus ... args;
		var values;
		values = List.new;
		args.pairsDo({ arg control, bus; values.addAll([control, bus.index, bus.numChannels])});
		^[48, nodeID, firstControl, aBus.index, aBus.numChannels] ++ values;
		//"/n_mapn"
	}
	set { arg controlName, value ... args;
		server.sendMsg(15, nodeID, controlName, value, *args); 	 //"/n_set"
	}
	
	setMsg { arg controlName, value ... args;
		^[15, nodeID, controlName, value] ++ args; 	 //"/n_set"
	}

	setn { arg controlName,  values ... args;
		server.sendMsg(*this.setnMsg(controlName, values, *args));
	}
	
	setnMsg { arg controlName,  values ... args;
		var nargs;
		args.do({ arg cnv;
			var two;
			nargs = nargs.add(cnv.at(0));
			nargs = nargs.add((two = cnv.at(1)).size);
			nargs = nargs.add(two);
		});
		^[16, nodeID,controlName, values.size] ++ values ++ nargs; 
			// "n_setn"
	}
	
	fill { arg controlName, numControls, value ... args;
		server.sendMsg(17, nodeID, controlName, numControls, value, *args); //"n_fill"
	}
	
	fillMsg { arg controlName, numControls, value ... args;
		^[17, nodeID, controlName, numControls, value] ++ args; //"n_fill"
	}

	release { arg releaseTime;
		server.sendMsg(*this.releaseMsg(releaseTime))
    	}
    	releaseMsg { arg releaseTime;
		//assumes a control called 'gate' in the synth
		if(releaseTime.isNil, {
			releaseTime = 0.0;
		},{
			releaseTime = -1.0 - releaseTime;
		});
		^[15, nodeID, \gate, releaseTime]
	}
	trace {
		server.sendMsg(10, nodeID);//"/n_trace"
	}
	query {
		OSCresponder(server.addr,'/n_info',{ arg a,b,c;
			var cmd,argnodeID,parent,prev,next,isGroup,head,tail;
			# cmd,argnodeID,parent,prev,next,isGroup,head,tail = c;
			// assuming its me ... if(nodeID == argnodeID)
			Post << if(isGroup == 1, "Group:" , "Synth:") << nodeID << Char.nl
				<< "parent   : " << parent << Char.nl
				<< "prev : " << prev << Char.nl
				<< "next  :" << next << Char.nl;
			if(isGroup==1,{
				Post << "head :" << head << Char.nl
				 << "tail :" << tail << Char.nl << Char.nl;
			});
		}).add.removeWhenDone;
		server.sendMsg(46, nodeID)  //"/n_query"
	}
	
	moveBefore { arg aNode;
		this.group = aNode.group; 
		server.sendMsg(18, nodeID, aNode.nodeID); //"/n_before"
	}
	moveAfter { arg aNode;
		this.group = aNode.group; 
		server.sendMsg(19, nodeID, aNode.nodeID); //"/n_after"
	}       
	moveToHead { arg aGroup;
		aGroup.asGroup.moveNodeToHead(this);
	}
	moveToTail { arg aGroup;
		aGroup.asGroup.moveNodeToTail(this);
	}

	// message creating methods
	
	moveBeforeMsg { arg aNode;
		^[18, nodeID, aNode.nodeID];//"/n_before"
	}
	moveAfterMsg { arg aNode;
		^[19, nodeID, aNode.nodeID]; //"/n_after"
	}       
	moveToHeadMsg { arg aGroup;
		^(aGroup ? group).moveNodeToHeadMsg(this);
	}
	moveToTailMsg { arg bundle, aGroup;
		^(aGroup ? group).moveNodeToTailMsg(this);
	}

	// private
	*prNew { arg server;
		^super.newCopyArgs(server.nextNodeID, server ? Server.default)
	}
	prSetNodeID { arg id;  nodeID = id ?? {server.nextNodeID}  }
	prSetPlaying { arg flag=true; isPlaying = flag; isRunning = flag }
	prMoveAfter {  arg afterThisOne;
		this.group = afterThisOne.group;
	}
	prMoveBefore {  arg beforeThisOne;
		this.group = beforeThisOne.group;
	}

	hash {  ^server.hash bitXor: nodeID.hash	}
	
	== { arg aNode;
		^(aNode.nodeID == nodeID) and: {aNode.server === server}
	}
	printOn { arg stream; stream << this.class.name << "(" << nodeID <<")" }

}


Group : Node {
	
	/** immediately sends **/
	*new { arg target, addAction=\addToHead;
		var group, server, addNum, inTarget;
		inTarget = target.asTarget;
		server = inTarget.server;
		group = this.basicNew(server);
		addNum = addActions[addAction];
		if((addNum < 2), { group.group = inTarget; }, { group.group = inTarget.group; });
		server.sendMsg(21, group.nodeID, addNum, inTarget.nodeID);
		^group
	}
	newMsg { arg target, addAction = \addToHead;
		var addNum, inTarget;
		inTarget = target.asTarget;
		addNum = addActions[addAction];
		(addNum < 2).if({ group = inTarget; }, { group = inTarget.group; });
		^[21, nodeID, addNum, inTarget.nodeID]		//"/g_new"
	}
	*after { arg aNode;    ^this.new(aNode, \addAfter) }
	*before {  arg aNode; 	^this.new(aNode, \addBefore) }
	*head { arg target; 	^this.new(target.asGroup, \addToHead) }
	*tail { arg target; 	^this.new(target.asGroup, \addToTail) }
	*replace { arg groupToReplace; ^this.new(groupToReplace, \addReplace) }
	
	moveNodeToHead { arg aNode;
		aNode.group = this;
		server.sendMsg(22, nodeID, aNode.nodeID); //"/g_head"
	}
	moveNodeToTail { arg aNode;
		aNode.group = this;
		server.sendMsg(23, nodeID, aNode.nodeID); //"/g_tail"
	}
	freeAll {
		// free my children, but this node is still playing
		server.sendMsg(24, nodeID); //"/g_freeAll"
	}
	deepFree {
		server.sendMsg(50, nodeID) //"/g_deepFree"
	}

	/*  return messages */
	addToHeadMsg { arg arggroup; 
		group = arggroup.asGroup;
		^[21, nodeID, 0, group.nodeID] 			//"/g_new"
	}
	addToTailMsg { arg arggroup;
		group = arggroup.asGroup;
		^[21, nodeID, 1, group.nodeID] 			//"/g_new"
	}
	addAfterMsg {  arg afterThisOne;
		group = afterThisOne.group;
		^[21, nodeID, 3, afterThisOne.nodeID] 	//"/g_new"
	}
	addBeforeMsg {  arg beforeThisOne;
		group = beforeThisOne.group;
		^[21, nodeID, 2, beforeThisOne.nodeID] 	//"/g_new"
	}
	addReplaceMsg { arg removeThisOne;
		group = removeThisOne.group;
		^[21, nodeID, 4, removeThisOne.nodeID] 	//"/g_new"
	}
	moveNodeToHeadMsg { arg aNode;
		^[22, nodeID, aNode.nodeID]; 			//"/g_head"
	}
	moveNodeToTailMsg { arg aNode;
		^[23, nodeID, aNode.nodeID];			//g_tail
	}
	
}

Synth : Node {

	var <>defName;
	
	/** immediately sends **/
	*new { arg defName, args, target, addAction=\addToHead;
		var synth, server, addNum, inTarget;
		inTarget = target.asTarget;
		server = inTarget.server;
		addNum = addActions[addAction];
		synth = this.basicNew(defName, server);
		if((addNum < 2), { synth.group = inTarget; }, { synth.group = inTarget.group; });
		server.sendMsg(9, defName, synth.nodeID, addNum, inTarget.nodeID, *args;);
		^synth
	}
		/** does not send	(used for bundling) **/
	*basicNew { arg defName, server, nodeID;
		^super.basicNew(server, nodeID).defName_(defName.asDefName)
	}
	
	newMsg { arg target, args, addAction = \addToHead;
		var addNum, inTarget;
		addNum = addActions[addAction];
		inTarget = target.asTarget;
		(addNum < 2).if({ group = inTarget; }, { group = inTarget.group; });
		^[9, defName, nodeID, addNum, inTarget.nodeID] ++ args; //"/s_new"
	}
	*after { arg aNode, defName, args;	
		^this.new(defName, args, aNode, \addAfter);
	}
	*before {  arg aNode, defName, args;
		^this.new(defName, args, aNode, \addBefore); 
	}
	*head { arg aGroup, defName, args; 
		^this.new(defName, args, aGroup, \addToHead); 
	}
	*tail { arg aGroup, defName, args; 
		^this.new(defName, args, aGroup, \addToTail);  
	}
	*replace { arg synthToReplace, defName, args;
		^this.new(defName, args, synthToReplace, \addReplace)
	}
	// nodeID -1 
	*grain { arg defName, args, target, addAction=\addToHead;
		var server;
		target = target.asTarget;
		server = target.server;
		server.sendMsg(9, defName.asDefName, -1, addActions[addAction], target.nodeID, *args);
			//"/s_new"
		^nil;
	}

	*newPaused { arg defName, args, target, addAction=\addToHead;
		var synth, server;
		target = target.asTarget;
		server = target.server;
		synth = this.basicNew(defName, server);
		server.sendBundle(nil, synth.newMsg(synth.defName, target, args, addAction), 
			[12, synth.nodeID, 0]); // "/n_run"
		^synth
	}
	addToHeadMsg { arg arggroup, args;
		group = arggroup.asGroup;
		^[9, defName, nodeID, 0, group.nodeID] ++ args	// "/s_new"
	}
	addToTailMsg { arg arggroup, args;
		group = arggroup.asGroup; 
		^[9, defName, nodeID, 1, group.nodeID] ++ args // "/s_new"
	}
	addAfterMsg {  arg afterThisOne, args;
		group = afterThisOne.group; 
		^[9, defName, nodeID, 3, afterThisOne.nodeID] ++ args // "/s_new"
	}
	addBeforeMsg {  arg beforeThisOne, args;
		group = beforeThisOne.group; 
		^[9, defName, nodeID, 2, beforeThisOne.nodeID] ++ args // "/s_new"
	}
	addReplaceMsg { arg removeThisOne, args;
		group = removeThisOne.group; 
		^[9, defName, nodeID, 4, removeThisOne.nodeID] ++ args // "/s_new"
	}
	
	get { arg index, action;
		OSCpathResponder(server.addr,['/n_set',nodeID,index],{ arg time, r, msg; 
			action.value(msg.at(3)); r.remove }).add;
		server.sendMsg(44, nodeID, index);	//"/s_get"
	}
	
	getn { arg index, count, action;

		OSCpathResponder(server.addr,['/n_setn',nodeID,index],{arg time, r, msg;
			action.value(msg.copyToEnd(4)); r.remove } ).add; 
		server.sendMsg(45, nodeID, index, count); //"/s_getn"
	}
		
	// private
	*prNew { arg defName, server;
		^this.basicNew(defName, server)
	}	
	printOn { arg stream; stream << this.class.name << "(" <<< defName << " : " << nodeID <<")" }

}

RootNode : Group {
	
	classvar <roots;
	
	*new { arg server;
		server = server ? Server.default;
		^(roots.at(server.name) ?? {
			^super.basicNew(server, 0).rninit
		})
	}
	rninit { 
		roots.put(server.name, this);
		isPlaying = isRunning = true;
		group = this; // self
	}
	*initClass {  roots = IdentityDictionary.new; }

	run { "run has no effect on RootNode".warn; }
	free { "free has no effect on RootNode".warn; }
	moveBefore { "moveBefore has no effect on RootNode".warn; }
	moveAfter { "moveAfter has no effect on RootNode".warn; }
	moveToHead { "moveToHead has no effect on RootNode".warn; }
	moveToTail{ "moveToTail has no effect on RootNode".warn; }

	*freeAll {
		roots.do({ arg rn; rn.freeAll })
	}
} 


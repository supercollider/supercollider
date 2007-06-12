
Node {
	
	var <>nodeID, <>server, <>group;
	var <>isPlaying = false, <>isRunning = false;
	
	*basicNew { arg server, nodeID;
		server = server ? Server.default;
		^super.newCopyArgs(nodeID ?? { server.nextNodeID }, server)
	}

	free { arg sendFlag=true;
		if(sendFlag, {
			server.sendBundle(nil, ["/n_free", nodeID]);  //"/n_free"
		});
		group = nil;
		isPlaying = false;
		isRunning = false;
	}
	freeMsg { ^[11, nodeID] }

	run { arg flag=true;
		server.sendBundle(nil, 
			[12, nodeID,flag.binaryValue]); 			 //"/n_run"
	}
	
	map { arg controlName, busIndex ... args;
		server.sendBundle(nil, 
			[14, nodeID, controlName, busIndex]++args); //"/n_map"
	}
	mapMsg {  arg controlName, busIndex ... args;
			^[14, nodeID, controlName, busIndex]++args; //"/n_map"
	}
	
	set { arg controlName, value ... args;
		server.sendBundle(nil, 
			["/n_set", nodeID, controlName, value] ++args); 	 //"/n_set"
	}
	
	setWithArray { arg args;
		server.sendBundle(nil, 
			[15, nodeID] ++ args); 					//"/n_set"
	}

	setn { arg controlName,  values ... args;
		var nargs;
		args.do({ arg cnv;
			var two;
			nargs = nargs.add(cnv.at(0));
			nargs = nargs.add((two = cnv.at(1)).size);
			nargs = nargs.add(two);
		});
		server.sendBundle(nil, 
			([16, nodeID,controlName, values.size] ++ values 
				++ nargs)); 						// "n_setn"
	}
	
	fill { arg controlName, numControls, value ... args;
		server.sendBundle(nil, 
			[17, nodeID, controlName, numControls, value]++args); //"n_setn"
	}

	release { arg releaseTime;
		server.sendBundle(nil, this.releaseMsg(releaseTime))
    	}
    	releaseMsg { arg releaseTime;
		//assumes a control called 'gate' in the synth
		if(releaseTime.isNil, {
			releaseTime = 0.0;
		},{
			releaseTime = -1.0 - releaseTime;
		});
		^["/n_set", nodeID, \gate, releaseTime]
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
		server.sendMsg("/n_query",nodeID) 
	}

	moveBefore { arg aNode;
		aNode.group.moveNodeBefore(this, aNode);
	}
	moveAfter { arg aNode;
		aNode.group.moveNodeAfter(this, aNode);
	}       
	moveToHead { arg aGroup;
		aGroup.asGroup.moveNodeToHead(this);
	}
	moveToTail { arg aGroup;
		aGroup.asGroup.moveNodeToTail(this);
	}
	// addAction conversion
	
	// return messages for bundling
	addToHead { arg arggroup, args; 
		group = arggroup.asGroup;
		 ^this.nodeToServerMsg(group.nodeID, 0, args);
	}
	addToTail { arg arggroup, args;
	 	group = arggroup.asGroup;
	 	^this.nodeToServerMsg(group.nodeID, 1, args);
	}
	addAfter {  arg afterThisOne, args;
	 	group = afterThisOne.group;
	 	^this.nodeToServerMsg(afterThisOne.nodeID, 3, args);
	}
	addBefore {  arg beforeThisOne, args;
	 	group = beforeThisOne.group;
	 	^this.nodeToServerMsg(beforeThisOne.nodeID, 2, args);
	}
	addReplace { arg removeThisOne, args;
	 	group = removeThisOne.group;
	 	^this.nodeToServerMsg(removeThisOne.nodeID, 4, args);
	}
			
	
	// message creating methods
	
	moveBeforeMsg { arg bundle, aNode;
		^bundle.add([18, nodeID, aNode.nodeID]);//"/n_before"
	}
	moveAfterMsg { arg bundle, aNode;
		^bundle.add([19, nodeID, aNode.nodeID]); //"/n_after"
	}       
	moveToHeadMsg { arg bundle, aGroup;
		^bundle.add((aGroup ? group).moveNodeToHeadMsg(this));
	}
	moveToTailMsg { arg bundle, aGroup;
		^bundle.add((aGroup ? group).moveNodeToTailMsg(this));
	}

	//another syntax style
	getMsg { arg cmdName, argList;
		^[cmdName, nodeID] ++ argList
	}
	msgToBundle { arg bundle, cmdName, argList;
		^bundle.add([cmdName, nodeID] ++ argList)
	}
	newMsg { arg target, addAction=\addToHead, args;
		var bundle, addActionNum;
		target = target.asTarget;
		^this.perform(addAction, target, args)
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
	
	nodeToServerMsg { 
		^this.subclassResponsibility(thisMethod)       
	}

	register { arg argServer;
		server = argServer;
		nodeID = server.nextNodeID;
	}
	// not used ?
	remove {
		group = nil;
		isPlaying = false;
		isRunning = false;
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
		var group, server;
		target = target.asTarget;
		server = target.server;
		group = this.basicNew(server);
		server.sendBundle(nil, group.perform(addAction, target));
		^group
	}
	*after { arg aNode;    ^this.new(aNode, \addAfter) }
	*before {  arg aNode; 	^this.new(aNode, \addBefore) }
	*head { arg target; 	^this.new(target.asGroup, \addToHead) }
	*tail { arg target; 	^this.new(target.asGroup, \addToTail) }
	*replace { arg groupToReplace; ^this.new(groupToReplace, \addReplace) }
	
	moveNodeToHead { arg aNode;
		aNode.group = this;
		server.sendBundle(nil,
			[22, nodeID, aNode.nodeID]); //"/g_head"
	}
	moveNodeToTail { arg aNode;
		aNode.group = this;
		server.sendBundle(nil,
			[23, nodeID, aNode.nodeID]); //"/g_tail"
	}
	moveNodeBefore { arg  movedNode, aNode;
		movedNode.group = this;
		server.sendBundle(nil,
			[18, movedNode.nodeID, aNode.nodeID]); //"/n_before"
	}
	moveNodeAfter { arg  movedNode, aNode;
		movedNode.group = this;
		server.sendBundle(nil,
			[19, movedNode.nodeID, aNode.nodeID]); //"/n_after"
	}  
	freeAll {
		// free my children, but this node is still playing
		server.sendBundle(nil, ["/g_freeAll", nodeID]); //"/g_freeAll"
	}
	deepFree {
		server.sendBundle(nil, ["/g_deepFree", nodeID])
	}

	/*  return messages */
	addToHeadMsg { arg arggroup; 
		group = arggroup.asGroup;
		^["/g_new", nodeID, 0, group.nodeID] 
	}
	addToTailMsg { arg arggroup;
		group = arggroup.asGroup;
		^["/g_new", nodeID, 1, group.nodeID] 
	}
	addAfterMsg {  arg afterThisOne;
		group = afterThisOne.group;
		^["/g_new", nodeID, 3, afterThisOne.nodeID] 
	}
	addBeforeMsg {  arg beforeThisOne;
		group = beforeThisOne.group;
		^["/g_new", nodeID, 2, beforeThisOne.nodeID] 
	}
	addReplaceMsg { arg removeThisOne;
		group = removeThisOne.group;
		^["/g_new", nodeID, 4, removeThisOne.nodeID] 
	}
	moveNodeToHeadMsg { arg aNode;
		^["/g_head", nodeID, aNode.nodeID]; //"/g_head"
	}
	moveNodeToTailMsg { arg aNode;
		^["/g_tail", nodeID, aNode.nodeID];//g_tail
	}

			
	/** bundle messages **/
	*newToBundle { arg bundle, target, addAction=\addToHead;
		var group;
		target = target.asTarget;
		group = this.basicNew(target.server);
		bundle.add(group.perform(addAction, target));
		^group
	}
	//private
	nodeToServerMsg { arg targetID, addActionNum;
		^["/g_new", nodeID, addActionNum, targetID]  	 // "/g_new"
	}
	

}

Synth : Node {

	var <>defName;
	
	/** immediately sends **/
	*new { arg defName, args, target, addAction=\addToHead;
		var synth, server;
		target = target.asTarget;
		server = target.server;
		synth = this.basicNew(defName, server);
		server.sendBundle(nil, synth.perform(addAction, target, args));
		^synth
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
		var synth, server;
		target = target.asTarget;
		server = target.server;
		synth = this.basicNew(defName, server, -1);
		server.sendBundle(nil, synth.perform(addAction, target, args));
		^synth
	}
	*newLoad { arg defName, args, target, addAction=\addToHead, dir="synthdefs/";
		var msg, synth;
		target = target.asTarget;
		synth = this.basicNew(defName, target.server);
		msg = synth.newMsg(target,addAction,args);
		synth.server.sendMsg(6, dir ++synth.defName++".scsyndef", msg); //"/d_load"
		^synth
	}	
	*newPaused { arg defName, args, target, addAction=\addToHead;
		var bundle, synth;
		bundle = List.new;
		synth = this.newToBundle(bundle, defName,args, target, addAction);
		synth.msgToBundle(bundle, 12, 0); //"/n_run"
		synth.server.listSendBundle(nil, bundle);
		^synth
	}


	/** does not send	(used for bundling) **/
	*basicNew { arg defName, server, nodeID;
		^super.basicNew(server, nodeID).defName_(defName.asDefName)
	}
	*newToBundle { arg bundle, defName, args, target, addAction=\addToHead;
		var synth;
		target = target.asTarget;
		synth = this.basicNew(defName, target.server);
		bundle.add(synth.perform(addAction, target, args));
		^synth
	}

	addToHeadMsg { arg arggroup, args;
		group = arggroup.asGroup;
		^["/s_new", defName, nodeID, 0, group.nodeID] ++ args // s_new
	}
	addToTailMsg { arg arggroup, args;
		group = arggroup.asGroup; 
		^["/s_new", defName, nodeID, 1, group.nodeID] ++ args
	}
	addAfterMsg {  arg afterThisOne, args;
		group = afterThisOne.group; 
		^["/s_new", defName, nodeID, 3, afterThisOne.nodeID] ++ args
	}
	addBeforeMsg {  arg beforeThisOne, args;
		group = beforeThisOne.group; 
		^["/s_new", defName, nodeID, 2, beforeThisOne.nodeID] ++ args
	}
	addReplaceMsg { arg removeThisOne, args;
		group = removeThisOne.group; 
		^["/s_new", defName, nodeID, 4, removeThisOne.nodeID] ++ args
	}
	
	get { arg index, action;
		OSCpathResponder(server.addr,['/n_set',nodeID,index],{ arg time, r, msg; 
			action.value(msg.at(3)); r.remove }).add;
		server.listSendMsg(["/s_get", nodeID, index]);
	}
	
	getn { arg index, count, action;

		OSCpathResponder(server.addr,['/n_setn',nodeID,index],{arg time, r, msg;
			action.value(msg.copyToEnd(4)); r.remove } ).add; 
		server.listSendMsg(["/s_getn", nodeID, index, count]);
	}
		
	// private
	*prNew { arg defName, server;
		^this.basicNew(defName, server)
	}
	nodeToServerMsg { arg targetID, addActionNum, args;
		^["/s_new", defName, nodeID, addActionNum, targetID] ++ args
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
	
	remove {}

	// post warning ?
	run {}
	free {}
	moveBefore {}
	moveAfter {}
	moveToHead {}
	moveToTail{}

	*freeAll {
		roots.do({ arg rn; rn.freeAll })
	}
} 


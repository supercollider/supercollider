
Node {
	
	var <>nodeID, <>group, <>prev, <>next;
	var <>server;
	var <>isPlaying = false, <>isRunning = false;
	
	*basicNew { arg nodeID,server;
		server = server ? Server.local;
		^super.new.server_(server).nodeID_(nodeID ?? {server.nextNodeID})
	}

	free { arg sendFlag=true;
		if(sendFlag, {
			server.sendBundle(nil, [11, nodeID]);  //"/n_free"
		});
		this.remove;
	}
	run { arg flag=true;
		server.sendBundle(nil, [12, nodeID,flag.binaryValue]); //"/n_run"
	}
	map { arg controlName, busIndex ... args;
		server.sendBundle(nil, 
			[14, nodeID,controlName, busIndex]++args); //"/n_map"
	}
	set { arg controlName, value ... args;
		server.sendBundle(nil, 
			[15, nodeID,controlName, value]++args); //"/n_set"
	}
	setWithArray { arg args;
		server.sendBundle(nil, 
			[15, nodeID] ++ args); //"/n_set"
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
				++ nargs)); // "n_setn"
	}
	fill { arg controlName, numControls, value ... args;
		server.sendBundle(nil, 
			[17, nodeID,controlName,numControls, value]++args); //"n_setn"
	}
	release { //assumes a control called 'gate' in the synth
		server.sendBundle(nil, 
			[15, nodeID, \gate, 0]) //"/n_set"
	}	      

	moveBefore { arg aNode;
		aNode.group.moveNodeBefore(this, aNode);
	}
	moveAfter { arg aNode;
		aNode.group.moveNodeAfter(this, aNode);
	}       
	moveToHead { arg aGroup;
		(aGroup ? group).moveNodeToHead(this);
	}
	moveToTail { arg aGroup;
		(aGroup ? group).moveNodeToTail(this);
	}
	
	
	
	// return messages for bundling
	addToHead { arg arggroup,args; 
		group = arggroup.asGroup;
		^this.nodeToServerMsg(group.nodeID, 0, args);
	}
	addToTail { arg arggroup,args;
		group = arggroup.asGroup;
		^this.nodeToServerMsg(group.nodeID, 1, args);
	}
	addAfter {  arg afterThisOne,args;
		group = afterThisOne.group;
		^this.nodeToServerMsg(afterThisOne.nodeID, 3, args);
	}
	addBefore {  arg beforeThisOne,args;
		group = beforeThisOne.group;
		^this.nodeToServerMsg(beforeThisOne.nodeID, 2, args);
	}
	addReplace { arg removeThisOne,args;
		group = removeThisOne.group;
		^this.nodeToServerMsg(removeThisOne.nodeID, 4, args);
	}
	moveBeforeMsg { arg  bundle, aNode;
		^bundle.add([18, nodeID, aNode.nodeID]);//"/n_after"
	}
	moveAfterMsg { arg bundle, aNode;
		^bundle.add([19, nodeID, aNode.nodeID]); //"/n_after"
	}       
	moveToHeadMsg { arg bundle, aGroup;
		^(aGroup ? group).moveNodeToHeadMsg(bundle, this);
	}
	moveToTailMsg { arg bundle, aGroup;
		^(aGroup ? group).moveNodeToTailMsg(bundle, this);
	}
	//another syntax style
	getMsg { arg cmdName, argList;
		^[cmdName, nodeID] ++ argList
	}
	msgToBundle { arg bundle, cmdName, argList;
		^bundle.add([cmdName, nodeID] ++ argList)
	}
	newMsg { arg target, addAction=\addToTail, args;
		var bundle, addActionNum;
		target = target.asTarget;
		^this.perform(addAction, target, args)
	}
	


	// private
	*prNew { arg server;
		^super.new.register(server)
	}
	prSetNodeID { arg id;  nodeID = id ?? {server.nextNodeID}  }
	prSetPlaying { arg flag=true; isPlaying = flag; isRunning = flag }
	prMoveAfter {  arg afterThisOne;
		this.group = afterThisOne.group;
		prev = afterThisOne;
		next = afterThisOne.next;
		if (afterThisOne.next.notNil, { 
			afterThisOne.next.prev = this; 
		},{ 
			group.tail = this; 
		});
		afterThisOne.next = this;
	}
	prMoveBefore {  arg beforeThisOne;
		this.group = beforeThisOne.group;
		next = beforeThisOne;
		prev = beforeThisOne.prev;
		if (beforeThisOne.prev.notNil, { 
			beforeThisOne.prev.next = this; 
		},{ 
			group.head = this; 
		});
		beforeThisOne.prev = this;
	}
	nodeToServer { arg addAction,target,args;
		var msg;
		msg = [this.perform(addAction,target,args)];
		target.group.finishBundle(msg, this);
		server.listSendBundle(nil, msg);
		//isPlaying = isRunning = true;
	}
	nodeToServerMsg { 
		^this.subclassResponsibility(thisMethod)       
	}

	register { arg argServer;
		server = argServer;
		nodeID = server.nextNodeID;
	}
	remove {
		if (next.notNil, { 
			next.prev = prev; 
		},{
			if(group.notNil, { group.tail = prev });
		});
		if (prev.notNil, { 
			prev.next = next; 
		},{
			if(group.notNil, { group.head = next });
		});
		group = next = prev = nil;
		isPlaying = false;
		isRunning = false;
	}
	// for Group-deepDo
	deepDo {	}
	asStream { ^Ref(this) } //insulate next

	hash {  ^server.hash bitXor: nodeID.hash	}
	== { arg aNode;
		^(aNode.nodeID == nodeID) and: {aNode.server === server}
	}
	printOn { arg stream; stream << this.class.name << "(" << nodeID <<")" }

}


Group : Node {

	var <>head, <>tail;
	
	*new { arg target,addAction=\addToTail;
		var group, server;
		target = target.asTarget;
		server = target.server;
		group = this.prNew(server);
		group.nodeToServer(addAction,target);
		^group
	}
	*after { arg aNode;     ^this.new(aNode, \addAfter) }
	*before {  arg aNode; ^this.new(aNode, \addBefore) }
	*head { arg aGroup; ^this.new(aGroup.asGroup, \addToHead) }
	*tail { arg aGroup; ^this.new(aGroup.asGroup, \addToTail) }
	*replace { arg groupToReplace; ^this.new(groupToReplace, \addReplace) }
	
	moveNodeToHead { arg aNode;
		server.sendBundle(nil,
			[22, nodeID, aNode.nodeID]); //"/g_head"
	}
	moveNodeToTail { arg aNode;
		server.sendBundle(nil,
			[23, nodeID, aNode.nodeID]); //"/g_tail"
	}
	moveNodeBefore { arg  movedNode, aNode;
		server.sendBundle(nil,
			[18, movedNode.nodeID, aNode.nodeID]); //"/n_before"
	}
	moveNodeAfter { arg  movedNode, aNode;
		server.sendBundle(nil,
			[19, movedNode.nodeID, aNode.nodeID]); //"/n_after"
	}       
	
	nodeToServerMsg { arg targetID, addActionNum;
		^["/g_new", nodeID, addActionNum, targetID]  // 21
	}
		
	freeAll { arg sendFlag=true;
		// free my children, but this node is still playing
		server.sendBundle(nil,[24,nodeID]); //"/g_freeAll"
	}
	
			
	/** bundle messages **/
	*newToBundle { arg bundle, target, addAction=\addToTail;
		var group;
		target = target.asTarget;
		group = this.prNew(target.server);
		bundle.add(group.perform(addAction, target));
		group.group.finishBundle(bundle, group);
		^group
	}
	
	moveNodeToHeadMsg { arg aNode;
		^["/g_head", nodeID, aNode.nodeID]; //"/g_head"   22
	}
	
	moveNodeToTailMsg { arg aNode;
		^[23, nodeID, aNode.nodeID];//g_tail
	}
	finishBundle { //overridden in subclass
	}
	
	//these work only with linked nodes.
	do { arg function;
		var node, nextNode;
		node = head;
		while({ node.notNil }, {
			nextNode = node.next;
			function.value(node);
			node = nextNode;
		});		     
	}
	deepDo { arg function;
		var node, nextNode;
		node = head;
		while({ node.notNil }, {
			nextNode = node.next;
			function.value(node);
			node.deepDo(function);
			node = nextNode;
		});
	}

	//private
	prAddHead { arg node;
		node.group = this;
		node.prev = nil;
		node.next = head;
		if(head.notNil, { head.prev = node });
		head = node;
		if (tail.isNil, {
			tail = node;
		});
	}
	prAddTail { arg node;
		node.group = this;
		node.next = nil;
		node.prev = tail;
		if(tail.notNil, { tail.next = node });
		tail = node;
		if (head.isNil, {
			head = node;
		});
	}
}

Synth : Node {

	var <>defName;
	
	/** immediately sends **/
	*new { arg defName,args,target,addAction=\addToTail;
		var synth, server;
		target = target.asTarget;
		server = target.server;
		synth = this.prNew(defName, server);
		synth.nodeToServer(addAction, target, args);
		^synth
	}
	*after { arg aNode,defName,args;	
		^this.new(defName, args, aNode, \addAfter);
	}
	*before {  arg aNode,defName,args;
		^this.new(defName, args, aNode, \addBefore); 
	}
	*head { arg aGroup,defName,args; 
		^this.new(defName, args, aGroup, \addToHead); 
	}
	*tail { arg aGroup,defName,args; 
		^this.new(defName, args, aGroup, \addToTail);  
	}
	*replace { arg synthToReplace, defName, args;
		^this.new(defName, args, synthToReplace, \addReplace)
	}
	// nodeID -1 
	*grain { arg defName,args,target,addAction=\addToTail;
		var synth, server;
		target = target.asTarget;
		server = target.server;
		synth = this.basicNew(defName, server,-1);
		synth.nodeToServer(addAction, target, args);
		^synth
	}
	*newLoad { arg defName,args,target,addAction=\addToTail,dir="synthdefs/";
		var msg, synth;
		target = target.asTarget;
		synth = this.prNew(defName, target.server);
		msg = synth.newMsg(target,addAction,args);
		synth.server.sendMsg(6, dir ++synth.defName++".scsyndef", msg); //"/d_load"
		^synth
	}	
	*newPaused { arg defName,args,target,addAction=\addToTail;
		var bundle, synth;
		bundle = List.new;
		synth = this.newToBundle(bundle, defName,args,target,addAction);
		synth.msgToBundle(bundle, 12, 0); //"/n_run"
		synth.server.listSendBundle(nil, bundle);
		^synth
	}


	/** does not send	(used for bundling) **/
	*basicNew { arg defName,server,nodeID;
		^super.basicNew(nodeID,server).defName_(defName)
	}
	//  bundle is a List
	*newToBundle { arg bundle, defName, args, target, addAction=\addToTail;
		var synth;
		target = target.asTarget;
		synth = this.prNew(defName, target.server);
		bundle.add(synth.perform(addAction, target, args));
		synth.group.finishBundle(bundle, synth);
		^synth
	}

	// basic msg construction
	addToHeadMsg { arg arggroup,args;
		group = arggroup;
		^[9, defName, nodeID, 0, group.nodeID] ++ args // s_new
	}
	addToTailMsg { arg arggroup,args;
		group = arggroup; 
		^[9, defName, nodeID, 1, group.nodeID] ++ args
	}
	addAfterMsg {  arg afterThisOne,args;
		group = afterThisOne.group; 
		^[9, defName, nodeID, 3, afterThisOne.nodeID] ++ args
	}
	addBeforeMsg {  arg beforeThisOne,args;
		group = beforeThisOne.group; 
		^[9, defName, nodeID, 2, beforeThisOne.nodeID] ++ args
	}
	addReplaceMsg { arg removeThisOne,args;
		group = removeThisOne.group; 
		^[9, defName, nodeID, 4, removeThisOne.nodeID] ++ args
	}
	
	
	trace {
		server.sendMsg(10, nodeID);//"/s_trace"
	}

	// private
	*prNew { arg defName, server;
		^super.prNew(server).defName_(defName.asDefName)
	}
	nodeToServerMsg { arg targetID, addActionNum, args;
		^[9, defName, nodeID, addActionNum, targetID] ++ args
	}
}

RootNode : Group {
	
	classvar <roots;
	
	
	*new { arg server, connected=false;
		server = server ?? {Server.local};
		^(roots.at(server.name) ?? {
			^super.prNew(server).rninit//.connect(connected)
		})
	}
	rninit { 
		roots.put(server.name, this);
		isPlaying = isRunning = true;
		group = this; // self
	}
	register { arg srv; 
		server = srv;
		nodeID = 0; 
	}
	*initClass {  roots = IdentityDictionary.new; }
	
	nodeToServer {} // already running
	
	remove {
		if (next.notNil, { 
			next.prev = prev; 
		});
		if (prev.notNil, { // i have no prev
			prev.next = next; 
		});
		next = prev = nil;
	}

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


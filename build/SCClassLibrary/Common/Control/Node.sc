
Node {
	
	var <nodeID, <group, <>prev, <>next;
	var <server;
	var <>isPlaying = false,<>isRunning = false;
	
	free { arg sendCmd = true; 
		/* might be called by the group doing g_freeAll
			so then we wouldn't want to send the command, just free the
			structure */
		if(isPlaying, {
			this.remove;
			this.isPlaying = false;
			(this.asString ++ " was freed").postln;
			if(sendCmd, { 
				server.sendBundle(server.latency, ["/n_free", nodeID]); 
			});
		});
	}
	run { arg flag=true;
		isRunning = flag;
		server.sendBundle(server.latency, ["/n_run", nodeID,flag.binaryValue]);
	}
	set { arg controlName, value ... args;
		server.sendBundle(server.latency, 
			["/n_set", nodeID,controlName, value]++args);
	}	
	setn { arg controlName,  values ... args;
		args = args.collect({ arg cnv;
			[cnv.at(0), cnv.at(1).size, cnv.at(1)];
		}).flat;
		server.sendBundle(server.latency, 
			(["/n_setn", nodeID,controlName, values.size] ++ values 
				++ args).postln);
	}
	fill { arg controlName, numControls, value ... args;
		server.sendBundle(server.latency, 
			["/n_fill", nodeID,controlName,numControls, value]++args);
	}	
	map { arg controlName, busIndex ... args;
		server.sendBundle(server.latency, 
			["/n_map", nodeID,controlName, busIndex]++args);
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
	//support for Group::deepDo
	deepDo { 	}
	
	== { arg aNode;
		^(aNode.server === server) && (aNode.nodeID == nodeID)
	}
	hash { 	^server.hash bitXor: nodeID.hash	}
	printOn { arg stream; stream << this.class.name << "(" << nodeID <<")" }
	
	/** PRIVATE IMPLEMENTATION  **/
	*prNew {
		^super.new //just return the instance, gets called by RootNode
	}
	
	initNodeID {  nodeID = server.nodeAllocator.alloc  }
	
	group_ { arg g;
		group = g;
		server = group.server;
	}
	nodeToServer { arg addActionNum,targetID,args;
		 ^this.subclassResponsibility(thisMethod)	
	}
	
	addToHead { arg arggroup,args;
		arggroup.prAddHead(this); // sets group
		this.nodeToServer(0,group.nodeID,args);
	}
	addToTail { arg arggroup,args;
		arggroup.prAddTail(this);
		this.nodeToServer(1,group.nodeID,args);
	}
	addAfter {  arg afterThisOne,args;
		this.prMoveAfter(afterThisOne);
		this.nodeToServer(3,afterThisOne.nodeID,args);
	}
	addBefore {  arg beforeThisOne,args;
		this.prMoveBefore(beforeThisOne);
		this.nodeToServer(2,beforeThisOne.nodeID,args);
	}
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
	remove {
		if (next.notNil, { 
			next.prev = prev; 
		},{
			group.tail = prev;
		});
		if (prev.notNil, { 
			prev.next = next; 
		},{
			group.head = next;
		});
		group = next = prev = nil;
	}
	
}


Group : Node {

	var <>head, <>tail;
	
	*new { arg target,addAction=\addToTail;
		^super.new.perform(addAction,target.asTarget)
	}
	*after { arg aNode;	^this.prNew.addAfter(aNode) }
	*before {  arg aNode; ^this.prNew.addBefore(aNode) }
	*head { arg aGroup; ^this.prNew.addToHead(aGroup.asGroup) }
	*tail { arg aGroup; ^this.prNew.addToTail(aGroup.asGroup) }

	
	moveNodeToHead { arg aNode;
		aNode.remove;
		this.prAddHead(aNode);
		server.sendBundle(server.latency,["/g_head", nodeID, aNode.nodeID]);
	}
		
	moveNodeToTail { arg aNode;
		aNode.remove;
		this.prAddTail(aNode);
		server.sendBundle(server.latency,["/g_tail", nodeID, aNode.nodeID]);
	}
	
	moveNodeBefore { arg  movedNode, aNode;
		movedNode.remove;
		movedNode.prMoveBefore(aNode);
		server.sendBundle(server.latency,
			["/n_before", movedNode.nodeID, aNode.nodeID]);
	}
	
	moveNodeAfter { arg  movedNode, aNode;
		movedNode.remove;
		movedNode.prMoveAfter(aNode);
		server.sendBundle(server.latency,
			["/n_after", movedNode.nodeID, aNode.nodeID]);
	}	
	
	nodeToServer { arg addActionNum,targetID;
		group.sendGroupToServer(this, addActionNum,targetID); 	}
	
	sendGroupToServer { arg arggroup, addActionNum,targetID;
		if(server.serverRunning, {
			arggroup.initNodeID;
			server.sendBundle(server.latency,
				["/g_new", arggroup.nodeID, addActionNum, targetID].postln
			);
			
			arggroup.isPlaying = true;
			arggroup.isRunning = true;
		}, { "Server not running".inform });
	
	}
	
	sendSynthToServer { arg argsynth, addActionNum,targetID,args;
		if(server.serverRunning, {
			argsynth.initNodeID;
			server.sendBundle(server.latency,
				(["/s_new", argsynth.defName, argsynth.nodeID, 
					addActionNum, targetID] 
				++ args).postln);
			
			argsynth.isPlaying = true;
			argsynth.isRunning = true;
		}, { "Server not running".inform });
	
	}
	
	freeChildren {
		this.do({ arg node;
			node.free(false);
		});
	}
	freeAll { 	
		server.sendBundle(server.latency,["/g_freeAll",nodeID]);
		this.freeChildren;
		// free my children, but this node is still playing
	}
	free {
		super.free;
		this.freeChildren;
		
	}
	
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
	
	*new { arg defName,args,target,addAction=\addToTail;
		^this.prNew(defName).perform(addAction,target.asTarget,args ?? {[]})
	}
	*prNew { arg defName;
		^super.new.defName_(defName.asDefName)
	}
	
	*after { arg aNode,defName,args;	
		^this.prNew(defName).addAfter(aNode,args) 
	}
	*before {  arg aNode,defName,args;
		^this.prNew(defName).addBefore(aNode,args) 
	}
	*head { arg aGroup,defName,args; 
		^this.prNew(defName).addToHead(aGroup.asGroup,args) 
	}
	*tail { arg aGroup,defName,args; 
		^this.prNew(defName).addToTail(aGroup.asGroup,args) 
	}
	
	nodeToServer { arg addActionNum,targetID,args;
		group.sendSynthToServer(this, addActionNum,targetID,args); 	}

	trace {
		server.sendMsg("/s_trace", nodeID);
	}
}

RootNode : Group {
	
	classvar <roots;
	
	*new { arg server;
		server = server ?? {Server.local};
		^(roots.at(server.name) ?? {
			^super.prNew.rninit(server)
		})
	}
	rninit { arg s; 
		server = s;
		roots.put(s.name,this);
		nodeID = 0;
		isPlaying = isRunning = true;
		group = this; // self
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
	moveBefore { }
	moveAfter { 	}
	moveToHead { 	}
	moveToTail{}
	
	stop {
		this.freeAll;
	}
	*stop { arg server;
		var r;
		if (server.isNil, {
			roots.do({ arg rn; rn.stop; });
		}, {
			r = roots.at(server.name);
			if(r.notNil, { r.stop });
		});
	}
}



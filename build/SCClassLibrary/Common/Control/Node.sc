
Node {
	
	var <nodeID, <group, <>prev, <>next;
	var <server;
	var <>isPlaying = false, <>isRunning = false;
	
	free {  arg sendFlag=true;
		if(sendFlag, {
			server.sendBundle(server.latency, [11, nodeID]);  //"/n_free"
		}, {
			this.unregister;
		});
	}
	run { arg flag=true;
		server.sendBundle(server.latency, [12, nodeID,flag.binaryValue]); //"/n_run"
	}
	map { arg controlName, busIndex ... args;
		server.sendBundle(server.latency, 
			[14, nodeID,controlName, busIndex]++args); //"/n_map"
	}
	set { arg controlName, value ... args;
		server.sendBundle(server.latency, 
			[15, nodeID,controlName, value]++args); //"/n_set"
	}
	setn { arg controlName,  values ... args;
		args = args.collect({ arg cnv;
			[cnv.at(0), cnv.at(1).size, cnv.at(1)];
		}).flat;
		server.sendBundle(server.latency, 
			([16, nodeID,controlName, values.size] ++ values 
				++ args).postln); // "n_setn"
	}
	fill { arg controlName, numControls, value ... args;
		server.sendBundle(server.latency, 
			[17, nodeID,controlName,numControls, value]++args); //"n_setn"
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
	release { //assumes a control called 'gate' in the synth
		server.sendBundle(server.latency, [15, nodeID, \gate, 0]) //"/n_set"
	}	       

	
	
	/**  message and bundle creation **/
	// create messages but don't send them or add them to parent node objects
	getMsg { arg cmdName, argList;
		^[cmdName, nodeID] ++ (argList ? #[]);
	}
	addMsg { arg bundle, cmdName, argList;
		^bundle.add([cmdName, nodeID] ++ (argList ? #[]));
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
	
	
	//support for Group::deepDo
	deepDo {	}
	
	== { arg aNode;
		^(aNode.nodeID == nodeID) and: {aNode.server === server}
	}

	
	
	/** PRIVATE IMPLEMENTATION  **/	
	*prNew {
		^super.new //just return the instance, gets called by RootNode
	}
	//remote node creation needs to set id. normally it is not passed in
	prSetNodeID { arg id;  nodeID = id ?? {server.nodeAllocator.alloc}  }
	//in some cases these need to be set.
	prSetServer { arg s; server = s ?? {Server.local} }
	prSetPlaying { arg flag=true; isPlaying = flag }
	freeNodeID {  server.nodeAllocator.free(nodeID); nodeID = nil  }
	convertAddAction { arg symbol;
		^#[\addToHead, \addToTail, \addBefore, \addAfter,\addBefore,\addReplace].indexOf(symbol) ? 1
	}

	group_ { arg g;
		group = g;
		server = group.server;
	}
	nodeToServer { arg addActionNum,targetID,args;
		 ^this.subclassResponsibility(thisMethod)       
	}
	
	addToHead { arg arggroup,args;
		group = arggroup; 
		this.nodeToServer(0,arggroup.nodeID,args);
	}
	addToTail { arg arggroup,args;
		group = arggroup; 
		this.nodeToServer(1,arggroup.nodeID,args);
	}
	addAfter {  arg afterThisOne,args;
		group = afterThisOne.group; 
		this.nodeToServer(3,afterThisOne.nodeID,args);
	}
	addBefore {  arg beforeThisOne,args;
		group = beforeThisOne.group; 
		this.nodeToServer(2,beforeThisOne.nodeID,args);
	}
	addReplace { arg removeThisOne,args;
		group = removeThisOne.group; 
		this.nodeToServer(4,removeThisOne.nodeID,args);
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
			if(group.notNil, { group.tail = prev });
		});
		if (prev.notNil, { 
			prev.next = next; 
		},{
			if(group.notNil, { group.head = next });
		});
		group = next = prev = nil;
	}
	
	register { arg argServer;
		server = argServer;
		this.prSetNodeID;
		RootNode(server).registerNode(this);
	}
	unregister {
		RootNode(server).unregisterNode(this);
		this.freeNodeID;
	}
	
	hash {  ^server.hash bitXor: nodeID.hash	}
	printOn { arg stream; stream << this.class.name << "(" << nodeID <<")" }

}


Group : Node {

	var <>head, <>tail;
	
	*new { arg target,addAction=\addToTail;
		^super.new.perform(addAction,target.asTarget)
	}
	*after { arg aNode;     ^this.prNew.addAfter(aNode) }
	*before {  arg aNode; ^this.prNew.addBefore(aNode) }
	*head { arg aGroup; ^this.prNew.addToHead(aGroup.asGroup) }
	*tail { arg aGroup; ^this.prNew.addToTail(aGroup.asGroup) }
	*replace { arg groupToReplace;
		^this.new(groupToReplace, \addReplace)
	}
	
	moveNodeToHead { arg aNode;
		server.sendBundle(server.latency,
			[22, nodeID, aNode.nodeID]); //"/g_head"
	}
		
	moveNodeToTail { arg aNode;
		server.sendBundle(server.latency,
			[23, nodeID, aNode.nodeID]); //"/g_tail"
	}
	
	moveNodeBefore { arg  movedNode, aNode;
		server.sendBundle(server.latency,
			[18, movedNode.nodeID, aNode.nodeID]); //"/n_before"
	}
	
	moveNodeAfter { arg  movedNode, aNode;
		server.sendBundle(server.latency,
			[19, movedNode.nodeID, aNode.nodeID]); //"/n_after"
	}       
	
	nodeToServer { arg addActionNum,targetID;
		group.sendGroupToServer(this, addActionNum,targetID);   
	}
	
	sendGroupToServer { arg arggroup, addActionNum,targetID;
		if(server.serverRunning, {
			arggroup.register(server);
			server.sendBundle(server.latency,
				[21, arggroup.nodeID, addActionNum, targetID] //"/g_new"
			);
		}, { "Server not running".inform });
	}
	
	sendSynthToServer { arg argsynth, addActionNum,targetID,args;
		if(server.serverRunning, {
			argsynth.register(server);
			server.sendBundle(server.latency,
				([9, argsynth.defName, argsynth.nodeID, //"/s_new"
					addActionNum, targetID] 
				++ args));
		}, { "Server not running".inform });
	}
	
	freeAll {       
		server.sendBundle(server.latency,[24,nodeID]); //"/g_freeAll"
		// free my children, but this node is still playing
	}
	
	do { arg function;
		var node;
		node = head;
		while({ node.notNil }, {
			function.value(node);
			node = node.next;
		});		     
	}
	
	deepDo { arg function;
		var node;
		node = head;
		while({ node.notNil }, {
			node.deepDo(function);
			function.value(node);
			node = node.next;
		});
	}
		
	// bundle messages //

	*newMsg { arg bundle, target, addAction;
		var group;
		group = this.prNew;
		bundle.add(group.newMsg(target, addAction));
		^group
	}
	newMsg { arg target, addAction;
		var addActionNum;
		target = target.asTarget;
		this.register(target.server);
		addActionNum = this.convertAddAction(addAction);
		^[21, nodeID, addActionNum, target.nodeID]; //"/g_new"
	}

	moveNodeToHeadMsg { arg aNode;
		^[22, nodeID, aNode.nodeID]; //"/g_head"
	}
	
	moveNodeToTailMsg { arg aNode;
		^[23, nodeID, aNode.nodeID];//g_tail
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
		^this.prNew(defName).perform(addAction,target.asTarget,args ? (#[]))
	}
	*prNew { arg defName;
		^super.new.defName_(defName.asDefName)
	}
	
	*newLoad { arg defName,args,target,addAction=\addToTail,dir="synthdefs/";
		var msg, synth;
		synth = this.prNew(defName);
		msg = synth.newMsg(target,args,addAction);
		synth.server.sendMsg(6, dir ++synth.defName++".scsyndef", msg); //"/d_load"
		^synth
	}
	
	*replace { arg synthToReplace, defName, args;
		^this.new(defName, args, synthToReplace, \addReplace)
	}
	
	*newPaused {arg defName,args,target,addAction=\addToTail;
		var bundle, synth;
		bundle = List.new;
		synth = this.newMsg(bundle, defName,args,target,addAction);
		synth.addMsg(bundle, 12, 0); //"/n_run"
		synth.server.listSendBundle(nil, bundle);
		^synth
	}
	//no linking, only use for self releasing nodes
	*newUnlinked { arg defName,args,target,addAction=\addToTail;
		var synth, server;
		target = target.asTarget;
		server = target.server;
		synth = this.prNew(defName).register(server);
		addAction = synth.convertAddAction(addAction ?? { this.defaultAddAction });	     server.sendBundle(server.latency, 
		 	[9, synth.defName, synth.nodeID, addAction, target.nodeID] ++ (args ? #[]));
		^synth
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
		group.sendSynthToServer(this, addActionNum,targetID,args);      
	}

	trace {
		server.sendMsg(10, nodeID);//"/s_trace"
	}
	
	
	///  bundle messages  ///
	//this adds the message to the bundle and returns the synth.
	// bundle should be a List
	*newMsg { arg bundle, defName, args, target, addAction=\addToTail;
		var synth;
		synth = this.prNew(defName);
		bundle.add(synth.newMsg(target, addAction, args));
		^synth
	}
	
	newMsg { arg target, addAction=\addToTail, args;
		var bundle, addActionNum;
		target = target.asTarget;
		addActionNum = this.convertAddAction(addAction);
		this.register(target.server);
		bundle = [9, defName, nodeID, addActionNum, target.nodeID]; //"/s_new"
		^if(args.notNil,{ bundle ++ args }, { bundle });
	}
}

RootNode : Group {
	
	classvar <roots;
	var nodeWatcher, <connected;
	
	*new { arg server, connected=true;
		server = server ?? {Server.local};
		^(roots.at(server.name) ?? {
			^super.prNew.rninit(server)
		}).connect(connected)
	}
	rninit { arg s; 
		server = s;
		roots.put(s.name, this);
		nodeID = 0;
		isPlaying = isRunning = true;
		group = this; // self
	}
	
	*initClass {  roots = IdentityDictionary.new; }
	nodeToServer {} // already running

	connect { connected = true; if(nodeWatcher.notNil, { nodeWatcher.startListen }); }
	disconnect { connected = false; if(nodeWatcher.notNil, { nodeWatcher.stopListen }); }
	
	nodeWatcher { //lazy init
		^nodeWatcher ?? {
			nodeWatcher = NodeWatcher(server);
			nodeWatcher.startListen;
			nodeWatcher.register(this);
		}
	}
	
	registerNode { arg node;
		if(connected and: {server.serverRunning}, {
			this.nodeWatcher.register(node);
		})
	}
	
	unregisterNode { arg node;
		if(connected, {
			nodeWatcher.unregister(node);
		})
	}

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
	
	deepDo {} 
	
	stop {	this.freeAll;	}
	clear { this.nodeWatcher.clear }
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

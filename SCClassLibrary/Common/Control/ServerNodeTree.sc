ServerNodeTree {

	var server;
	var updateFunc, updater, <>refreshRate = 0.2;
	var currentMsg, currentNodeTree = nil;
	var <running = false;
	var oscResp;

	*new { |server|
		^super.new.init(server)
	}

	init { |parentServer|
		server = parentServer;
	}

	getNodeTree {
		if(running) {
			if(currentNodeTree.isNil) {
				this.prFormatNodeTree;
			};
			^currentNodeTree
		} {
			^nil
		};
	}

	run {
		if(running.not) {
			// Init osc receiver
			oscResp = OSCFunc(
				{ |msg|
					// Only catch relevant messages
					// (root node and controls enabled)
					if((msg[1] == 1) and: { msg[2] == 0 }) {
						currentNodeTree = nil;
						currentMsg = msg;
					};
				},
				'/g_queryTree.reply',
				server.addr
			).fix;
			// Start auto update thread
			updateFunc = {
				updater = fork {
					loop {
						server.sendMsg(
							"/g_queryTree",
							0,
							1
						);
						refreshRate.wait;
					};
				};
				running = true;
			};
		};
		updateFunc.value;
		CmdPeriod.add(updateFunc);
	}

	stop {
		if(running) {
			oscResp.free;
			updater.stop;
			CmdPeriod.remove(updateFunc);
			running = false;
		};
	}

	/*    PRIVATE METHODS    */

	// Called when all children in group have been processed
	prFinalizeGroup { |parentList, index|
		var group = parentList.last;
		group.removeAt(\nChildren);
		parentList.pop;
		// Check if this node is last child in parent,
		// and finalize parent if needed
		if(parentList.size > 0) {
			if(parentList.last[\children].size == parentList.last[\nChildren]) {
				index = this.prFinalizeGroup(parentList, index);
			};
		};
		^(index + 2)
	}

	// Convert integer array representation of a node into readable format
	prAnalyzeNode { |msg, index, nodes, parentList|
		var node = IdentityDictionary();
		// Check if we're not finished:
		if(index < (msg.size - 1)) {
			if(msg[index + 1] == -1) {
				// Node is a synth:
				node[\type] = \synth;
				node[\id] = msg[index];
				node[\name] = msg[index + 2];
				node[\controls] = List();
				if(parentList.size == 0) {
					// If parent list size is nil, this is a root node
					nodes.add(node);
				} {
					// Else, it has a parent
					parentList.last[\children].add(node);
				};
				// Process controls
				if(msg[index + 3] > 0) {
					msg[index + 3].do({
						node[\controls].add(
							[
								msg[index + 4],
								msg[index + 5]
							]
						);
						index = index + 2;
					});
				};
				index = index + 4;
				// Finalize parent if last child
				if(parentList.size > 0) {
					if(parentList.last[\children].size == parentList.last[\nChildren]) {
						// /!\ DO NOT UPDATE INDEX HERE
						this.prFinalizeGroup(parentList, index);
					};
				};
			} {
				// Node is a group:
				node[\type] = \group;
				node[\id] = msg[index];
				node[\children] = List();
				node[\nChildren] = msg[index + 1];
				if(parentList.size == 0) {
					// If parent list size is nil, this is a root node
					nodes.add(node);
				} {
					// Else, it has a parent
					parentList.last[\children].add(node);
				};
				// This is needed to allow recursivity:
				parentList.add(node);
				// Does it have children?
				if(node[\nChildren] > 0) {
					index = index + 2;
				} {
					index = this.prFinalizeGroup(parentList, index);
				};
			};
			index = this.prAnalyzeNode(
				msg,
				index,
				nodes,
				parentList
			);
			^index
		};
	}

	// Recursively convert osc message into readable format
	prFormatNodeTree {
		var nodes = List();
		var parentList = List();
		// Format tree datas
		this.prAnalyzeNode(
			currentMsg,
			4,
			nodes,
			parentList
		);
		currentNodeTree = nodes;
	}

}

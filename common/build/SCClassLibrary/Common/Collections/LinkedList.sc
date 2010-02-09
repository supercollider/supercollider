LinkedListNode {
	var <>prev, <>next, <>obj;
	// this class supports the LinkedList class

	*new { arg item;
		^super.new.obj_(item)
	}
	remove {
		if (prev.notNil, { prev.next_(next); });
		if (next.notNil, { next.prev_(prev); });
		next = prev = nil;
	}
}

LinkedList : SequenceableCollection {
	var head, tail, <size = 0;


	copy {
		var copy = LinkedList.new;
		this.do {|item| copy.add(item) }
		^copy
	}
	species { ^this.class }
	do { arg function;
		var i = 0;
		var node = head;
		while ({ node.notNil },{
			function.value(node.obj, i);
			node = node.next;
			i = i + 1;
		});
	}
	reverseDo { arg function;
		var i = size - 1;
		var node = tail;
		while ({ node.notNil },{
			function.value(node.obj, i);
			node = node.prev;
			i = i - 1;
		});
	}
	addFirst { arg obj;
		var node = LinkedListNode.new(obj);
		if (head.notNil, {
			node.next_(head);
			head.prev_(node);
		});
		head = node;
		if (tail.isNil, {
			tail = node;
		});
		size = size + 1;
	}
	add { arg obj;
		var node = LinkedListNode.new(obj);
		if (tail.notNil, {
			node.prev_(tail);
			tail.next_(node);
		});
		tail = node;
		if (head.isNil, {
			head = node;
		});
		size = size + 1;
	}
	remove { arg obj;
		var node = this.findNodeOfObj(obj);
		if ( node.notNil, {
			if (head == node, { head = node.next; });
			if (tail == node, { tail = node.prev; });
			node.remove;
			size = size - 1;
		});
	}
	pop {
		var node;
		if ( tail.notNil, {
			node = tail;
			tail = node.prev;
			if (head == node, { head = nil; });
			node.remove;
			size = size - 1;
			^node.obj
		},{
			^nil
		});

	}
	popFirst {
		var node;
		if ( head.notNil, {
			node = head;
			head = node.next;
			if (tail == node, { tail = nil; });
			node.remove;
			size = size - 1;
			^node.obj
		},{
			^nil
		});

	}

	first { if (head.notNil, { ^head.obj },{ ^nil }) }
	last  { if (tail.notNil, { ^tail.obj },{ ^nil }) }

	at { arg index;
		var node = this.nodeAt(index);
		if (node.notNil, {
			^node.obj
		},{
			^nil
		})
	}
	put { arg index, item;
		var node = this.nodeAt(index);
		if (node.notNil, {
			node.obj_(item);
		});
	}
	removeAt { arg index;
		var node = this.nodeAt(index);
		if (node.notNil, {
			if (head == node, { head = node.next; });
			if (tail == node, { tail = node.prev; });
			node.remove;
			size = size - 1;
			^node.obj
		},{
			^nil
		});
	}
	// private
	nodeAt { arg index;
		var i = 0, node;
		if ( index >= 0 and: { index < size }, {
			if (index < (size - index), {
				node = head;
				while ({ i != index },{
					node = node.next;
					i = i + 1;
				});
				^node
			},{
				i = size - 1;
				node = tail;
				while ({ i != index },{
					node = node.prev;
					i = i - 1;
				});
				^node
			});
		},{
			this.indexOutOfRange;
			^nil
		})
	}

	findNodeOfObj { arg obj;
		var node = head;
		while ({ node.notNil },{
			if (node.obj == obj, { ^node });
			node = node.next;
		});
		^nil
	}
}

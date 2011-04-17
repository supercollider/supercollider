NodeControl {

	var <node,index,<>name;

	*new { arg node,index;
		^super.newCopyArgs(node,index)
	}
	value_ { arg aval;
		node.set(index,aval)
	}
	readFromBus { arg bus;
		node.map(index,bus)
	}
	stopReadFromBus {
		node.map(index,-1)
	}
	setMsg { arg value; ^[15, node.nodeID,index, value] }

	server { ^node.server }
	group { ^node.group }
}

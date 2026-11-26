NodeControl {

	var <node, index, <>name;

	*new { |node, index|
		^super.newCopyArgs(node, index)
	}
	value_ { |aval|
		node.set(index, aval)
	}
	readFromBus { |bus|
		node.map(index, bus)
	}
	stopReadFromBus {
		node.map(index, -1)
	}
	setMsg { |value| ^[15, node.nodeID, index, value] }

	server { ^node.server }
	group { ^node.group }
}

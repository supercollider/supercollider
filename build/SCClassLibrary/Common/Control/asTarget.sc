
+Server {
	asTarget {
		^RootNode(this)
	}
}

+Node {
	asTarget {
		^this	
	}
}

+Nil {
	asTarget {
		^RootNode.new;
	}
}
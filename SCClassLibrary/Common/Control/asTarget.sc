+Server {
	asTarget { ^Group.basicNew(this, 1) }
	asNodeID { ^0 }
}

+Node {
	asTarget { ^this }
	asNodeID { ^nodeID }
}

+Nil {
	asTarget { ^Server.default.asTarget }
	asNodeID { ^this }
}

+Integer {
	asTarget { ^Group.basicNew(Server.default, this) }
	asNodeID { ^this }
}

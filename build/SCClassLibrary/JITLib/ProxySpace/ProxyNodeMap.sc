ProxyNodeMap : NodeMap {
		translate { arg proxy; 
			^proxy.outbus.tryPerform(\index); 				}
}



ProxyNodeMap : NodeMap {
		translate { arg proxy; 
			var index;
			index = proxy.bus.tryPerform(\index); 					^if(index.notNil, { 										proxy.wakeUp;										index										}, {												 	nil 										});
		}
}



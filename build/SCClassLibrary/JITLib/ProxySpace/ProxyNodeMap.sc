ProxyNodeMap : NodeMap {
		translate { arg proxy; 
			var index;
			index = proxy.bus.index; 							^if(index.notNil, { 										proxy.wakeUp;										index										}, {												 	nil 									});
		}
}



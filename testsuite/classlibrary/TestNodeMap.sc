TestProxyNodeMap : UnitTest {


	test_nodeMap_whenSetWithProxy_maps {
		var proxy = NodeProxy.new;
		var otherProxy = NodeProxy.new;
		var map = proxy.nodeMap;
		map.set(\x, otherProxy);
		this.assert(map.mappingKeys.includes(\x));
		proxy.clear;
		otherProxy.clear;
	}

	test_nodeMap_whenSetWithObject_sets {
		var proxy = NodeProxy.new;
		var object = [1, 2, 3];
		var map = proxy.nodeMap;
		map.set(\x, object);
		this.assert(map.settingKeys.includes(\x));
		proxy.clear;
	}

	test_nodeMap_controlNamesConvertsObjectToControlInput {
		var proxy = NodeProxy.new;
		var map = proxy.nodeMap;
		var buffer = Buffer.alloc(numFrames:1);
		map.set(\x, buffer);
		this.assert(map.controlNames.first.defaultValue == buffer.bufnum);
		buffer.free;
		proxy.clear;
	}
}


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

	test_nodeMap_controlNamesDoesNotConvertObjectToControlInput {
		var proxy = NodeProxy.new;
		var map = proxy.nodeMap;
		var buffer = Buffer.alloc(numFrames:1);
		map.set(\x, buffer);
		this.assertEquals(map.controlNames.first.defaultValue, buffer);
		buffer.free;
		proxy.clear;
	}

	test_nodeMap_controlNamesConvertObjectToControlInputFinally {
		var proxy = NodeProxy.new;
		var map = proxy.nodeMap;
		var buffer = Buffer.new(bufnum: 42, numFrames:1);
		var args, i;
		map.set(\x, buffer);
		args = map.asOSCArgArray;
		i = args.indexOf(\x);
		this.assertEquals(args[i+1], buffer.bufnum);
		buffer.free;
		proxy.clear;
	}
}

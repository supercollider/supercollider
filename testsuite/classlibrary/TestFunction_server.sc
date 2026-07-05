TestFunction_server : UnitTest {

	*features { ^[UnitTestFeatures.needsServerBoot] }

	test_plot {
		var condition = CondVar.new;
		var server = Server(thisMethod.name);
		server.bootSync;
		{ |x| DC.ar(x) }.asBuffer(duration: 0.01, target: server, action: { |b|
			b.get(3, { |val|
				this.assertEquals(val, 0, "unspecified function arguments should pass as 0 when function is written to a buffer");
				b.free;
				condition.signalOne;
			})
		});
		condition.waitFor(2);
		server.quitSync;
		server.remove;
	}
}

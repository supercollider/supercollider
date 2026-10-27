//# VERSION 3.16

TestPatternStreamArg : UnitTest {

	test_streamArg_Env {
		var embed;
		var obj = Env([80, 300], [0]);
		var stream = obj.streamArg;
		this.assertEquals(stream.next, 300, "streamArg should return stream of envelope levels");
	}

	test_streamArg_Env_in_Pcollect {
		var pat = Env([80, 300], [0]);
		var coll = Pcollect({ |x| x * 2 }, pat);
		var stream = coll.asStream;
		this.assertEquals(stream.next, 300 * 2, "Pcollect(env) should return envelope level");
	}

	test_streamArg_Env_in_PatternProxy {
		var coll;
		var proxy = PatternProxy.new;
		var pat = Env([80, 300], [0]);
		proxy.source = pat;
		var stream = proxy.asStream;
		this.assertEquals(stream.next, 300, "PatternProxy of an Env should return envelope level");
	}

	test_streamArg_Env_in_Pbindef {
		var coll;
		var pat = Env([80, 300], [0]);
		var proxy = Pbindef(\x, \dummy, pat);
		var stream = Pbindef(\x).asStream;
		var event = stream.next(Event.default);
		Pdef.clear;
		this.assertEquals(event[\dummy], 300, "Pbindef key of an Env should return envelope level");
	}

}

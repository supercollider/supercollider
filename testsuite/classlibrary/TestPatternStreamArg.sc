
TestPatternStreamArg : UnitTest {

	test_streamArg_Env {
		var obj, embed, stream;
		obj = Env([80, 300], [0]);
		stream = obj.streamArg;
		this.assertEquals(stream.next, 300, "streamArg should return stream of envelope levels");
	}

	test_streamArg_Env_in_Pcollect {
		var pat, coll, stream;
		pat = Env([80, 300], [0]);
		coll = Pcollect({ |x| x * 2 }, pat);
		stream = coll.asStream;
		this.assertEquals(stream.next, 300 * 2, "Pcollect(env) should return envelope level");
	}

	test_streamArg_Env_in_PatternProxy {
		var pat, coll, stream, proxy;
		proxy = PatternProxy.new;
		pat = Env([80, 300], [0]);
		proxy.source = pat;
		stream = proxy.asStream;
		this.assertEquals(stream.next, 300, "PatternProxy of an Env should return envelope level");
	}

	test_streamArg_Env_in_Pbindef {
		var pat, coll, stream, proxy, event;
		pat = Env([80, 300], [0]);
		proxy = Pbindef(\x, \dummy, pat);
		stream = Pbindef(\x).asStream;
		event = stream.next(Event.default);
		Pdef.clear;
		this.assertEquals(event[\dummy], 300, "Pbindef key of an Env should return envelope level");
	}

}

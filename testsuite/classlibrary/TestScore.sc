TestScore : UnitTest {

	test_pattern_asScore_plain {
		var pattern, score, s_new_messages, onsetTimes;
		pattern = Pbind(
			\dur, Pseq([0.5], inf),
			\freq, Prand([200, 300, 500], inf)
		);
		score = pattern.asScore(4.0);
		s_new_messages = score.score.drop(1).select { |x| x[1][1] == \default };
		onsetTimes = s_new_messages.collect { |x| x.first };
		this.assertEquals(onsetTimes, (0, 0.5..4), "onsets should be correctly derived from dur");
	}

	test_pattern_asScore_lag {
		var events, score, s_new_messages, onsetTimes;
		events = [(x: 0, lag: 0.5), (x: 1, dur: 0.5), (x: 2, lag: 0.5)];
		score = Pseq(events).asScore(3);
		s_new_messages = score.score.drop(1).select { |x| x[1][1] == \default };
		onsetTimes = s_new_messages.collect { |x| x.first };
		this.assertEquals(onsetTimes, [0.5, 1, 2], "onsets should be correctly derived from lag and dur");
	}

	test_pattern_asScore_timingOffset {
		var events, score, s_new_messages, onsetTimes;
		events = [(x: 0, timingOffset: 0.5), (x: 1, dur: 0.5), (x: 2, timingOffset: 0.5)];
		score = Pseq(events).asScore(3);
		s_new_messages = score.score.drop(1).select { |x| x[1][1] == \default };
		onsetTimes = s_new_messages.collect { |x| x.first };
		this.assertEquals(onsetTimes, [0.5, 1, 2], "onsets should be correctly derived from timingOffset and dur");
	}

	test_pattern_asScore_lag_multichannel {
		var events, score, s_new_messages, onsetTimes;
		events = [(x: 0, lag: [0.5, 1, 2])];
		score = Pseq(events).asScore(3);
		s_new_messages = score.score.drop(1).select { |x| x[1][1] == \default };
		onsetTimes = s_new_messages.collect { |x| x.first };
		this.assertEquals(onsetTimes, [0.5, 1, 2], "onsets should be correctly derived from lag in multichannel expanding");
	}

	test_pattern_asScore_timingOffset_multichannel {
		var events, score, s_new_messages, onsetTimes;
		events = [(x: 0, timingOffset: [0.5, 1, 2])];
		score = Pseq(events).asScore(3);
		s_new_messages = score.score.drop(1).select { |x| x[1][1] == \default };
		onsetTimes = s_new_messages.collect { |x| x.first };
		this.assertEquals(onsetTimes, [0.5, 1, 2], "onsets should be correctly derived from timingOffset in multichannel expanding");
	}

	test_pattern_asScore_timingValues_multichannel {
		var events, score, s_new_messages, onsetTimes;
		events = [(x: 0, timingOffset: [0.5, 1, 2], lag: [0.3, 0.7, 1])];
		score = Pseq(events).asScore(4);
		s_new_messages = score.score.drop(1).select { |x| x[1][1] == \default };
		onsetTimes = s_new_messages.collect { |x| x.first };
		this.assertEquals(onsetTimes, [0.5, 1, 2] + [0.3, 0.7, 1], "onsets should be correctly derived from lag and timingOffset in multichannel expanding");
	}

}

/*
TestScale.run
UnitTest.gui
*/

TestScale : UnitTest {

	*initClass {
		// way too many tests for this, use summaries instead
		reportPasses = false;
	}

	valueCompare { |a, b, within=0.0001|
		^a.isFloat.if { this.floatEquals(a, b, within) } { a == b }
	}

	floatEquals { |a, b, within=0.0001|
		^(a - b).abs < within
	}

	assertResultDictsEqual { |a, b, keys, fail_message, pass_message=""|
		var comparisons = keys.asArray.sort.collect { |key|
			[
				this.valueCompare(a[key], b[key]),
				[key, a[key], b[key]] .collect(_.asString).join(", ")
			]
		};
		var failures = comparisons.select { |r| r[0].not };
		failures.notEmpty.if {
			this.failed(currentMethod,
				fail_message ++ ":\n" ++ comparisons.collect(_.at(1)).join("\n"))
		} {
			this.passed(currentMethod, pass_message)
		}
		^failures.isEmpty
	}

	scalesEquivalent { |rawEvent, scaleEvent|
		var keys = [\note, \midinote, \freq, \octave, \gtranspose, \root]; //\freqToNote, \freqToScale
		var event_results = [rawEvent, scaleEvent].collect { |event|
			var result = IdentityDictionary[];
			event.make { |e| keys.do { |key| result.put(key, e.at(key).value) } };
			result;
		};
		var fail_message = "\nComparing:\ndegree %\nstepsPerOctave %\noctaveRatio %\nscale %\nScale %"
		.format(rawEvent.degree, rawEvent.stepsPerOctave, rawEvent.octaveRatio, rawEvent.scale, scaleEvent.scale);
		^this.assertResultDictsEqual(event_results[0], event_results[1], keys, fail_message);
	}

	test_event_integration {
		var result_values = List[];
		var degreeExamples = [0, 3, 19, -5, -37, 3s, 5b, 2s392, 8b104]
		++ ({ rrand(-24, 24) } ! 5)
		++ ({ rrand(-24.0, 24.0) } ! 5);
		var stepCounts = [12, 16, 7];
		var octaveRatios = [2.0, 1.9, 3.7];
		var octaves = (0..9);

		degreeExamples.do { |degree|
			stepCounts.do { |stepCount|
				octaveRatios.do { |octaveRatio|
					octaves.do { |octave|
						var scaleBase = (0..stepCount-1);
						var scales = [scaleBase] ++ ({
							scaleBase.scramble.keep((stepCount-3).rand + 3).sort
						} ! 2);
						scales.do { |scale|
							var scaleObj = Scale(scale, stepCount, Tuning(scaleBase/stepCount * 12, octaveRatio));
							var events = [scale, scaleObj].collect { |scaleThing|
								(degree: degree, scale: scaleThing,
									stepsPerOctave: stepCount, octaveRatio: octaveRatio,
									octave: octave
								).play
							};
							result_values.add(this.scalesEquivalent(*events));
						}
					}
				}
			}
		};
		Post << Char.nl << (currentMethod.asString + ":" + result_values.select(_.asBoolean).size.asString
			+ "passed," + result_values.select(_.not).size.asString + "failed.\n");
	}

}
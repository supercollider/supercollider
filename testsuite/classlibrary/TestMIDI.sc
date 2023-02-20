TestMIDI : UnitTest {
	test_linuxRealtimeCodesAreCorrect {
		var in, out;
		var find = { |array, str|
			array.detectIndex { |dev| dev.device.containsi(str) }
		};
		var inIndex;
		var currentTest;
		var cond = CondVar.new;
		var tested;

		if(thisProcess.platform.name == \linux) {
			if(MIDIClient.initialized.not) {
				MIDIClient.init;
			};
			// for macOS and Windows, 'in' would be in 'sources'
			// in Linux, you connect to a destination
			// check the MIDIOut help file, Linux section
			inIndex = find.(MIDIClient.destinations, "supercollider");
			if(inIndex.isNil) {
				this.assert(false, "SC MIDI input port not found");
				^this
			};
			out = MIDIOut(0).connect(inIndex).latency_(0);
			in = MIDIFunc.sysrt({ |data, srcID, index|
				this.assertEquals(index, currentTest.value,
					"Index for '%' sysrt message should match spec".format(currentTest.key)
				);
				tested = true;
				cond.signalAll;
			});
			[\start -> 10, \midiClock -> 8, \continue -> 11,
				\stop -> 12, \songPtr -> 2, \reset -> 15
			].do { |assn|
				tested = false;
				currentTest = assn;
				out.perform(assn.key, 0);
				cond.waitFor(0.5, { tested });
				if(tested.not) {
					this.assert(false, "% message was not received".format(assn.key));
				};
			};
			in.free;
			out.disconnect(inIndex);
		} {
			this.assert(true, "Realtime codes cannot be tested in macOS or Windows without external configuration, passing preemptively.");
		};
	}
}

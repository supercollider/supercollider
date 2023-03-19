TestIOUGens : UnitTest {
    var server;

	setUp {
		server = Server(this.class.name);
		server.bootSync;
	}

	tearDown {
		server.quit;
		server.remove;
	}

    //Test that InFeedback is equal to In when write / read are in the right order
    test_InFeedback_equals_In {
        var condition = Condition();
        var bus = Bus.audio(server);

        {
            var writer = Out.ar(bus.index, SinOsc.ar(1));
            var in = In.ar(bus.index);
            var inFeedback = InFeedback.ar(bus.index);
            inFeedback - in;
        }.loadToFloatArray(0.5, server, { | data |
            this.assertFloatEquals(data.sum, 0.0, "InFeedback should be equal to In when no other signal has been written to its bus", report:true);
            condition.unhang;
        });

        condition.hang;
    }

    //Test that AudioControl is equal to In when write / read are in the right order
    test_AudioControl_equals_In_for_read_after_write {
        var condition = Condition();
        var bus = Bus.audio(server);
        var sumBus = Bus.audio(server);

        var writer = {
            Out.ar(bus.index, SinOsc.ar(1));
            Silent.ar;
        };

        var in = {
            Out.ar(sumBus.index, In.ar(bus.index));
            Silent.ar;
        };

        var audioControl = {
            Out.ar(sumBus.index, -1 * (\in.ar)); //Invert to perform difference at sumBus
            Silent.ar;
        };

        var sum = {
            In.ar(sumBus.index);
        };

        server.bind({
            sum.loadToFloatArray(0.5, server, { | data |
                this.assertFloatEquals(data.sum, 0.0, "AudioControl should be equal to In for read after write", report:true);
                condition.unhang;
            });
			in.play(server);
            audioControl.play(server, args:[ \in, 'a' ++ bus.index ]);
			writer.play(server);
        });

        condition.hang;
    }

    //Test that InFeedback is equal to In + a delay of BlockSize when write/read are one cycle apart
    test_InFeedback_equals_In_for_write_after_read {
        var condition = Condition();
        var bus = Bus.audio(server);

        {
            var inFeedback = InFeedback.ar(bus.index);
            var writer = Out.ar(bus.index, SinOsc.ar(1));
            var blockSizeMs = BlockSize.ir / SampleRate.ir;
            var delayedIn = DelayN.ar(In.ar(bus.index), blockSizeMs, blockSizeMs);
            inFeedback - delayedIn;
        }.loadToFloatArray(0.5, server, { | data |
            this.assertFloatEquals(data.sum, 0.0, "InFeedback should be equal to In + a delay of BlockSize when write/read are one cycle apart", report:true);
            condition.unhang;
        });

        condition.hang;
    }

    //Test that AudioControl is equal to In + a delay of BlockSize when write/read are one cycle apart
    test_AudioControl_equals_In_for_write_after_read {
        var condition = Condition();
        var bus = Bus.audio(server);
        var sumBus = Bus.audio(server);

        var writer = {
            Out.ar(bus.index, SinOsc.ar(1));
            Silent.ar;
        };

        var delayedIn = {
            var blockSizeMs = BlockSize.ir / SampleRate.ir;
            Out.ar(sumBus.index, DelayN.ar(In.ar(bus.index), blockSizeMs, blockSizeMs));
            Silent.ar;
        };

        var audioControl = {
            Out.ar(sumBus.index, -1 * (\in.ar)); //Invert to perform difference at sumBus
            Silent.ar;
        };

        var sum = {
            In.ar(sumBus.index);
        };

        server.bind({
            sum.loadToFloatArray(0.5, server, { | data |
                this.assertFloatEquals(data.sum, 0.0, "AudioControl should be equal to In + a delay of BlockSize when write/read are one cycle apart", report:true);
                condition.unhang;
            });
			delayedIn.play(server);
			writer.play(server);
            audioControl.play(server, args:[ \in, 'a' ++ bus.index ]);
        });

        condition.hang;
    }
}

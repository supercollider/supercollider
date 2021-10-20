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
            this.assertFloatEquals(data.sum, 0.0, "InFeedback is not equal to In", report:true);
            condition.unhang;
        });

        condition.hang;
    }

    //Test that AudioControl is equal to In when write / read are in the right order
    test_AudioControl_equals_In {
        var condition = Condition();
        var bus = Bus.audio(server);
        var sumBus = Bus.audio(server);

        var writer = {
            Out.ar(bus.index, SinOsc.ar(1));
            Silence.ar;
        };

        var in = {
            Out.ar(sumBus.index, In.ar(bus.index));
            Silence.ar;
        };

        var audioControl = {
            Out.ar(sumBus.index, -1 * (\in.ar)); //Invert to perform difference at sumBus
            Silence.ar;
        };

        var sum = {
            In.ar(sumBus.index);
        };

        server.bind({
            sum.loadToFloatArray(0.5, server, { | data |
                this.assertFloatEquals(data.sum, 0.0, "AudioControl is not equal to In", report:true);
                condition.unhang;
            });
			in.play(server);
            audioControl.play(server, args:[ \in, 'a' ++ bus.index ]);
			writer.play(server);
        });

        condition.hang;
    }

    //Test that InFeedback is equal to In + a delay of BlockSize when write/read are one cycle apart
    test_InFeedback_equals_delayed_In {
        var condition = Condition();
        var bus = Bus.audio(server);

        {
            var inFeedback = InFeedback.ar(bus.index);
            var writer = Out.ar(bus.index, SinOsc.ar(1));
            var blockSizeMs = BlockSize.ir / SampleRate.ir;
            var delayedIn = DelayN.ar(In.ar(bus.index), blockSizeMs, blockSizeMs);
            inFeedback - delayedIn;
        }.loadToFloatArray(0.5, server, { | data |
            this.assertFloatEquals(data.sum, 0.0, "InFeedback is not equal to delayed In", report:true);
            condition.unhang;
        });

        condition.hang;
    }

    //Test that AudioControl is equal to In + a delay of BlockSize when write/read are one cycle apart
    test_AudioControl_equals_delayed_In {
        var condition = Condition();
        var bus = Bus.audio(server);
        var sumBus = Bus.audio(server);

        var writer = {
            Out.ar(bus.index, SinOsc.ar(1));
            Silence.ar;
        };

        var delayedIn = {
            var blockSizeMs = BlockSize.ir / SampleRate.ir;
            Out.ar(sumBus.index, DelayN.ar(In.ar(bus.index), blockSizeMs, blockSizeMs));
            Silence.ar;
        };

        var audioControl = {
            Out.ar(sumBus.index, -1 * (\in.ar)); //Invert to perform difference at sumBus
            Silence.ar;
        };

        var sum = {
            In.ar(sumBus.index);
        };

        server.bind({
            sum.loadToFloatArray(0.5, server, { | data |
                this.assertFloatEquals(data.sum, 0.0, "AudioControl is not equal to delayed In", report:true);
                condition.unhang;
            });
			delayedIn.play(server);
			writer.play(server);
            audioControl.play(server, args:[ \in, 'a' ++ bus.index ]);
        });

        condition.hang;
    }
}

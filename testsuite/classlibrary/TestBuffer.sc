TestBuffer : UnitTest {

	test_allocReadChannelMsg_missingChannelsAndCompletionMessage {
		var buffer, msg;
		buffer = Buffer.new;
		msg = buffer.allocReadChannelMsg("test_path", 0, -1);
		this.assertEquals(msg, ["/b_allocReadChannel", buffer.bufnum, "test_path", 0, -1]);
		buffer.free;
	}

	test_allocReadChannelMsg_emptyChannelsWithCompletionMessage {
		var buffer, msg;
		buffer = Buffer.new;
		msg = buffer.allocReadChannelMsg("test_path", 0, 25, [], ["test_message", 1, 2, 3]);
		this.assertEquals(msg, ["/b_allocReadChannel", buffer.bufnum, "test_path", 0, 25, ["test_message", 1, 2, 3]]);
		buffer.free;
	}

	test_allocReadChannelMsg_validChannelsNoCompletionMessage {
		var buffer, msg;
		buffer = Buffer.new;
		msg = buffer.allocReadChannelMsg("test_path", 0, -1, [7, 0, 1]);
		this.assertEquals(msg, ["/b_allocReadChannel", buffer.bufnum, "test_path", 0, -1, 7, 0, 1]);
		buffer.free;
	}

	test_allocReadChannelMsg_allArgumentsProvided {
		var buffer, msg;
		buffer = Buffer.new;
		msg = buffer.allocReadChannelMsg("test_path", 0, -1, [1, 0], ["complete", "xx"]);
		this.assertEquals(msg, ["/b_allocReadChannel", buffer.bufnum, "test_path", 0, -1, 1, 0, ["complete", "xx"]]);
		buffer.free;
	}

	test_bufferMsgMethodsConvertFloatToInt {
		// note, no messages will be sent and allocators aren't touched
		// so 'Server' is irrelevant
		var buf = Buffer(Server.default, 2048.0, 1.0, 0),
		copyBuf = Buffer(Server.default, 2048.0, 1.0, 1),
		checkMessage = { |msg, selector, indicesOfIntegers|
			this.assert(
				indicesOfIntegers.every { |index| msg[index].isInteger },
				"Buffer '%' method should convert floats to int at positions %, actual message = %"
				.format(selector, indicesOfIntegers, msg.asCompileString)
			);
		};

		checkMessage.(buf.allocMsg, \allocMsg, [2, 3]);
		checkMessage.(buf.allocReadMsg("a", 0.0, 100.0), \allocReadMsg, [3, 4]);
		checkMessage.(buf.allocReadChannelMsg("a", 0.0, 100.0, [0]), \allocReadChannelMsg, [3, 4]);
		checkMessage.(buf.readMsg("a", 0.0, -1.0, 0.0, 0), \readMsg, [3, 4, 5]);
		checkMessage.(buf.readChannelMsg("a", 0.0, -1.0, 0.0, 0, [0]), \readChannelMsg, [3, 4, 5]);
		checkMessage.(buf.cueSoundFileMsg("a", 0.0), \cueSoundFileMsg, [3, 4]);
		checkMessage.(buf.writeMsg("a", "aiff", "int16", -1.0, 0.0, 0), \writeMsg, [5, 6]);
		checkMessage.(buf.setMsg(0.0, 1), \setMsg, [2]);
		checkMessage.(buf.setnMsg(0.0, [1, 2]), \setnMsg, [2]);
		checkMessage.(buf.getMsg(0.0), \getMsg, [2]);
		checkMessage.(buf.getnMsg(0.0, 10.0), \getnMsg, [2, 3]);
		checkMessage.(buf.fillMsg(0.0, 10.0, 1.0), \fillMsg, [2, 3]);
		checkMessage.(buf.copyMsg(copyBuf, 0.0, 0.0, 10.0), \copyMsg, [3, 5, 6]);
	}

}

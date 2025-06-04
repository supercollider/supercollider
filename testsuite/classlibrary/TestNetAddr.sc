TestNetAddr : UnitTest {

	doReceiveTest {
		|port, testMsg, msgType, registerMethod, unregisterMethod, sendMethod|
		var messageReceived = CondVar();
		var messageValue = nil;
		var addr = NetAddr("127.0.0.1", port);
		var func = {
			|msg, time, replyAddr, recvPort|
			if (recvPort != port) {
				// filter out eventual late messages from server, until we have a way to wait for it to quit synchronously
				warn("TestNetAddr:doReceiveTest: skipping message received on unexpected port %: %".format(recvPort, msg));
			} {
				messageValue = [msg, time, replyAddr, recvPort];
				messageReceived.signalOne;
			}
		};
		var msg, time, replyAddr, recvPort;

		thisProcess.openUDPPort(port, msgType);
		thisProcess.perform(registerMethod, func);

		addr.perform(
			sendMethod,
			*testMsg.isArray.if({ testMsg }, { [testMsg] })
		);

		messageReceived.waitFor(1) { messageValue.notNil };

		thisProcess.perform(unregisterMethod, func);

		if (messageValue.isNil) {
			this.assert(false, "addr.% was never received".format(sendMethod));
		} {
			#msg, time, replyAddr, recvPort = messageValue;
			this.assertEquals(msg, testMsg, "Correct message received");
			this.assertFloatEquals(time, AppClock.seconds, "Timestamp is valid", within: 0.1);
			this.assertEquals(replyAddr.addr, addr.addr, "Hostname is valid");
			this.assertEquals(replyAddr.port, 57120, "Port is valid");
			this.assertEquals(recvPort, port, "recvPort is valid");
		}
	}

	test_receiveRaw {
		this.doReceiveTest(
			port:				54321,
			testMsg: 			"Test message. \n",
			msgType: 			\raw,
			registerMethod: 	\addRawRecvFunc,
			unregisterMethod: 	\removeRawRecvFunc,
			sendMethod: 		\sendRaw
		)
	}


	test_receiveOSC {
		this.doReceiveTest(
			port:				54322,
			testMsg: 			['/Test message. \n', 123], // floats will not pass - sclang's internal storage is `double` but is transmitted in OSC as `float`, so the transformation is not lossless.
			msgType: 			\osc,
			registerMethod: 	\addOSCRecvFunc,
			unregisterMethod: 	\removeOSCRecvFunc,
			sendMethod: 		\sendMsg
		)
	}
}

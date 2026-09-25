TestServer_notifyReply : UnitTest {

	notifyReply { |program, port, logins = 1|
		var ip = "127.0.0.1",
			addr,
			responder,
			reply,
			condvar = CondVar(),
			gotReply = false,
			attempts = 0;
		(program ++ ServerOptions().maxLogins_(logins).bindAddress_(ip).asOptionsString(port)).unixCmd;
		addr = NetAddr(ip, port);
		responder = OSCFunc({ |msg| reply = msg; gotReply = true; condvar.signalOne }, '/done', addr);
		// poll while the server boots: a /notify sent over UDP before the server
		// is listening is simply dropped, so resend until the /done reply arrives.
		// normally this takes about a second, the cap is a give-up for a server
		// that never comes up (25 * 0.2s = 5s).
		while ({ gotReply.not and: { attempts < 25 } }) {
			addr.sendMsg("/notify", 1);
			condvar.waitFor(0.2);
			attempts = attempts + 1;
		};
		responder.free;
		// resend /quit a few times: a single UDP packet can be dropped while the
		// server is still finishing its boot, which would leave it running.
		3.do { addr.sendMsg("/quit"); 0.05.wait };
		^ reply
	}

	test_notifyReply_includesMaxLogins {
		this.assertEquals(
			(this.notifyReply(Server.program, 57398) ?? { [] }).size,
			4,
			"scsynth /done /notify reply should include the client ID and max logins"
		);
		this.assertEquals(
			(this.notifyReply(Server.program.replace("scsynth", "supernova"), 57399) ?? { [] }).size,
			4,
			"supernova /done /notify reply should include the client ID and max logins"
		);
	}

	test_notifyReply_reportsConfiguredMaxLogins {
		this.assertEquals(
			(this.notifyReply(Server.program.replace("scsynth", "supernova"), 57397, 7) ?? { [] })[3],
			7,
			"supernova /done /notify should report the configured max logins"
		);
	}

}

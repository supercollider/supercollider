OSCService {
	var <name, <hostname, <port, <protocol;

// the way OSCServers should be created
	*knownServices {
		var services, numberOfKnownServices;
		numberOfKnownServices = this.numberOfKnownServices;
		services = Array.new(numberOfKnownServices);
		numberOfKnownServices.do({arg index;
			var service;
			service = OSCService.new;
			this.prInitOSCService(service, index);
			services.add(service);
		});

		^services;
	}

	*knownAddresses { arg applicationType="SuperCollider", protocol=\udp;
		var addr;
		this.knownServices.do { arg item;
			if(applicationType.isNil or: { item.name == applicationType }
			   and: { protocol.isNil or: { item.protocol == protocol } }, {
				addr = addr.add(item.netAddr);
			});
		};
		^addr
	}

// accessors
	*numberOfKnownServices {
		_NumOSCServices
	}

	netAddr {
		^NetAddr.new(hostname, port);
	}

// private methods
	*prInitOSCService { arg server, index;
		_InitOSCService
	}

	printOn { arg stream;
		stream << this.class.name << "(" <<* [name, hostname, port, protocol ? \unknown]  <<")"
	}
}

OSCService {
	var <name, <hostname, <port;

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
}

WaitForServerBoot {
	//extracted from cx
	*new { arg server, onComplete, limit=100;
			^Routine({
				
				if(server.serverRunning.not,{
					if (server.isLocal.not, { 
						"can't boot a remote server".inform; 
					}, {
						server.boot;
						while({
							server.serverRunning.not 
								and: {(limit = limit - 1) > 0}
						},{
							//can watch for Server.changed but we still need to time out
							"waiting for server to boot...".inform;
							0.2.wait;	
						});
					});
				});
				if(server.serverRunning.not,{
					"server failed to start".error;
				}, onComplete);
			}).play;
	}
}

WaitUntilServerBoot {
	//doesn't boot the server, just waits.
	*new { arg server, onComplete, limit=100;
			^Routine({
				
						while({
							server.serverRunning.not 
								and: {(limit = limit - 1) > 0}
						},{
							0.2.wait;	
						});
				
				if(server.serverRunning.not,{
					"server failed to start".error;
				}, onComplete);
			}).play;
	}
}


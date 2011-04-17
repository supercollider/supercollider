EventTypesWithCleanup {
	classvar <>cleanupTypes, <>ugenInputTypes, <>notNodeType;
	*initClass {
		Class.initClassTree(Event);
		Event.default[\eventTypes].putAll(
		(
		// ~bufnum has a default of 0, so we use ~bufNum instead....

			table: #{ | server |
			 	var bufNum;
 				if ( (bufNum = ~bufNum).notNil) {
					~schedBundle.value(~lag, ~timingOffset, server, [\b_setn, bufNum.asUGenInput, 0, ~amps.size] ++ ~amps)
				} {
					~type = \buffer;
					bufNum = ~bufNum = server.bufferAllocator.alloc;
					~numFrames = ~amps.size;
					~numChannels =  1;
					~schedBundle.value(~lag, ~timingOffset, server,
						[\b_alloc, bufNum, ~numFrames, ~numChannels,
						[\b_setn, bufNum.asUGenInput, 0, ~amps.size] ++ ~amps
					]);
				}
			},

			cheby: #{ | server |
			 	var bufNum;
 				if ( (bufNum = ~bufNum).notNil) {
					~schedBundle.value(~lag, ~timingOffset, server, [\b_gen, bufNum.asUGenInput, \cheby, ~genflags ? 7] ++ ~amps)
				} {
					~type = \buffer;
					bufNum = ~bufNum = server.bufferAllocator.alloc;
					~numFrames = ~numFrames ? 1024;
					~numChannels = ~numChannels ? 1;
					~schedBundle.value(~lag, ~timingOffset, server,
						[\b_alloc, bufNum, ~numFrames, ~numChannels,
						[\b_gen, bufNum, \cheby, ~genflags ? 7] ++ ~amps
					]);
				}
			},

			sine1: #{ | server |
			 	var bufNum;
 				if ( (bufNum = ~bufNum).notNil) {
					~schedBundle.value(~lag, ~timingOffset, server, [\b_gen, bufNum.asUGenInput, \sine1, ~genflags ? 7] ++ ~amps)
				} {
					~type = \buffer;
					bufNum = ~bufNum = server.bufferAllocator.alloc;
					~numFrames = ~numFrames ? 1024;
					~numChannels = ~numChannels ? 1;
					~schedBundle.value(~lag, ~timingOffset, server,
						[\b_alloc, bufNum, ~numFrames, ~numChannels,
						[\b_gen, bufNum, \sine1, ~genflags ? 7] ++ ~amps
					]);
				}
			},

			sine2: #{ | server |
			 	var bufNum,
			 		array = [~freqs, ~amps].lace(~freqs.size * 2);
 				if ( (bufNum = ~bufNum).notNil) {
					~schedBundle.value(~lag, ~timingOffset, server, [\b_gen, bufNum.asUGenInput, \sine2, ~genflags ? 7] ++ array)
				} {
					~type = \buffer;
					bufNum = ~bufNum = server.bufferAllocator.alloc;
					~numFrames = ~numFrames ? 1024;
					~numChannels = ~numChannels ? 1;
					~schedBundle.value(~lag, ~timingOffset, server,
						[\b_alloc, bufNum, ~numFrames, ~numChannels,
						[\b_gen, bufNum, \sine1, ~genflags ? 7] ++ array
					]);
				}
			},

			sine3: #{ | server |
			 	var bufNum,
			 		array = [~freqs, ~amps, ~phases].lace(~freqs.size * 3);
 				if ( (bufNum = ~bufNum).notNil) {
					~schedBundle.value(~lag, ~timingOffset, server, [\b_gen, bufNum.asUGenInput, \sine3, ~genflags ? 7] ++ array)
				} {
					~type = \buffer;
					bufNum = ~bufNum = server.bufferAllocator.alloc;
					~numFrames = ~numFrames ? 1024;
					~numChannels = ~numChannels ? 1;
					~schedBundle.value(~lag, ~timingOffset, server,
						[\b_alloc, bufNum, ~numFrames, ~numChannels,
						[\b_gen, bufNum, \sine1, ~genflags ? 7] ++ array
					]);
				}
			},

 			buffer: #{ | server |
				~bufNum = server.bufferAllocator.alloc(~numBufs ?? { ~numBufs =  1});
				~schedBundle.value(~lag, ~timingOffset, server, [\b_alloc, ~bufNum, ~numFrames, ~numChannels]);
			},

			freeBuffer: #{|server|
				~schedBundle.value(~lag, ~timingOffset, server, [\b_free, ~bufNum]);
				server.bufferAllocator.free(~bufNum);
			},

			allocRead: #{|server|
				var bufNum;
				if ( (bufNum = ~bufNum).isNil ) { bufNum = ~bufNum = server.bufferAllocator.alloc; ~type = \allocReadID };
				~schedBundle.value(~lag, ~timingOffset, server, [\b_allocRead, bufNum, ~path, ~firstFileFrame, ~numFrames]);
			},

			cue: #{ | server |
				var  bufNum, bndl, completion;
				if ( (bufNum = ~bufNum).isNil ) { bufNum = ~bufNum = server.bufferAllocator.alloc; ~type = \cueID };

				completion = ["/b_read", bufNum, ~path, ~firstFileFrame, ~bufferSize, ~firstBufferFrame, ~leaveOpen];
				bndl = ["/b_alloc", bufNum, ~bufferSize, ~numChannels, completion];
				~schedBundle.value(~lag, ~timingOffset, server, bndl);
			},

			freeAllocRead: #{|server|
				~schedBundle.value(~lag, ~timingOffset, server, [\b_free, ~bufNum]);
			},

			freeCue: #{ | server |
				var bufNum = ~bufNum;
				server.schedBundleArray.value(~lag, ~timingOffset, server, [["/b_close", bufNum], ["/b_free", bufNum ] ]  );
			},

			freeCueID: #{ | server |
				var bufNum = ~bufNum;
				server.schedBundleArray.value(~lag, ~timingOffset, server, [["/b_close", bufNum], ["/b_free", bufNum ] ]  );
				server.bufferAllocator.free(bufNum);
			},

			audioBus: #{ | server |
				~out = server.audioBusAllocator.alloc(~channels ? 1)
			},

			controlBus: #{ | server |
				~out = server.controlBusAllocator.alloc(~channels ? 1)
			},

			freeAudioBus: #{ | server |
				server.audioBusAllocator.free(~out)
			},

			freeControlBus: #{ | server |
				server.controlBusAllocator.free(~out)
			}
//,
//
//			group: #{|server|
//				var bundle;
//				if (~id.isNil) { ~id = server.nextNodeID };
//				bundle = [\g_new, ~id.asArray, Node.actionNumberFor(~addAction), ~group.asUGenInput].flop;
//				~schedBundleArray.value(~lag, ~timingOffset, server, bundle);
//			}


		) );

		notNodeType = (
			note:	false,
			on:		false,
			group:	false,
			tree:	false
		);

		cleanupTypes = (
			table:		\freeBuffer,			// free buffer and deallocate bufNum
			buffer:		\freeBuffer,			// free buffer and deallocate bufNum
			allocRead:	\freeAllocRead,		// free buffer
			cue:			\freeCue,				// free buffer, close file
			allocReadID:	\freeBuffer,			// free buffer and deallocate bufNum
			cueID:		\freeCueID,			// free buffer, close file, and deallocate bufNum
			audioBus:		\freeAudioBus, 		// deallocate bus
			controlBus:	\freeControlBus,		// deallocate bus


//			load:		\free,				// bufNum was allocated elsewhere, let the source clean it up
//			gen: 		\free,
//			read: 		\free,

			alloc: 		\free,
			on:			\off,
			group:		\kill,
			tree: 		\kill
		);


		ugenInputTypes = (
			buffer: 		\bufNum,
			allocRead: 	\bufNum,
			allocReadID: 	\bufNum,
			audioBus:		\out,
			controlBus:	\out,
			on:			\id,
			group:		\id,
			tree: 		\id
		);
	}

	*cleanup { | ev, flag = true |
		var type, notNode;
		type = ev[\type];
		notNode = notNodeType[type] ? true;
		if (flag || notNode) {
			 (	parent: ev,
				type: cleanupTypes[type]
			).play;
		}
	}

}

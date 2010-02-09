

ServerLogGui : ObjectGui {

	var nodeColors;

	guiBody { arg layout ... args;
		var w,events,tail=500;
		w = layout.bounds.width;
		nodeColors = Dictionary.new;

		events = args.first;

		events.do({ |ev|
			var eventTime,timeSent,delta,dir,bg,row;
			eventTime = ev.eventTime;
			if(ev.isKindOf(ServerLogSentEvent),{
				timeSent = ev.timeSent;
				delta = ev.delta;
				dir = ">>";
				bg = Color.green(0.5,0.3);
			},{
				timeSent = "";
				delta = "";
				dir = "<<";
				bg = Color.blue(0.5,0.3);
			});
			layout.startRow;
			layout.flow({ |r|
				// send/receive
				CXLabel(r,dir,30).background_(bg);
				// time
				CXLabel(r,eventTime,100);
				// sent
				CXLabel(r,timeSent,100);
				// delta
				CXLabel(r,delta,30);
				// msg
				if(ev.isBundle,{
					if(ev.msg.size == 1,{
						this.formatMsg(r,ev.msg[0]);
					},{
						//r.comp({ |c|
						//	c.flow({
								ev.msg.do({ |m,i|
									if(i>0,{
										r.startRow;
										CXLabel(r,"",272).background_(Color.clear);
									});
									this.formatMsg(r,m);
								})
							//})
						//}).resizeToFit;
					});
				},{
					this.formatMsg(r,ev.msg);
				});
			});
		});
	}

	formatMsg { |r,msg|
		var cmd,selector;
		cmd = ServerLog.cmdString(msg[0]);
		selector = "";
		cmd.do({ |char|
			if(char.isAlpha,{
				selector = selector ++ char;
			});
		});
		selector = ("cmd_"++selector).asSymbol;
		if(this.respondsTo(selector),{
			msg[0] = cmd;
			this.performList(selector,[msg,r]);
		},{
			cmd.gui(r).background_(this.colorForCmd(cmd));
			msg.do({ |m,i|
				if(i>0,{
					m.gui(r);
				})
			});
		});

		//cmd.gui(r).background_(this.colorForCmd(cmd));
		/*
		color code:
			fail
			create
			delete
			affirm : synced
		color code each node by rand color
		translate addActions

		group,synth colors would be nice

		*/
	}
	colorForCmd { |cmd|
		^cmd.switch(
			"/n_free",{ Color.red},
			"/g_freeAll",{Color.red},
			"/g_deepFree",{Color.red},

			"/n_run",{Color.yellow},
			"/n_map",{Color.yellow},
			"/n_mapn",{Color.yellow},
			"/n_set",{Color.yellow},
			"/n_setn",{Color.yellow},
			"/n_fill",{Color.yellow},

			"/n_trace",{Color.white},
			"/n_query",{Color.white},
			"/s_get",{Color.white},
			"/s_getn",{Color.white},

			"/s_new",{Color.green},
			"/n_before",{Color.green},
			"/n_after",{Color.green},
			"/g_new",{Color.green},
			"/g_head",{Color.green},
			"/g_tail",{Color.green}
		)
	}
	addAction { |num|
		^num.switch(
			0, {"addToHead:"},
			1, {"addToTail:"},
			2, {"addBefore:"},
			3, {"addAfter:"},
			4, {"addReplace:"},
			num// more to come
		)
	}

	coloredCmd { |cmd,color,r|
		cmd.asString.gui(r).background_(color);
	}
	coloredDef { |nodeID,r|
		var c;
		c = nodeColors[nodeID];
		if(c.isNil,{
			c = Color.rand;
			nodeColors[nodeID] = c;
		});
		nodeID.gui(r).background_(c);
	}
	coloredNode { |nodeID,r|
		var c,annotation;
		if(nodeID.isNil,{ // this is an error
			^nil.gui(r).background_(Color.red);
		});
		c = nodeColors[nodeID];
		if(c.isNil,{
			//c = Color.rand;
			c =  Color.new255(*((nodeID + 1) * 911640000).asDigits(256,3)).alpha_(0.5);
			nodeColors[nodeID] = c;
		});
		annotation = Library.at(AbstractPlayer, \nodeAnnotations, Server.default, nodeID);
		if(annotation.notNil,{
			(nodeID.asString+annotation).gui(r).background_(c);
		},{
			nodeID.gui(r).background_(c);
		});
	}
	guiArgs { |args,r|
		args.do({ |a|
			if(a.isNil,{
				a.gui(r).background_(Color.red)
			},{
				a.gui(r)
			})
		})
	}
	cmd_nfree { |msg,r|
		this.coloredCmd(msg[0],Color.yellow(alpha:0.5),r);
		this.coloredNode(msg[1],r);
	}
	cmd_gnew { |msg,r|
		this.coloredCmd(msg[0],Color.green(alpha:0.5),r);
		this.coloredNode(msg[1],r);
		// addAction
		this.addAction(msg[2]).gui(r);
		if(msg[1] != 1,{ // root node has a nil parent, nothing to get red about
			this.coloredNode(msg[3],r);
		},{
			msg[3].gui(r);
		});
	}
	cmd_gfreeAll { |msg,r|
		this.coloredCmd(msg[0],Color.yellow(alpha:0.5),r);
		msg[1].gui(r);
	}
	cmd_snew { |msg,r|
		//9, defName, synth.nodeID, addNum, inTarget.nodeID
		this.coloredCmd(msg[0],Color.green(alpha:0.5),r); // s_new
		this.coloredDef(msg[1],r); // defname
		this.coloredNode(msg[2],r); // nodeID
		this.addAction(msg[3]).gui(r); // addAction
		this.coloredNode(msg[4],r); // group
		this.guiArgs(msg.copyToEnd(5),r);// args
	}
	cmd_drecv { |msg,r|
		var defName;
		this.coloredCmd(msg[0],Color.green(alpha:0.5),r);
		defName = SynthDesc.defNameFromBytes(msg[1]);
		this.coloredDef( defName, r);
	}
	cmd_fail { |msg,r|
		this.coloredCmd(msg[0],Color.red,r);
		msg.copyToEnd(1).do({ |m| // args
			m.gui(r);
		});
	}
	cmd_ngo { |msg,r|
		this.coloredCmd(msg[0],Color.green(alpha:0.5),r);
		this.coloredNode(msg[1],r); // nodeID
		msg.copyToEnd(2).do({ |m| // args
			m.gui(r);
		});
	}
	cmd_nend { |msg,r|
		this.coloredCmd(msg[0],Color.yellow(alpha:0.5),r);
		this.coloredNode(msg[1],r); // nodeID
		this.guiArgs(msg.copyToEnd(2),r); // args
	}
	cmd_statusreply { |msg,r|
		var cmd, one, numUGens, numSynths, numGroups, numSynthDefs,
					avgCPU, peakCPU, sampleRate, actualSampleRate;
		 #cmd, one, numUGens, numSynths, numGroups, numSynthDefs,
					avgCPU, peakCPU, sampleRate, actualSampleRate = msg;

			("/status.reply % ugens % synths % groups % synthDefs".format(numUGens,numSynths,numGroups,numSynthDefs)).gui(r);
	}

}



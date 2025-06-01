NodeTreeView {
	var <server, <bounds, <window, view, resp, updateFunc, updater, tabSize = 25, treeViewStatus;
	var runningFunc, stoppedFunc, serverController;
	var globalAlpha = 1;

	*new { |server, bounds, parent|
		server  = server ?? { Server.default };
		^super.newCopyArgs(server).makeWindow(bounds, parent)
	}

	makeWindow { |bounds, parent|
		bounds = if(bounds.isNil) {
			Rect(128, 64, 400, 400)
		} {
			bounds.asRect.minSize(395@386)
		};
		if(parent.isNil) {
			window = Window(this.asString, bounds, scroll:true);
		} {
			window = parent
		}
		.front;

		view = UserView(window, window.view.bounds.resizeBy(-4));
		view.onClose_{
			view = nil;
			this.stop;
		};

		treeViewStatus = StaticText(
			view,
			Rect(tabSize, 0, view.bounds.width - (tabSize * 2) + 1, tabSize);
		).align_(\center).stringColor_(Color.grey(0.4));

	}

	start { |interval = 0.5, actionIfFail|
		var done = false;
		var collectChildren, levels, countSize;
		var pen, font;
		var drawFunc;

		view ?? {
			"The NodeTreeView instance was closed".warn;
			^this
		};

		updater !? { updater.isPlaying.if{ this.stop } };

		pen = GUI.current.pen;
		font = Font.sansSerif(10);

		view.drawFunc = {
			var xtabs = 0, ytabs = 0, drawFunc;

			drawFunc = {|group|
				var thisSize, rect, endYTabs;
				xtabs = xtabs + 1;
				ytabs = ytabs + 1;
				pen.font = font;
				group.do({|node, index|
					if(node.value.isArray, {
						thisSize = countSize.value(node);
						endYTabs = ytabs + thisSize + 0.2;
						rect = Rect(xtabs * tabSize + 0.5,
							ytabs * tabSize + 0.5,
							view.bounds.width - (xtabs * tabSize * 2),
							thisSize * tabSize;
						);
						pen.fillColor = Color.grey(0.8, globalAlpha);
						pen.fillRect(rect);
						pen.strokeRect(rect);
						pen.color = Color.grey(0, globalAlpha);
						pen.stringInRect(
							" Group" + node.key.asString +
							(node.key == 1).if("- default group", ""),
							rect
						);
						drawFunc.value(node.value);
						ytabs = endYTabs;
					},{
						rect = Rect(xtabs * tabSize + 0.5,
							ytabs * tabSize + 0.5,
							7 * tabSize,
							0.8 * tabSize
						);
						pen.fillColor = Color.white;
						pen.fillRect(rect);
						pen.strokeRect(rect);
						pen.color = Color.grey(0, globalAlpha);
						pen.stringInRect(
							" " ++ node.key.asString + node.value.asString,
							rect
						);
						ytabs = ytabs + 1;
					});
				});
				xtabs = xtabs - 1;
			};
			drawFunc.value(levels);
		};

		// msg[1] controls included
		// msg[2] nodeID of queried group
		// initial number of children
		resp = OSCFunc({ arg msg;
			var finalEvent;
			var i = 2, j, controls, printControls = false, dumpFunc;
			if(msg[1] != 0, {printControls = true});
			dumpFunc = {|numChildren|
				var event, children;
				event = ().group;
				event.id = msg[i];
				event.instrument = nil; // need to know it's a group
				i = i + 2;
				children = Array.fill(numChildren, {
					var id, child;
					// i = id
					// i + 1 = numChildren
					// i + 2 = def (if synth)
					id = msg[i];
					if(msg[i+1] >=0, {
						child = dumpFunc.value(msg[i+1]);
					}, {
						j = 4;
						child = ().synth.instrument_(msg[i+2]);
						if(printControls, {
							controls = ();
							msg[i+3].do({
								controls[msg[i + j]] = msg[i + j + 1];
								j = j + 2;
							});
							child.controls = controls;
							i = i + 4 + (2 * controls.size);
						}, {i = i + 3 });
					});
					child.id = id;
				});
				event.children = children;
				event;
			};
			finalEvent = dumpFunc.value(msg[3]);
			done = true;
			collectChildren = {|group|
				group.children.collect({|child|
					if(child.children.notNil,{
						child.id -> collectChildren.value(child);
					}, {
						child.id -> child.instrument;
					});
				});
			};
			levels = collectChildren.value(finalEvent);
			countSize = {|array|
				var size = 0;

				array.do({|elem|
					var sizeFactor = if (elem.key < 1000) {
						(elem.value.size + 1).expexp(1, 501, 1, 1.006)
					} {
						array.size.linlin(0, 100, 1.03, 1.032)
					};
					if (elem.value.isArray) {
						size = size + (countSize.value(elem.value) * sizeFactor) + 2.5
					} {
						size = size + 1
				};
				});
				size
			};
			defer {
				var height;
				view !? {
					window !? {
						height = if(view.bounds.isNil) { 400 } { view.bounds.height };
						view.bounds = Rect(
							0,
							0,
							min(window.bounds.width - 4, 394),
							max(height, tabSize * countSize.value(levels) + (tabSize * 2.3)));
						view.refresh
					}
				}
			}
		}, '/g_queryTree.reply', this.server.addr).fix;

		updateFunc = {
			updater = fork {
				loop {
					this.server.sendMsg("/g_queryTree", 0, 0);
					interval.wait;
				}
			}
		};

		SystemClock.sched(3, {
			if(done.not, {
				actionIfFail.value();
				// "Server failed to respond to Group:queryTree!".warn;
			});
		});

		runningFunc = {
			globalAlpha = 1;
			updateFunc.value;
			CmdPeriod.add(updateFunc);
			defer {
				treeViewStatus.string_("");
			}
		};
		stoppedFunc = { | serverStopped = false |
			updater.stop;
			CmdPeriod.remove(updateFunc);
			defer {
				var msg;
				view !? {
					msg = serverStopped.if{ "Server not running." } { "View stopped." };
					((levels.size ? 0) > 0).if({ msg = msg ++ " Last node tree:"});
					treeViewStatus.string_(msg);
					globalAlpha = 0.6;
					view.refresh;
				}
			}
		};

		serverController = SimpleController(server)
		.put(\serverRunning, { if(server.serverRunning, runningFunc, { stoppedFunc.(true) }) });

		if(server.serverRunning, runningFunc, { stoppedFunc.(true) }); // start here
	}

	stop {
		serverController.remove;
		stoppedFunc.(false);
		resp.free;
	}

	close {
		window.close
	}

	front {
		window.front
	}
}

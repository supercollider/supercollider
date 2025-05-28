ServerTreeView {
	var <server, <bounds, <window, <>isUpdating, isUpdatingText, isNotUpdatingText, viewParent, resp, updateFunc, updater, tabSize = 25, treeViewStatus;

	*new { |server, bounds, parent|
		server  = server ?? { Server.default };
		^super.newCopyArgs(server).makeWindow(bounds, parent)
	}

	makeWindow { |bounds, parent|
		var view;
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

		window.onClose_{
			updater.stop;
			CmdPeriod.remove(updateFunc);
			resp.free
		};

		view = UserView(window, window.view.bounds);
		viewParent = view;
		treeViewStatus = StaticText(
			viewParent,
			Rect(tabSize, 0, viewParent.bounds.width - (tabSize * 2) + 1, tabSize)
		);

		treeViewStatus
		.string_(" STOPPED")
		.font_(Font.sansSerif(14).boldVariant);

		isUpdatingText = {
			treeViewStatus.string_(" RUNNING: current server tree")
			.background_(Color.grey(0.9))
		};
		isNotUpdatingText = {
			treeViewStatus.string_(" STOPPED: last updated server tree")
											.background_(Color.grey(0.7))
		};
		isUpdating = false;

		// The following three lines are suggested by @dyfer, but it is contrary to the design by @telephon.
		// If this part is added, then the lin
		// So I did not add this part, but I left it as comment.
		/*
		if(server.serverRunning) {
		this.start;
		};
		*/
	}

	start { |interval = 0.5, actionIfFail|
		var done = false;
		var collectChildren, levels, countSize;
		var view, bounds;
		var pen, font;
		var drawFunc, lastGroupRect, lastSynthRect, lastRect, lastNodeKey, lastNodeSize;

		pen = GUI.current.pen;
		font = Font.sansSerif(10);

		drawFunc = { |group, xtabs, ytabs|
			var thisSize, rect, endYTabs, verticalStretchFactor = 1, defaultGroupRect;
			xtabs = xtabs + 1;
			ytabs = ytabs + 1;
			pen.font = font;
			group.do({ |node, index|
				if(node.value.isArray, {
					thisSize = countSize.value(node);
					verticalStretchFactor = thisSize.expexp(1, 1000, 1, 1.04);
					endYTabs = ytabs + thisSize + 0.2;
					rect = Rect(xtabs * tabSize + 0.5,
						if (
							lastRect.notNil && (node.key != 1) && (lastNodeKey != 1)
						) {
							if (index == 0) {
								ytabs * tabSize * verticalStretchFactor
							} {
								lastRect.bottom + 5
							}
						}
						{
							ytabs * tabSize * verticalStretchFactor
						} + 0.5,
						viewParent.bounds.width - (xtabs * tabSize * 2),
						thisSize * tabSize * verticalStretchFactor
					);
					pen.fillColor = Color.grey(0.8);
					pen.fillRect(rect);
					pen.strokeRect(rect);
					pen.color = Color.black;
					pen.stringInRect(
						" Group" + node.key.asString +
						if(node.key == 1) {
							defaultGroupRect = rect;
							"- default group"
						} {
							""
						},
						rect
					);
					drawFunc.value(node.value, xtabs, ytabs);
					ytabs = endYTabs;
					lastNodeSize = node.value.size;
					lastGroupRect = rect;
				},{
					rect = Rect(xtabs * tabSize + 0.5,
						/*
							ytabs * tabSize * (
							if ("localhost|(S|s)cope".matchRegexp(node.asString)) {
							verticalStretchFactor.debug(node.key).linlin(1, 1.05, 1.01, 1.03)
							} {
							1
							}
							) + 0.5
							*/
						if(defaultGroupRect.notNil) {
							if ("localhost|stethoscope|system_freqScope".matchRegexp(node.asString)) {
								if (group.asString.contains("localhost")) {
									tabSize * (index - 2) + defaultGroupRect.bottom + 5
								} {
									tabSize * (index - 1) + defaultGroupRect.bottom + 5
								}
							} { tabSize * index + defaultGroupRect.bottom }
						} {
							ytabs * tabSize * verticalStretchFactor
						}
						+ 0.5,
						7 * tabSize,
						0.8 * tabSize
					);
					pen.fillColor = Color.white;
					pen.fillRect(rect);
					pen.strokeRect(rect);
					pen.color = Color.black;
					pen.stringInRect(
						" " ++ node.key.asString + node.value.asString,
						rect
					);
					ytabs = ytabs + 1;
					lastSynthRect = rect
				});
				lastRect = rect;
				lastNodeKey = node.key
			});
			xtabs = xtabs - 1;
		};
		viewParent.drawFunc = {

			var xtabs = 0, ytabs = 0;
			drawFunc.value(levels, xtabs, ytabs)
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
					if(elem.value.isArray, { size = size + countSize.value(elem.value) + 2}, {size = size + 1;});
				});
				size
			};
			defer {
				var height = if(viewParent.bounds.isNil) {
					400
					} {
					viewParent.bounds.height
					};
				viewParent.bounds = Rect(0, 0, 400, max(height, tabSize * countSize.value(levels) * 1.06));
				viewParent.refresh
			}
		}, '/g_queryTree.reply', this.server.addr).fix;

		isUpdating = true;

		if(window.isClosed.not) {
			if(updater.isPlaying.not) {
				updateFunc = {
					if(updater.notNil) {
						updater.stop;
						updater = nil
					};
					updater = (
						fork {
							loop {
								defer {
									if(server.serverRunning && window.isClosed.not && this.isUpdating){
										isUpdatingText.()
									} {
										if (done) {
											isNotUpdatingText.()
										}
									}
								};
								this.server.sendMsg("/g_queryTree", 0, 0);
								interval.wait
							}
						}
					)
				};
				updateFunc.();

				CmdPeriod.add(updateFunc);
				SystemClock.sched(3, {
					if(done.not && (isUpdating == true), {
						actionIfFail.value();
						"Server failed to respond to Group:queryTree!".warn;
					});
				});
				isUpdating = true
			} {
				"A serverTreeView is already running, so your request will not be executed.\n".warn
			}
		} {
			"The window containing the ServerTreeView instance is closed.\nCreate one to plot server-side node tree.".warn
		}
	}

	stop {
		if(window.isClosed.not) {
			isNotUpdatingText.();
			isUpdating = false;
			updater.stop;
			resp.free;
			resp = nil
		};
		CmdPeriod.remove(updateFunc);
	}

	close {
		this.stop;
		window.close
	}

	front {
		window.front.debug
	}
}

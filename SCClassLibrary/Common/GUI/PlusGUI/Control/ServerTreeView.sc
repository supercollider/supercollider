ServerTreeView {
	var <server, <bounds, viewParent, <window, resp, updateFunc, updater, tabSize = 25, treeViewStatus;

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
		view = UserView(window, window.view.bounds);
		viewParent = view;
		//if(window.view.respondsTo(\hasHorizontalScroller_)) { window.view.hasHorizontalScroller_(false) };
		treeViewStatus = StaticText(window, Rect(tabSize, 0, window.bounds.width - (tabSize * 2) + 1, tabSize));
		treeViewStatus
		.string_(" STOPPED")
		.font_(Font.sansSerif(14 ).boldVariant)
		.onClose_{ updateFunc.stop };
		window.front
	}

	start { |interval = 0.5, actionIfFail|
		var done = false;
		var collectChildren, levels, countSize;
		var view, bounds;
		var pen, font;
		var drawFunc;

		treeViewStatus.string_(" RUNNING: current server tree")
		.background_(Color.grey(0.9));

		pen = GUI.current.pen;
		font = Font.sansSerif(10);

		drawFunc = { |group, xtabs, ytabs|
			var thisSize, rect, endYTabs;
			xtabs = xtabs + 1;
			ytabs = ytabs + 1;
			pen.font = font;
			group.do({|node|
				if(node.value.isArray, {
					thisSize = countSize.value(node);
					endYTabs = ytabs + thisSize + 0.2;
					rect = Rect(xtabs * tabSize + 0.5,
						ytabs * tabSize + 0.5,
						viewParent.bounds.width - (xtabs * tabSize * 2),
						thisSize * tabSize;
					);
					pen.fillColor = Color.grey(0.8);
					pen.fillRect(rect);
					pen.strokeRect(rect);
					pen.color = Color.black;
					pen.stringInRect(
						" Group" + node.key.asString +
						(node.key == 1).if("- default group", ""),
						rect
					);
					drawFunc.value(node.value, xtabs, ytabs);
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
					pen.color = Color.black;
					pen.stringInRect(
						" " ++ node.key.asString + node.value.asString,
						rect
					);
					ytabs = ytabs + 1;
				});
			});
			xtabs = xtabs - 1;
		};
		viewParent.drawFunc = {
			var xtabs = 0, ytabs = 0;
			drawFunc.value(levels, xtabs, ytabs);
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
				viewParent.bounds = Rect(0, 0, 400, max(height, tabSize * (countSize.value(levels) + 2)));
				viewParent.refresh;
			}
		}, '/g_queryTree.reply', this.server.addr).fix;

		updater = {
			fork {
				loop {
					defer {
						if (this.server.serverRunning.not) {
							if(window.isClosed.not) {
								treeViewStatus.string_(" STOPPED: last updated server tree")
								.background_(Color.grey(0.7));
								/*this.server.doWhenBooted{
									treeViewStatus.string_(" RUNNING: current server tree")
									.background_(Color.grey(0.9))
								}*/
							}
						}
					};
					this.server.sendMsg("/g_queryTree", 0, 0);
					interval.wait;
				};
			}
		};
		updateFunc = updater.value;
		CmdPeriod.add(updater);
		SystemClock.sched(3, {
			if(done.not, {
				actionIfFail.value();
				"Server failed to respond to Group:queryTree!".warn;
			});
		});
	}

	stop {
		treeViewStatus.string_(" STOPPED: last updated server tree")
		.background_(Color.grey(0.7));
		updateFunc.stop;
		CmdPeriod.remove(updater);
		resp.free;
	}

	close {
		this.stop;
		window.close
	}
}

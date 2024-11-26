NodeTreeView : View {

	var server;
	var <>displayControls = true;
	var updateFunc, updater, <>refreshRate = 0.3;
	var scroll, canvas;
	var nodeTree, drawCommands, viewHeight, penPos;
	var font, fontSize = 10, fontOverflow = 1.2, margins = 10;
	var xIndex;
	var string, truncValue = 0.001;

	*new { |server|
		^super.new.init(server)
	}
	
	init { |parentServer|
		server = parentServer;
		font = Font.sansSerif(fontSize);
		fontOverflow = (fontSize * fontOverflow).asInteger;
		scroll = ScrollView();
		canvas = UserView();
		this.layout_(
			VLayout()
			.margins_(0)
			.spacing_(0)
			.add(scroll);
		);
		scroll.canvas_(canvas);
		canvas.drawFunc_({ |view|
			Pen.strokeColor_(Color.black);
			nodeTree = server.getNodeTree;
			if(drawCommands.isNil) {
				Pen.fillColor_(Color.grey(0.75));
				Pen.addRect(
					Rect(
						margins,
						margins,
						view.bounds.width - (margins * 2),
						view.bounds.height - (margins * 2)
					)
				);
				Pen.fillStroke;
				Pen.stringCenteredIn(
					"Server not running",
					Rect(
						0,
						0,
						this.bounds.width,
						this.bounds.height
					),
					font,
					Color.black
				);
			} {
				penPos = margins;
				drawCommands.do({ |command|
					this.prDrawCommand(command, view);
				});
			};
		});

		updateFunc = {
			updater = fork {
				loop {
					nodeTree = server.getNodeTree;
					if(nodeTree.isNil) {
						viewHeight = nil;
						drawCommands = nil;
					} {
						this.prConstructDrawCommands;
					};
					{
						if(viewHeight.isNil) {
							viewHeight = this.bounds.height - 2;
						};
						canvas.fixedHeight_(viewHeight);
						canvas.refresh;
					}.defer;
					refreshRate.wait;
				}
			}
		};
		updateFunc.value;

		this.onClose_({
			updater.stop;
			CmdPeriod.remove(updateFunc);
		});

		this.onResize_({
			if(drawCommands.isNil) {
				canvas.fixedHeight_(this.bounds.height - 2);
			};
		});
	}

	prNodeAsDrawCommand { |node, parent, xIndex|
		var command = ();
		command[\type] = node[\type];
		command[\id] = node[\id];
		command[\xIndex] = xIndex;
		command[\height] = (margins * 2) + fontOverflow;
		xIndex = xIndex + 1;
		if(node[\type] == \group) {
			var prevParent = parent;
			parent = command;
			command[\children] = List(0);
			node[\children].do({ |node|
				this.prNodeAsDrawCommand(node, parent, xIndex);
			});
			if(prevParent.notNil) {
				prevParent[\height] = prevParent[\height] + command[\height] + margins;
				prevParent[\children].add(command);
			} {
				drawCommands.add(command);
			};
		} {
			command[\name] = node[\name];
			command[\controls] = List(0);
			if(displayControls and: { node[\controls].size > 0 }) {
				command[\height] = command[\height] + margins;
				node[\controls].do({ |pair|
					command[\controls].add(pair);
					command[\height] = command[\height] + fontOverflow;
				});
			};
			if(parent.notNil) {
				parent[\height] = parent[\height] + command[\height] + margins;
				parent[\children].add(command);
			} {
				drawCommands.add(command);
			};
		};
	}

	prConstructDrawCommands {
		if(drawCommands.notNil) {
			drawCommands.do({ |command|
				this.prClearDrawCommand(command);
			});
		};
		drawCommands = List(0);
		xIndex = 1;
		nodeTree.do({ |node|
			this.prNodeAsDrawCommand(node, nil, xIndex);
		});
		viewHeight = 0;
		drawCommands.do({ |command| viewHeight = viewHeight + command[\height]; });
		viewHeight = viewHeight + ((drawCommands.size + 1) * margins);
	}

	prClearDrawCommand { |command|
		if(command[\type] == \group) {
			command[\children].do({ |child| this.prClearDrawCommand(child); });
			command[\children].clear;
		} {
			command[\controls].clear;
		};
		command.clear;
	}

	prDrawCommand { |command, view|
		if(command[\type] == \group) {
			Pen.fillColor_(
				Color.grey(
					min(
						(0.5 * (1.15.pow(command[\xIndex]))),
						0.9
					)
				)
			);
			Pen.addRect(
				Rect(
					margins * (command[\xIndex]),
					penPos,
					view.bounds.width - (margins * command[\xIndex] * 2),
					command[\height]
				)
			);
			Pen.fillStroke;
			Pen.stringInRect(
				"Group" + command[\id].asString +
				(command[\id] == 1).if("- default group", ""),
				Rect(
					margins * (command[\xIndex] + 1),
					penPos + margins,
					view.bounds.width - (margins * (command[\xIndex] + 1) * 2),
					fontOverflow
				),
				font,
				Color.black
			);
			penPos = penPos + (margins * 2) + fontOverflow;
			command[\children].do({ |child|
				this.prDrawCommand(child, view);
			});
			penPos = penPos + margins;
		} {
			Pen.fillColor_(Color.white);
			Pen.addRect(
				Rect(
					margins * (command[\xIndex]),
					penPos,
					view.bounds.width - (margins * command[\xIndex] * 2),
					command[\height])
			);
			Pen.fillStroke;
			Pen.stringCenteredIn(
				(command[\id].asString ++ " " ++ command[\name]),
				Rect(
					margins * (command[\xIndex]),
					penPos + margins,
					view.bounds.width - (margins * command[\xIndex] * 2),
					fontOverflow
				),
				font,
				Color.black
			);
			penPos = penPos + margins + fontOverflow;
			if(command[\controls].size > 0) {
				penPos = penPos + margins;
				command[\controls].do({ |pair|
					string = pair[1].trunc(truncValue).asString;
					if(string.endsWith(".0")) {
						string = string.replace(".0", "");
					};
					Pen.stringRightJustIn(
						(pair[0].asString ++ ": "),
						Rect(
							0,
							penPos,
							view.bounds.width / 2,
							fontOverflow
						),
						font,
						Color.black
					);
					Pen.stringLeftJustIn(
						string,
						Rect(
							view.bounds.width / 2,
							penPos,
							view.bounds.width / 2,
							fontOverflow
						),
						font,
						Color.black
					);
					penPos = penPos + fontOverflow;
				});
			};
			penPos = penPos + (margins * 2);
		};
	}
}
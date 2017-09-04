AbstractMenuAction : QObject {
	var <>action;

	initConnections {
		this.connectMethod('changed()', 'onChanged');
		this.connectMethod('triggered(bool)', 'onTriggered');
		this.connectMethod('hovered()', 'onHovered');
		this.connectMethod('toggled(bool)', 'onToggled');

		this.menuRole = \noRole;
	}

	onChanged 	{ 		this.changed(\changed) }
	onTriggered	{ |b| 	this.changed(\triggered, b); action.value(this, b); }
	onHovered 	{ 		this.changed(\hovered) }
	onToggled 	{ |b|	this.changed(\toggled, b) }

	menuRole 	{ 		^this.getProperty(\menuRole) }
	menuRole_ 	{ |b| 	^this.setProperty(\menuRole, QMenuRole(b)) }

	shortcut 	{ 		^this.getProperty(\shortcut) }
	shortcut_ 	{ |str| ^this.setProperty(\shortcut, str) }

	string 		{ 		^this.getProperty(\text) }
	string_	 	{ |str| ^this.setProperty(\text, str) }

	menu 		{ 		^this.getProperty(\menu) }
	menu_ 		{ |b| 	^this.setProperty(\menu, b) }

	asAction { ^this }
}

MenuAction : AbstractMenuAction {
	*qtClass { ^'QcAction' }

	*new {
		|string, function|
		^super.new.init().string_(string).action_(function);
	}

	*separator {
		|string|
		^MenuAction().separator_(true).string_(string)
	}

	init {
		this.initConnections();
	}

	checkable 	{ 		^this.getProperty(\checkable) }
	checkable_ 	{ |b| 	^this.setProperty(\checkable, b) }

	checked 	{ 		^this.getProperty(\checked) }
	checked_ 	{ |b| 	this.checkable = true; ^this.setProperty(\checked, b) }

	toolTip 	{ 		^this.getProperty(\toolTip) }
	toolTip_ 	{ |str| ^this.setProperty(\toolTip, str) }

	statusTip 	{ 		^this.getProperty(\statusTip) }
	statusTip_ 	{ |str| ^this.setProperty(\statusTip, str) }

	font 		{ 		^this.getProperty(\font) }
	font_ 		{ |font| ^this.setProperty(\font, font) }

	separator 	{ 		^this.getProperty(\separator) }
	separator_	{ |b| 	^this.setProperty(\separator, b) }

	icon_ 	{
		|icon|
		this.iconVisible = icon.notNil;
		^this.setProperty(\icon, icon)
	}

	iconVisible	{ |b| 	^this.getProperty(\iconVisibleInMenu) }
	iconVisible_{ |b| 	^this.setProperty(\iconVisibleInMenu, b) }

	asMenu {
		this.menu = Menu().title_(this.string);
		^this.menu;
	}

	printOn { arg stream; stream << this.class.name << "(\"" << (this.string ? "-")  << "\")" }
}

CustomViewAction : AbstractMenuAction {
	*new {
		|view, function|
		^super.new.init().defaultView_(view).action_(function)
	}

	*qtClass { ^'QcWidgetAction' }

	init {
		this.initConnections
	}

	defaultView { 		^this.getProperty(\defaultWidget) }
	defaultView_{ |b| 	^this.setProperty(\defaultWidget, b) }
}

MainMenu {
	classvar <otherMenus;
	classvar <applicationMenu;
	classvar <registered;

	*initClass {
		var serversMenu = Menu(MenuAction.separator).title_("Servers");
		serversMenu.addDependant({
			|menu, what|
			if (what == \aboutToShow) {
				menu.clear();
				Server.all.do {
					|s|
					var running, restart, options, default;
					var runningOrBooting = (s.serverRunning || s.serverBooting);

					running = MenuAction(runningOrBooting.if("◼ Stop", "▶ Start"));
					running.action = {
						if (runningOrBooting.not) {
							s.boot;
						} {
							s.quit;
						}
					};

					if (s.serverRunning) {
						restart = MenuAction("↻ Restart");
						restart.action = { s.restart };
					};

					if (\ServerOptionsGui.asClass.notNil) {
						options = MenuAction("Options...");
						options.action_({
							\ServerOptionsGui.asClass.new(s);
						});
					};

					menu.addAction(MenuAction("  %  ".format(s.name)).font_(Font(italic:true)));
					menu.addAction(running);
					restart !? { menu.addAction(restart) };
					options !? { menu.addAction(options) };
					menu.addAction(MenuAction.separator);
				}
			}
		});

		applicationMenu = Menu(
			MenuAction("Stop", { CmdPeriod.run; }).shortcut_("Ctrl+."),
			serversMenu,
			MenuAction.separator,
			MenuAction("Quit", { 0.exit; }).shortcut_("Ctrl+Q")
		).title_("SuperCollider");

		registered = List();

		this.clear();
	}

	*register {
		|action, menu, group=\none|
		var itemAssoc;
		menu 		= this.prGetItem(menu.asSymbol, registered, List).value;
		group 		= this.prGetItem(group.asSymbol, menu, List).value;
		itemAssoc 	= this.prGetItem(action.asAction.string.asSymbol, group);
		itemAssoc.value = action;

		this.prUpdate();
	}

	*unregister {
		|toRemove|
		registered.do {
			|menu|
			menu.value.do {
				|group|
				group.value.removeAllSuchThat({
					|assoc|
					assoc.value == toRemove;
				})
			}
		};

		this.prUpdate();
	}

	*prGetItem {
		|name, list, createClass|
		var found;

		found = list.select({ |item| item.key == name });
		if (found.isEmpty) {
			found = name -> (createClass !? { createClass.new });
			list.add(found);
			^found
		} {
			^found[0];
		};
	}


	*clear {
		otherMenus = [];
		this.prUpdate();
	}

	*add {
		|menu|
		otherMenus = otherMenus.add(menu);
		this.prUpdate();
	}

	*remove {
		|menu|
		otherMenus = otherMenus.remove(menu);
		this.prUpdate();
	}

	*insert {
		|index, menu|
		otherMenus = otherMenus.insert(index, menu);
		this.prUpdate();
	}

	*prUpdate {
		var registeredMenus = registered.collect {
			|groupsAssoc|
			var menu, groups, firstGroup, otherGroups, lastGroup;

			menu = Menu().title_(groupsAssoc.key);
			groups = groupsAssoc.value;

			firstGroup = groups.select({ |g| g.key == \none });
			otherGroups = groups.reject({ |g| g.key == \none });

			(firstGroup ++ otherGroups).do {
				|itemsAssoc|
				var name, items;
				name = itemsAssoc.key;
				items = itemsAssoc.value;

				if (name != \none) {
					menu.addAction(MenuAction.separator.string_(name))
				};

				menu.addAction(MenuAction.separator.string_(name));
				items.do {
					|actionAssoc|
					menu.addAction(actionAssoc.value.asAction)
				}
			};

			menu
		};

		var actualotherMenus = ([applicationMenu] ++ otherMenus ++ registeredMenus).asArray();
		this.prSetAppMenus(actualotherMenus);
	}

	*prSetAppMenus {
		|menus|
		_Qt_SetAppMenus
	}
}

AbstractActionView : View {
	actions {
		^this.getProperty(\actions);
	}

	clear {
		this.invokeMethod('clear');
	}

	addAction {
		|action|
		^this.invokeMethod('addAction', action.asAction);
	}

	removeAction {
		|action|
		^this.invokeMethod('removeAction', action.asAction);
	}

	insertAction {
		|before, action|
		if (before.isKindOf(Number)) {
			before = this.actions[before]
		};

		^this.invokeMethod('insertAction', before, action.asAction);
	}
}

Menu : AbstractActionView {
	*qtClass { ^'QcMenu' }

	*new {
		|...actions|
		var menu = super.new.init();
		actions.do {
			|a|
			menu.addAction(a);
		};
		^menu;
	}

	*newFrom {
		|otherMenu|
		^otherMenu.copy
	}

	init {
		this.connectMethod('triggered(QAction*)', 'onTriggered');
		this.connectMethod('aboutToShow()', 'onShow' /*, true*/);   // @TODO - Is this safe? Deadlock potential?
		this.connectMethod('aboutToHide()', 'onHide');
		this.connectMethod('hovered(QAction*)', 'onHovered');
	}

	copy {
		var properties = [\title, \tearOff];
		var newMenu = Menu();

		this.actions.do {
			|action|
			newMenu.addAction(action)
		};

		properties.do {
			|prop|
			newMenu.perform(prop.asSetter, this.perform(prop))
		};

		^newMenu
	}

	title 			{ 			^this.getProperty(\title) }
	title_ 			{ |title| 	^this.setProperty(\title, title) }

	tearOff 		{ 			^this.getProperty(\tearOffEnabled) }
	tearOff_ 		{ |b| 		^this.setProperty(\tearOffEnabled, b) }

	onShow 			{			this.changed(\aboutToShow) }
	onHide			{ 			this.changed(\aboutToHide) }
	onTriggered 	{ |action| 	this.changed(\triggered, action) }
	onHovered 		{ |action|	this.changed(\hovered, action) }

	front {
		|point, action|
		point = point ?? QtGUI.cursorPosition;
		action = action ?? MenuAction();
		this.invokeMethod(\popup, [point, action])
	}

	asAction {
		^MenuAction(this.title).menu_(this)
	}

	asMenu {
		^this
	}

	remove {
		// Don't destroy menus when they are closed - just hide them
	}

	printOn { arg stream; stream << this.class.name << "(\"" << (this.title ? "-")  << "\")" }
}

ToolBar : AbstractActionView {
	*qtClass { ^'QcToolBar' }

	*new {
		|...actions|
		var toolbar = super.new.init();
		actions.do {
			|a|
			toolbar.addAction(a);
		};
		^toolbar;
	}

	init {}

	orientation { 		^this.getProperty(\orientation) }
	orientation_{ |i| 	^this.setProperty(\orientation, QOrientation(i)) }

	toolButtonStyle { 		^this.getProperty(\toolButtonStyle) }
	toolButtonStyle_{ |i| 	^this.setProperty(\toolButtonStyle, QToolButtonStyle(i)) }

	// These are not currently functional in an SC context
	movable 	{ 		^this.getProperty(\movable) }
	movable_ 	{ |b| 	^this.setProperty(\movable, b) }

	floatable 	{ 		^this.getProperty(\floatable) }
	floatable_ 	{ |b| 	^this.setProperty(\floatable, b) }

	floating 	{ 		^this.getProperty(\floating) }
	floating_ 	{ |b| 	^this.setProperty(\floating, b) }
}

+View {
	asAction {
		|func|
		^CustomViewAction(this, func)
	}
}

+Function {
	asAction {
		^MenuAction().action_(this)
	}
}

+String {
	asAction {
		^MenuAction().string_(this)
	}
}


AbstractAction : QObject {
	var <>func;

	initConnections {
		this.connectMethod('changed()', 'onChanged');
		this.connectMethod('triggered(bool)', 'onTriggered');
		this.connectMethod('hovered()', 'onHovered');
		this.connectMethod('toggled(bool)', 'onToggled');

		this.menuRole = 0;
	}

	onChanged 	{ this.changed(\changed) }
	onTriggered	{ |b| this.changed(\triggered, b); func.value(this, b); }
	onHovered 	{ this.changed(\hovered) }
	onToggled 	{ this.changed(\toggled) }

	// @TODO needs enum class
	menuRole 	{ 		^this.getProperty(\menuRole) }
	menuRole_ 	{ |b| 	^this.setProperty(\menuRole, b) }

	shortcut 	{ 		^this.getProperty(\shortcut) }
	shortcut_ 	{ |str| ^this.setProperty(\shortcut, str) }

	enabled 	{		^this.getProperty(\enabled) }
	enabled_ 	{ |b| 	^this.setProperty(\enabled, b) }

	string 		{ 		^this.getProperty(\text) }
	string_	 	{ |str| ^this.setProperty(\text, str) }

	menu 		{ 		^this.getProperty(\menu) }
	menu_ 		{ |b| 	^this.setProperty(\menu, b) }

	asAction { ^this }
}

Action : AbstractAction {
	*qtClass { ^'QcAction' }

	*new {
		|string, function|
		^super.new.init().string_(string).func_(function);
	}

	*separator {
		|string|
		^Action().separator_(true).string_(string)
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
}

CustomViewAction : AbstractAction {
	*new {
		|view, function|
		^super.new.init().defaultView_(view).func_(function)
	}

	*qtClass { ^'QcWidgetAction' }

	init {
		this.initConnections
	}

	defaultView { 		^this.getProperty(\defaultWidget) }
	defaultView_{ |v| 	^this.setProperty(\defaultWidget, v) }
}

MainMenu {
	classvar <otherMenus;
	classvar <applicationMenu;
	classvar <serversMenu;
	classvar <registered;
	classvar systemMenus;
	classvar <>buildAppMenusAction;

	*initClass {
		serversMenu = Menu(Action.separator).title_("Servers");

		applicationMenu = Menu(
			Action("Stop", { CmdPeriod.run; }).shortcut_("Ctrl+."),
			serversMenu,
			Action.separator,
			Action("Quit", { 0.exit; }).shortcut_("Ctrl+Q")
		).title_("SuperCollider");

		this.prUpdateServersMenu();
		applicationMenu.addDependant({
			|menu, what|
			if (what == \aboutToShow) {
				this.prUpdateServersMenu();
			}
		});
		Server.all.do(_.addDependant({ this.prUpdateServersMenu() }));

		registered = List();

		systemMenus = [\SuperCollider, \File, \Edit, \Server, \Quarks];
		systemMenus.do({ |systemMenu| this.prGetMenu(systemMenu) });

		this.clear();
	}

	*register {
		|action, menu, group=\none|
		var menuList, existingIndex;

		menu = menu.asSymbol;
		group = group.asSymbol;

		menuList = this.prGetMenuGroup(menu, group);
		existingIndex = menuList.detectIndex({ |existing| existing.string == action.string });
		if (existingIndex.notNil) {
			"Menu item '%' replaced an existing menu".format(action.string).warn;
			menuList[existingIndex] = action;
		} {
			menuList.add(action);
		};

		this.clear().prUpdate();
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

	*prGetMenu {
		|name|
		var menu, insertIndex;
		menu = registered.detect({ |m| m.isKindOf(Association) and: { m.key == name } });

		if (menu.notNil) {
			menu = menu.value;
		} {
			menu = List.newFrom([ \none -> List() ]);
			insertIndex = systemMenus.detectIndex(_ == name);
			if (insertIndex.isNil) {
				insertIndex = registered.size();
			};
			registered.insert(insertIndex, name -> menu);
		};

		^menu;
	}

	*prGetMenuGroup {
		|menuName, groupName|
		var menu, group;

		menu = this.prGetMenu(menuName);
		group = menu.detect({ |g| g.isKindOf(Association) and: { g.key == groupName } });

		if (group.notNil) {
			group = group.value;
		} {
			group = List();
			menu.add(groupName -> group);
		};

		^group;
	}

	*prUpdateServersMenu {
		serversMenu.clear();
		Server.all.do {
			|s|
			var running, options, kill, default;
			var startString, runningString, defaultString;

			startString = if (s.serverRunning, "Stop", "Boot");
			runningString = if (s.serverRunning, "(running)", "(stopped)");
			defaultString = if (s == Server.default, "◎", " ");

			running = Action(startString);
			running.func = {
				if (s.serverRunning) {
					s.quit;
				} {
					s.boot;
				}
			};
			if ((s == Server.default) && s.serverRunning.not) {
				running.shortcut = "Ctrl+B";
			};

			kill = Action("Kill");
			kill.func = { s.kill };

			options = Action("Options...");
			options.func_({
				ServerOptionsGui(s);
			});

			serversMenu.addAction(Action(defaultString + s.name + runningString).font_(Font(italic:true)));
			serversMenu.addAction(running);

			if (s.serverRunning) {
				serversMenu.addAction(kill)
			};

			if (\ServerOptionsGui.asClass.notNil) {
				serversMenu.addAction(options);
			};

			serversMenu.addAction(Action.separator);
		};

		serversMenu.addAction(Action("Kill All", { Server.killAll; }));
	}


	*prBuildAppMenus {
		if (buildAppMenusAction.notNil) {
			^buildAppMenusAction.value(registered);
		} {
			^registered.collect {
				|entry|
				var menu, menuName, menuItems;

				menuName = entry.key;
				menuItems = entry.value;

				menu = Menu().title_(menuName.asString);

				menuItems.do {
					|item, i|
					var groupName, groupItems;
					groupName = item.key;
					groupItems = item.value;

					if (i != 0) {
						menu.addAction(Action.separator.string_(groupName));
					};

					groupItems.do {
						|groupItem|
						menu.addAction(groupItem);
					}
				};

				menu
			};
		}
	}

	*prUpdate {
		var menus = this.prBuildAppMenus();
		var actualotherMenus = ([applicationMenu] ++ menus).asArray();
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

	title 			{ 		^this.getProperty(\title) }
	title_ 			{ |b| 	^this.setProperty(\title, b) }

	tearOff 		{ 		^this.getProperty(\tearOffEnabled) }
	tearOff_ 		{ |b| 	^this.setProperty(\tearOffEnabled, b) }

	onShow 			{		this.changed(\aboutToShow) }
	onHide			{ 		this.changed(\aboutToHide) }
	onTriggered 	{ |act| this.changed(\triggered, act) }
	onHovered 		{ 		this.changed(\hovered) }

	front {
		|point, action|
		point = point ?? QtGUI.cursorPosition;
		action = action ?? Action();
		this.invokeMethod(\popup, [point, action])
	}

	asAction {
		^Action(this.title).menu_(this)
	}

	asMenu {
		^this
	}

	remove {
		// Don't destroy menus when they are closed - just hide them
	}
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

	// @TODO needs enum class
	orientation { 		^this.getProperty(\orientation) }
	orientation_{ |i| 	^this.setProperty(\orientation, i) }

	moveable 	{ 		^this.getProperty(\moveable) }
	moveable_ 	{ |b| 	^this.setProperty(\moveable, b) }

	// @TODO - do these work? how?
	floatable 	{ 		^this.getProperty(\floatable) }
	floatable_ 	{ |b| 	^this.setProperty(\floatable, b) }

	floating 	{ 		^this.getProperty(\floating) }
	floating_ 	{ |b| 	^this.setProperty(\floating, b) }

	attachView {
		|action, view|
		if (action.isNumber) {
			action = this.actions[action];
		};
		this.invokeMethod('attachWidget', [action, view])
	}
}

+View {
	asAction {
		|func|
		^CustomViewAction(this, func)
	}
}

+Function {
	asAction {
		^Action().func_(this)
	}
}

+String {
	asAction {
		^Action().string_(this)
	}
}


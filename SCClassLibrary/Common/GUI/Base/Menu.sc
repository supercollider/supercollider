AbstractMenuAction : QObject {
	var <>action;

	initConnections {
		this.connectMethod('changed()', 'onChanged');
		this.connectMethod('triggered(bool)', 'onTriggered');
		this.connectMethod('hovered()', 'onHovered');
		this.connectMethod('toggled(bool)', 'onToggled');

		this.menuRole = \noRole;
	}

	onChanged 	{ this.changed(\changed) }
	onTriggered	{ |b| 	this.changed(\triggered, b); action.value(this, b); }
	onHovered 	{ this.changed(\hovered) }
	onToggled 	{ |b|	this.changed(\toggled, b) }

	menuRole 	{ 		^this.getProperty(\menuRole) }
	menuRole_ 	{ |b| 	^this.setProperty(\menuRole, QMenuRole(b)) }

	shortcut 	{ 		^this.getProperty(\shortcut) }
	shortcut_ 	{ |str| ^this.setProperty(\shortcut, str) }

	string 		{ 		^this.getProperty(\text) }
	string_	 	{ |str| ^this.setProperty(\text, str) }

	menu 		{ 		^this.getProperty(\menu) }
	menu_ 		{ |b| 	^this.setProperty(\menu, b) }

	asMenuAction { ^this }
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

	enabled 	{ 		^this.getProperty(\enabled) }
	enabled_ 	{ |b| 	^this.setProperty(\enabled, b) }

	toolTip 	{ 		^this.getProperty(\toolTip) }
	toolTip_ 	{ |str| ^this.setProperty(\toolTip, str) }

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
	defaultView_{ |v| 	^this.setProperty(\defaultWidget, v) }
}

MainMenu {
	classvar <otherMenus;
	classvar <applicationMenu;
	classvar <serversMenu, serversMenuActions;
	classvar <registered;
	classvar systemMenus;
	classvar <>buildAppMenusAction;

	*initClass {}

	*initBuiltInMenus {
		if(Platform.hasQt.not) { ^nil; };	// skip init on Qt-less builds

		serversMenu = Menu().title_("Servers");

		applicationMenu = Menu(
			MenuAction("Stop", { CmdPeriod.run; }).shortcut_("Ctrl+."),
			serversMenu,
			MenuAction.separator,
			MenuAction("Quit", { 0.exit; }).shortcut_("Ctrl+Q")
		).title_("SuperCollider");

		applicationMenu.addDependant({
			|menu, what|
			if (what == \aboutToShow) {
				{ this.prUpdateServersMenu() }.defer;
			}
		});
		Server.all.do(_.addDependant({
			{ this.prUpdateServersMenu() }.defer
		}));

		registered = List();

		systemMenus = [\SuperCollider, \File, \Edit, \Server, \Quarks, \Help];
		systemMenus.do({ |systemMenu| this.prGetMenu(systemMenu) });

		this.prUpdateServersMenu();
		this.clear();
		this.prUpdate();
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

		this.prUpdate();
	}

	*unregister {
		|removeAction|
		registered.do {
			|menuAssoc|
			menuAssoc.value.do {
				|groupAssoc|
				groupAssoc.value.do {
					|action, i|
					if (removeAction == action) {
						groupAssoc.value.removeAt(i);
						^this.prUpdate();
					}
				}
			}
		}
	}

	*registerQuarkMenu {
		|quarkName, menu|
		menu.title = quarkName;
		this.register(menu, \Quarks);
	}

	*unregisterQuarkMenu {
		|quarkName|
		var menuList, existingIndex;
		menuList = this.prGetMenuGroup(\Quarks, \none);
		existingIndex = menuList.detectIndex({ |existing| existing.string.asSymbol == quarkName.asSymbol });
		if (existingIndex.notNil) {
			menuList.removeAt(existingIndex)
		};

		this.prUpdate();
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
		if (serversMenu.actions.size == 0) {
			serversMenu.addAction(MenuAction.separator);
			serversMenu.addAction(MenuAction("Kill All", { Server.killAll; }));
		};

		serversMenuActions = serversMenuActions ?? { () };
		serversMenuActions.keys.difference(Server.all.asSet).do {
			|missing|
			serversMenuActions[missing].do {
				|action|
				serversMenu.removeAction(action);
			};
			serversMenuActions[missing] = nil;
		};

		Server.all.do {
			|s|
			var actionsList;
			// var running, options, kill, default;
			var startString, runningString, defaultString;
			var actions, new;

			actionsList = serversMenuActions[s] ?? {
				new = ();
				serversMenuActions[s] = new;

				new[\name] = MenuAction().font_(Font(italic:true)).action_({
					Server.default = s;
				});

				new[\running] = MenuAction().action_({
					if (s.serverRunning) {
						s.quit;
					} {
						s.boot;
					}
				});

				new[\kill] = MenuAction("Kill", { s.kill });

				new[\options] = MenuAction("Options...", {
					\ServerOptionsGui.asClass.new(s);
				});

				new[\separator] = MenuAction.separator;

				actions = ([\name, \running, \kill] ++ (\ServerOptionsGui.asClass !? \options) ++ [\separator]);
				actions = actions.collect(new.at(_));
				actions.do {
					|a|
					serversMenu.insertAction(serversMenu.actions.size - 2, a);
				};

				new;
			};

			startString = if (s.serverRunning, "Stop", "Boot");
			runningString = if (s.serverRunning, "(running)", "(stopped)");
			defaultString = if (s == Server.default, "◎", " ");

			actionsList[\name].string = defaultString + s.name + runningString;

			actionsList[\running].string = startString;
			if(s == Server.default) {
				actionsList[\running].shortcut = if(s.serverRunning, "", "Ctrl+B");
			};

			actionsList[\kill].enabled = (s.serverRunning);
		};
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
						menu.addAction(MenuAction.separator.string_(groupName));
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
		var actualotherMenus = ([applicationMenu] ++ otherMenus ++ menus).asArray();
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
		^this.invokeMethod('addAction', action.asMenuAction);
	}

	removeAction {
		|action|
		^this.invokeMethod('removeAction', action.asMenuAction);
	}

	insertAction {
		|before, action|
		if (before.isKindOf(Number)) {
			before = this.actions[before]
		};

		^this.invokeMethod('insertAction', [before.asMenuAction, action.asMenuAction]);
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

	title 			{ 		^this.getProperty(\title) }
	title_ 			{ |title| 	^this.setProperty(\title, title) }

	string			{ 			^this.title }
	string_			{ |title| 	^this.title_(title) }

	tearOff 		{ 		^this.getProperty(\tearOffEnabled) }
	tearOff_ 		{ |b| 	^this.setProperty(\tearOffEnabled, b) }

	onShow 			{		this.changed(\aboutToShow) }
	onHide			{ 		this.changed(\aboutToHide) }
	onTriggered 	{ |action| 	this.changed(\triggered, action) }
	onHovered 		{ |action|	this.changed(\hovered, action) }

	front {
		|point, action|
		point = point ?? QtGUI.cursorPosition;
		action = action ?? MenuAction();
		this.invokeMethod(\popup, [point, action])
	}

	asMenuAction {
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
	toolButtonStyle_{ |i| 	^this.setProperty(\toolButtonStyle, i) }

	// These are not currently functional in an SC context
	movable 	{ 		^this.getProperty(\movable) }
	movable_ 	{ |b| 	^this.setProperty(\movable, b) }

	floatable 	{ 		^this.getProperty(\floatable) }
	floatable_ 	{ |b| 	^this.setProperty(\floatable, b) }

	floating 	{ 		^this.getProperty(\floating) }
	floating_ 	{ |b| 	^this.setProperty(\floating, b) }
}

+View {
	asMenuAction {
		|func|
		^CustomViewAction(this, func)
	}
}

+Function {
	asMenuAction {
		^MenuAction().action_(this)
	}
}

+String {
	asMenuAction {
		^MenuAction().string_(this)
	}
}


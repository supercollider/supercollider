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
	defaultView_{ |b| 	^this.setProperty(\defaultWidget, b) }
}

MainMenu {
	classvar <otherMenus;
	classvar <applicationMenu;
	classvar <registered;

	*initClass {
		var serversMenu = Menu(Action.separator).title_("Servers");
		serversMenu.addDependant({
			|menu, what|
			if (what == \aboutToShow) {
				menu.clear();
				Server.all.do {
					|s|
					var running, options, default;

					running = Action("Running");
					running.checked = s.serverRunning;
					running.func = {
						if (running.checked) {
							s.boot;
						} {
							s.quit;
						}
					};

					options = Action("Options...");
					options.func_({
						ServerOptionsGui(s);
					});

					menu.addAction(Action("◎" + s.name).font_(Font(italic:true)));
					menu.addAction(running);
					menu.addAction(options);
					menu.addAction(Action.separator);
				}
			}
		});

		applicationMenu = Menu(
			Action("Stop", { CmdPeriod.run; }).shortcut_("Ctrl+."),
			serversMenu,
			Action.separator,
			Action("Quit", { 0.exit; }).shortcut_("Ctrl+Q")
		).title_("SuperCollider");

		registered = ();

		this.clear();
	}

	*register {
		|action, menu, group=\none|
		menu = menu.asSymbol;
		group = group.asSymbol;

		registered[menu] = registered[menu] ?? { () };
		registered[menu][group] = registered[menu][group] ?? { () };
		registered[menu][group][action.string.asSymbol] = action;

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

	*prUpdate {
		var menus = registered.collect {
			|groups, name|
			var menu = Menu().title_(name.asString);
			groups.do {
				|group, groupName|
				menu.addAction(Action.separator.string_(groupName));
				group.do {
					|action|
					menu.addAction(action.asAction)
				}
			};
			menu
		};
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
		this.connectMethod('aboutToShow()', 'onShow', true);   // @TODO - Is this safe? Deadlock potential?
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


# sced (SuperCollider mode for gedit)
# Copyright 2009 Artem Popov and other contributors (see AUTHORS)
#
#    sced is free software: 
#    you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
# 
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
# 
#    You should have received a copy of the GNU General Public License
#    along with this program.  If not, see <http://www.gnu.org/licenses/>.

import gtk
import gedit
import gio

from LogPanel import LogPanel
from ScLang import ScLang
from Logger import Logger
from util import *

ui_str = """<ui>
  <menubar name="MenuBar">
    <menu name="ToolsMenu" action="Tools">
      <placeholder name="ToolsOps_5">
        <menuitem action="ScedSuperColliderMode"/>
      </placeholder>
    </menu>
  </menubar>
</ui>
"""

scui_str = """<ui>
  <menubar name="MenuBar">
    <placeholder name="ExtraMenu_1">
      <menu action="SuperColliderMenu">
        <menuitem action="ScedEvaluate"/>
        <separator/>
        <menuitem action="ScedStopSound"/>
        <menuitem action="ScedRecord"/>
        <separator/>
        <menuitem action="ScedServerGUI"/>
        <menuitem action="ScedServerMeter"/>
        <menuitem action="ScedStartServer"/>
        <menuitem action="ScedStopServer"/>
        <separator/>
        <menuitem action="ScedStartSwingOSC"/>
        <menuitem action="ScedStopSwingOSC"/>
        <separator/>
        <menuitem action="ScedFindHelp"/>
        <menuitem action="ScedBrowseHelp"/>
        <menuitem action="ScedSearchHelp"/>
        <menuitem action="ScedMethodArgs"/>
        <separator/>
        <menuitem action="ScedFindDefinition"/>
        <menuitem action="ScedBrowseClass"/>
        <menuitem action="ScedOpenDevFile"/>
        <separator/>
        <menuitem action="ScedInspectObject"/>
        <separator/>
        <menuitem action="ScedRestartInterpreter"/>
        <menuitem action="ScedRecompile"/>
        <menuitem action="ScedClearOutput"/>
      </menu>
    </placeholder>
  </menubar>

  <toolbar name="ToolBar">
    <separator/>
    <toolitem action="ScedRecord"/>
  </toolbar>
</ui>
"""

class WindowHelper:
    def __init__(self, plugin, window):
        self.__plugin = plugin
        self.__window = window

        self.__insert_menu()

    def deactivate(self):
        self.__remove_menu()

        self.__plugin = None
        self.__window = None

    def update_ui(self):
        pass

    def __insert_menu(self):
        manager = self.__window.get_ui_manager()
        self.__actions = gtk.ActionGroup("ScedActions")

        toggle_entries = [
            ("ScedSuperColliderMode", None, "_SuperCollider Mode", None,
             _("Toggle SuperCollider interaction mode"),
             self.on_sc_mode_activate,
             False)
        ]

        self.__actions.add_toggle_actions(toggle_entries)

        manager.insert_action_group(self.__actions, -1)
        self.__ui_id = manager.add_ui_from_string(ui_str)

    def __remove_menu(self):
        manager = self.__window.get_ui_manager()
        manager.remove_ui(self.__ui_id)
        manager.remove_action_group(self.__actions)
        manager.ensure_update()

    def __insert_sc_menu(self):
        manager = self.__window.get_ui_manager()
        self.__sc_actions = gtk.ActionGroup("SuperColliderActions")

        entries = [
            ("SuperColliderMenu",   None, "Super_Collider"),

            ("ScedEvaluate", gtk.STOCK_EXECUTE, _("Evaluate"), "<control>E",
             _("Evaluate line or selection"),
             self.on_evaluate),

            ("ScedStopSound", gtk.STOCK_STOP, _("Stop Sound"), "Escape",
             _("Stop sound and free all server nodes"),
             self.on_stop_sound),

            ("ScedFindHelp", None, _("Find Help"), "<control>U",
             _("Find and open help file"),
             self.on_find_help),

            ("ScedBrowseHelp", None, _("Browse Help"), None,
             _("Browse help by categories"),
             self.on_browse_help),

            ("ScedSearchHelp", None, _("Search Help"), "<control><alt>U",
             _("Search for help"),
             self.on_search_help),

            ("ScedMethodArgs", None, _("Show method args"), "<alt>A",
             _("Show method arguments and defaults"),
             self.on_method_args),

            ("ScedFindDefinition", None, _("Find Definition"), "<control>Y",
             _("Find and open class definition"),
             self.on_find_definition),

            ("ScedBrowseClass", None, _("Browse class"), None,
             _("Browse class"),
             self.on_browse_class),

            ("ScedOpenDevFile", None, _("Open development file"), "<control><alt>K",
             _("Open corresponding development file for current document"),
             self.on_open_dev_file),

            ("ScedInspectObject", None, _("Inspect Object"), None,
             _("Inspect object state"),
             self.on_inspect_object),

            ("ScedRestartInterpreter", None, _("Restart Interpreter"), None,
             _("Restart sclang"),
             self.on_restart),

            ("ScedRecompile", None, _("Recompile class library"), "<control><shift>R",
             _("Recompile class library"),
             self.on_recompile),

            ("ScedClearOutput", gtk.STOCK_CLEAR, _("Clear output"), None,
             _("Clear interpreter log"),
             self.on_clear_log),

            ("ScedServerGUI", None, _("Show Server GUI"), None,
             _("Show GUI for default server"),
             self.on_server_gui),

            ("ScedServerMeter", None, _("Show level meters"), None,
             _("Show level meters for default server"),
             self.on_server_meter),

            ("ScedStartServer", None, _("Start Server"), None,
             _("Start the default server"),
             self.on_start_server),

            ("ScedStopServer", None, _("Stop Server"), None,
             _("Stop the default server"),
             self.on_stop_server),

            ("ScedStartSwingOSC", None, _("Start SwingOSC GUI Server"), None,
             _("Start the SwingOSC GUI server"),
             self.on_start_swingosc),

            ("ScedStopSwingOSC", None, _("Stop SwingOSC GUI Server"), None,
             _("Stop the SwingOSC GUI server"),
             self.on_stop_swingosc),
        ]

        toggle_entries = [
            ("ScedRecord", gtk.STOCK_MEDIA_RECORD, "Record", None,
             _("Toggle recording"),
             self.on_record,
             False)
        ]

        self.__sc_actions.add_actions(entries)
        self.__sc_actions.add_toggle_actions(toggle_entries)
        manager.insert_action_group(self.__sc_actions, -1)
        self.__scui_id = manager.add_ui_from_string(scui_str)

    def __remove_sc_menu(self):
        manager = self.__window.get_ui_manager()
        manager.remove_ui(self.__scui_id)
        manager.remove_action_group(self.__sc_actions)
        manager.ensure_update()

    def on_sc_mode_activate(self, action):
        if action.get_active():
            self.__log_panel = LogPanel()
            panel = self.__window.get_bottom_panel()
            panel.show()
            panel.add_item(self.__log_panel, _("SuperCollider output"), gtk.STOCK_EXECUTE)
            self.__log_panel.show()

            self.__lang = ScLang(self.__plugin)
            self.__lang.start()

            self.__logger = Logger(self.__lang.stdout, self.__log_panel)
            self.__insert_sc_menu()
        else:
            panel = self.__window.get_bottom_panel()
            panel.remove_item(self.__log_panel)
            # FIXME: un-record
            self.__lang.stop()
            self.__logger.stop()
            self.__remove_sc_menu()

    def on_evaluate(self, action):
        doc = self.__window.get_active_document()

        try:
            i1, i2 = doc.get_selection_bounds()
        except ValueError:
            i1 = doc.get_iter_at_mark(doc.get_insert())
            i1.set_line_offset(0)
            i2 = i1.copy()
            i2.forward_to_line_end()

            if is_block_beginning(doc.get_text(i1, i2)):
                try:
                    i1, i2 = find_block(doc, i1)
                except RuntimeError:
                    statusbar = self.__window.get_statusbar()
                    context = statusbar.get_context_id("supercollider")
                    statusbar.flash_message(context,
                        "Code block is not properly closed")
                    return
                doc.select_range(i1, i2)

        text = doc.get_text(i1, i2)
        self.__lang.evaluate(text)

    def on_stop_sound(self, action):
        record = self.__sc_actions.get_action("ScedRecord");
        if record.get_active():
            record.activate() # untoggle
        self.__lang.stop_sound()

    def on_record(self, action):
        self.__lang.toggle_recording(action.get_active())

    def get_selection(self):
        doc = self.__window.get_active_document()

        try:
            i1, i2 = doc.get_selection_bounds()
        except ValueError:
            i1 = doc.get_iter_at_mark(doc.get_insert())
            i1, i2 = find_word(doc, i1)
            doc.select_range(i1, i2)

        return doc.get_text(i1, i2)

    def on_find_help(self, action):
        text = self.get_selection()
        self.__lang.evaluate("HelpBrowser.openHelpFor(\"" + text + "\");")

    def on_browse_help(self, action):
        self.__lang.evaluate("HelpBrowser.openBrowsePage;")

    def on_search_help(self, action):
        text = self.get_selection()
        self.__lang.evaluate("HelpBrowser.openSearchPage(\"" + text + "\");")

    def on_method_args(self, action):
        text = self.get_selection()
        self.__lang.evaluate("Help.methodArgs(\"" + text + "\");")

    def on_find_definition(self, action):
        text = self.get_selection()
        self.__lang.evaluate("(\"gedit \" + (\"" + text + "\"" + ".interpret.filenameSymbol.asString)).systemCmd", silent=True)

    def on_browse_class(self, action):
        text = self.get_selection()
        self.__lang.evaluate("" + text + ".browse", silent=True)

    def on_open_dev_file(self, action):
        doc = self.__window.get_active_document()
        path = gio.File(doc.get_uri()).get_path() #get_location()
        self.__lang.evaluate("(\"gedit\"+thisProcess.platform.devLoc(\""+path+"\")).systemCmd", silent=True);

    def on_inspect_object(self, action):
        text = self.get_selection()
        self.__lang.evaluate("" + text + ".inspect", silent=True)

    def on_recompile(self, action):
        self.__lang.stdin.write("\x18")

    def on_restart(self, action):
        if self.__lang.running():
            self.__lang.stop()
        self.__lang.start()
        self.__logger = Logger(self.__lang.stdout, self.__log_panel)

    def on_clear_log(self, action):
        self.__log_panel.buffer.set_text("")

    def on_server_gui(self, action):
        self.__lang.evaluate("Server.default.makeGui;", silent=True)

    def on_server_meter(self, action):
        self.__lang.evaluate("Server.default.meter;", silent=True)

    def on_start_server(self, action):
        # FIXME: make these actions possible only if interpreter is running and okay
        self.__lang.evaluate("Server.default.boot;", silent=True)

    def on_stop_server(self, action):
        # FIXME: make these actions possible only if interpreter is running and okay
        self.__lang.evaluate("Server.default.quit;", silent=True)

    def on_start_swingosc(self, action):
        # FIXME: make these actions possible only if interpreter is running and okay
        self.__lang.evaluate("SwingOSC.default.boot;GUI.swing;", silent=False)

    def on_stop_swingosc(self, action):
        # FIXME: make these actions possible only if interpreter is running and okay
        self.__lang.evaluate("SwingOSC.default.quit;", silent=False)

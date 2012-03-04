# -*- coding: utf-8 -*-
# Copyright 2009-2011 Artem Popov and contributors (see AUTHORS)
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

import re, subprocess, time
from gi.repository import GObject, Gedit, Gtk, Pango

ui_str = """<ui>
  <menubar name="MenuBar">
    <menu name="ToolsMenu" action="Tools">
      <placeholder name="ToolsOps_5">
        <menuitem action="ScedSuperColliderMode"/>
      </placeholder>
    </menu>
  </menubar>
</ui>"""

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
        <menuitem action="ScedStartServer"/>
        <menuitem action="ScedStopServer"/>
        <separator/>
        <menuitem action="ScedFindHelp"/>
        <menuitem action="ScedBrowseHelp"/>
        <menuitem action="ScedSearchHelp"/>
        <menuitem action="ScedMethodArgs"/>
        <separator/>
        <menuitem action="ScedFindDefinition"/>
        <menuitem action="ScedBrowseClass"/>
        <menuitem action="ScedInspectObject"/>
        <menuitem action="ScedOpenDevFile"/>
        <separator/>
        <menuitem action="ScedFrontWindows"/>
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
</ui>"""

def is_block_beginning(s):
    s = "".join(s.split())
    if s == "(" or s.startswith("(//") or s.startswith("(/*"):
        return True
    else:
        return False

def find_block(doc, where=None):
    if where is None:
        i1 = doc.get_iter_at_mark(doc.get_insert())
    else:
        i1 = where.copy()

    # move backward until a block beginning is found
    while True:
        i1.set_line_offset(0)

        i2 = i1.copy()
        i2.forward_to_line_end()

        if is_block_beginning(doc.get_text(i1, i2, False)):
            break

        if not i1.backward_line():
            raise RuntimeError("Couldn't find where code block starts!")

    i2 = i1.copy()
    count = 1

    line_comment = False
    block_comment = 0

    # move forward to the end of the block
    while True:
        if not i2.forward_char():
            raise RuntimeError("Couldn't find where code block ends!")

        char = i2.get_char()

        i3 = i2.copy()
        i3.forward_chars(2)
        ct = i2.get_text(i3)

        if ct == "*/":
            block_comment -= 1
        elif ct == "/*":
            block_comment += 1
        elif ct == "//":
            line_comment = True
        elif char == "\n" and line_comment:
            line_comment = False

        if not block_comment and not line_comment:
            if char == "(":
                count += 1
            elif char == ")":
                count -= 1

        if count == 0:
            break

    # include 2 more characters just in case "where" is near the end
    i2.forward_chars(2)

    if where.in_range(i1, i2):
        return i1, i2
    else:
        raise RuntimeError("Couldn't find code block!")

def class_char_predicate(c, *args):
    if re.match("[A-Za-z0-9_]", c):
        return False
    return True

def find_word(doc, where=None):
    if where is None:
        i1 = doc.get_iter_at_mark(doc.get_insert())
    else:
        i1 = where.copy()

    #scclass_regex = "[A-Za-z0-9_]"
    
    while i1.backward_char():
        if not re.match("[A-Za-z0-9_]", i1.get_char()):
            break
    
    if not i1.is_start():
        i1.forward_char()
    i2 = i1.copy()
    
    while i2.forward_char():
        if not re.match("[A-Za-z0-9_]", i2.get_char()):
            break

    # FIXME: find_char no longer works with gir bindings
    #i1.backward_find_char(class_char_predicate, None, None)
    #if not i1.is_start():
    #    i1.forward_char()
    #i2 = i1.copy()
    #i2.forward_find_char(class_char_predicate, None, None)

    return i1, i2

class ScLang:
    def __init__(self):
        self.__sclang = None

    def start (self):
        if self.running():
            return

        # FIXME: maybe we need a default value in Settings?
        #folder = self.__settings.props.runtime_folder
        #if folder is None:
        #    folder = os.getcwd()

        self.__sclang = subprocess.Popen(["sclang",
                "-i", "sced"],#, "-d", folder],
                bufsize=0,
                stdin=subprocess.PIPE,
                stdout=subprocess.PIPE,
                stderr=subprocess.STDOUT,
                close_fds=True)
        self.stdout = self.__sclang.stdout
        self.stdin = self.__sclang.stdin

    def stop(self):
        if self.running():
            self.stdin.close()
            self.__sclang.wait()
            self.__sclang = None

    def running(self):
        return (self.__sclang is not None) and (self.__sclang.poll() is None)

    # FIXME: use sclang.communicate()
    def evaluate(self, code, silent=False):
        self.stdin.write(bytes(code))
        if silent:
            self.stdin.write(bytes("\x1b"))
        else:
            self.stdin.write(bytes("\x0c"))
        self.stdin.flush()

    def toggle_recording(self, record):
        if record:
            self.evaluate("s.prepareForRecord;", silent=True)
            time.sleep(0.1) # give server some time to prepare
            self.evaluate("s.record;", silent=True)
        else:
            self.evaluate("s.stopRecording;", silent=True)

    def stop_sound(self):
        self.evaluate("thisProcess.stop;", silent=True)

class Logger:
    def __init__(self, pipe, log_view):
        self.__log_view = log_view

        tag_table = log_view.buffer.get_tag_table()
        self.__tag = Gtk.TextTag()

        self.__good_tag = GObject.new(Gtk.TextTag,
            weight                  = Pango.Weight.BOLD,
            foreground              = "darkgreen",
            paragraph_background    = "lightgreen")

        self.__bad_tag = GObject.new(Gtk.TextTag,
            weight                  = Pango.Weight.BOLD,
            foreground              = "darkred",
            paragraph_background    = "pink")

        # for warnings, etc.
        self.__ugly_tag = GObject.new(Gtk.TextTag,
            #weight                  = pango.WEIGHT_BOLD,
            foreground              = "red")

        tag_table.add(self.__tag)
        tag_table.add(self.__good_tag)
        tag_table.add(self.__bad_tag)
        tag_table.add(self.__ugly_tag)

        self.__watch_id = GObject.io_add_watch(pipe,
            GObject.IO_IN |
            GObject.IO_PRI |
            GObject.IO_ERR |
            GObject.IO_HUP,
            self.__on_output)

    def __on_output(self, source, condition):
        s = source.readline()
        if s == '':
            self.__append_to_buffer("EOF")
            return False

        # FIXME: A workaround for a mac character
        self.__append_to_buffer(bytes(s))

        if condition & GObject.IO_ERR:
            s = source.read() # can safely read until EOF here
            self.__append_to_buffer(bytes(s))
            return False
        elif condition & GObject.IO_HUP:
            s = source.read() # can safely read until EOF here
            self.__append_to_buffer(bytes(s))
            return False
        elif condition != 1:
            return False
        return True

    def __append_to_buffer(self, text):
        buffer = self.__log_view.buffer

        if text.startswith("ERROR"):
            tags = self.__bad_tag

        elif text.startswith("WARNING") or text.startswith("FAILURE"):
            tags = self.__ugly_tag

        elif text.startswith("Welcome to SuperCollider"):
            tags = self.__good_tag
        else:
            tags = self.__tag

        buffer.insert_with_tags(buffer.get_end_iter(), text.rstrip(), tags)
        buffer.insert(buffer.get_end_iter(), "\n")

        buffer.place_cursor(buffer.get_end_iter())
        self.__log_view.view.scroll_mark_onscreen(buffer.get_insert())

        # only required for thread-based implementation
        # return False

    def stop(self):
        GObject.source_remove(self.__watch_id)

class LogPanel(Gtk.ScrolledWindow):
    def __init__(self):
        Gtk.ScrolledWindow.__init__(self)

        self.props.shadow_type = Gtk.ShadowType.IN
        self.props.hscrollbar_policy = Gtk.PolicyType.AUTOMATIC
        self.props.vscrollbar_policy = Gtk.PolicyType.AUTOMATIC

        self.buffer = Gtk.TextBuffer()
        self.view = Gtk.TextView()
        self.view.modify_font(Pango.FontDescription("Monospace"))
        self.view.props.buffer = self.buffer
        self.view.props.editable = False
        self.view.props.wrap_mode = Gtk.WrapMode.CHAR

        self.add(self.view)
        self.view.show()

class ScedWindowActivatable(GObject.Object, Gedit.WindowActivatable):
    __gtype_name__ = "ScedWindowActivatable"
    window = GObject.property(type=Gedit.Window)
    
    def __init__(self):
        GObject.Object.__init__(self)
    
    def do_activate(self):
        self.__insert_menu()

    def do_deactivate(self):
        self.__remove_menu()

    def do_update_state(self):
        pass
    
    def __remove_menu(self):
        manager = self.window.get_ui_manager()
        manager.remove_ui(self.__ui_id)
        manager.remove_action_group(self.__actions)
        manager.ensure_update()
    
    def __insert_menu(self):
        manager = self.window.get_ui_manager()
        self.__actions = Gtk.ActionGroup("ScedActions")

        toggle_entries = [
            ("ScedSuperColliderMode", None, "_SuperCollider Mode", None,
             _("Toggle SuperCollider interaction mode"),
             self.on_sc_mode_activate,
             False)
        ]

        self.__actions.add_toggle_actions(toggle_entries)

        manager.insert_action_group(self.__actions, -1)
        self.__ui_id = manager.add_ui_from_string(ui_str)
        manager.ensure_update()
    
    def __remove_sc_menu(self):
        manager = self.window.get_ui_manager()
        manager.remove_ui(self.__scui_id)
        manager.remove_action_group(self.__sc_actions)
        manager.ensure_update()
    
    def __insert_sc_menu(self):
        manager = self.window.get_ui_manager()
        self.__sc_actions = Gtk.ActionGroup("SuperColliderActions")

        entries = [
            ("SuperColliderMenu",   None, "Super_Collider"),

            ("ScedEvaluate", Gtk.STOCK_EXECUTE, _("Evaluate"), "<control>E",
             _("Evaluate line or selection"),
             self.on_evaluate),

            ("ScedStopSound", Gtk.STOCK_STOP, _("Stop Sound"), "Escape",
             _("Stop sound and free all server nodes"),
             self.on_stop_sound),

            ("ScedFindHelp", None, _("Find Help"), "<control>U",
             _("Find and open help file"),
             self.on_find_help),

            ("ScedBrowseHelp", None, _("Browse Help"), "<control><alt>U",
             _("Browse help by categories"),
             self.on_browse_help),

            ("ScedSearchHelp", None, _("Search Help"), None,
             _("Search for help"),
             self.on_search_help),

            ("ScedMethodArgs", None, _("Show method args"), "<alt>A",
             _("Show method arguments and defaults"),
             self.on_method_args),

            ("ScedFindDefinition", None, _("Find Definition"), "<control>Y",
             _("Find and open class definition"),
             self.on_find_definition),

            ("ScedBrowseClass", None, _("Browse class"), None,
             _("Browse class (needs running SwingOSC server)"),
             self.on_browse_class),

            ("ScedInspectObject", None, _("Inspect Object"), None,
             _("Inspect object state (needs running SwingOSC server)"),
             self.on_inspect_object),

            ("ScedOpenDevFile", None, _("Open development file"), "<control><alt>K",
             _("Open corresponding development file for current document"),
             self.on_open_dev_file),

            ("ScedRestartInterpreter", None, _("Restart Interpreter"), None,
             _("Restart sclang"),
             self.on_restart),

            ("ScedRecompile", None, _("Recompile class library"), "<control><shift>R",
             _("Recompile class library"),
             self.on_recompile),

            ("ScedClearOutput", Gtk.STOCK_CLEAR, _("Clear output"), None,
             _("Clear interpreter log"),
             self.on_clear_log),

            ("ScedStartServer", None, _("Start Server"), None,
             _("Start the default server"),
             self.on_start_server),

            ("ScedStopServer", None, _("Stop Server"), None,
             _("Stop the default server"),
             self.on_stop_server),

            ("ScedServerGUI", None, _("Show Server GUI"), None,
             _("Show GUI for default server"),
             self.on_server_gui),

            ("ScedFrontWindows", None, _("Raise all windows"), "<alt>W",
             _("Raise all windows"),
             self.on_front_windows),
        ]

        toggle_entries = [
            ("ScedRecord", Gtk.STOCK_MEDIA_RECORD, "Record", None,
             _("Toggle recording"),
             self.on_record,
             False)
        ]

        self.__sc_actions.add_actions(entries)
        self.__sc_actions.add_toggle_actions(toggle_entries)
        manager.insert_action_group(self.__sc_actions, -1)
        self.__scui_id = manager.add_ui_from_string(scui_str)
        manager.ensure_update()
    
    def on_sc_mode_activate(self, action, data=None):
        if action.get_active():
            self.__log_panel = LogPanel()
            panel = self.window.get_bottom_panel()
            panel.show()
            panel.add_item_with_stock_icon(self.__log_panel,
                "SuperCollider", _("SuperCollider output"), Gtk.STOCK_EXECUTE)
            self.__log_panel.show()

            self.__lang = ScLang()
            self.__lang.start()

            self.__logger = Logger(self.__lang.stdout, self.__log_panel)
            self.__insert_sc_menu()
        else:
            panel = self.window.get_bottom_panel()
            panel.remove_item(self.__log_panel)
            # FIXME: un-record
            self.__lang.stop()
            self.__logger.stop()
            self.__remove_sc_menu()
    
    def on_evaluate(self, action, data=None):
        doc = self.window.get_active_document()

        try:
            i1, i2 = doc.get_selection_bounds()
        except ValueError:
            i1 = doc.get_iter_at_mark(doc.get_insert())
            i1.set_line_offset(0)
            i2 = i1.copy()
            i2.forward_to_line_end()

            if is_block_beginning(doc.get_text(i1, i2, False)):
                try:
                    i1, i2 = find_block(doc, i1)
                except RuntimeError:
                    #statusbar = self.window.get_statusbar()
                    #context = statusbar.get_context_id("supercollider")
                    # FIXME: no longer works with gir
                    #statusbar.flash_message(context,
                    #    "Code block is not properly closed")
                    return
                doc.select_range(i1, i2)

        text = doc.get_text(i1, i2, False)
        self.__lang.evaluate(text)

    def on_stop_sound(self, action, data=None):
        record = self.__sc_actions.get_action("ScedRecord");
        if record.get_active():
            record.activate() # untoggle
        self.__lang.stop_sound()

    def on_record(self, action, data=None):
        self.__lang.toggle_recording(action.get_active())

    def get_selection(self):
        doc = self.window.get_active_document()

        try:
            i1, i2 = doc.get_selection_bounds()
        except ValueError:
            i1 = doc.get_iter_at_mark(doc.get_insert())
            i1, i2 = find_word(doc, i1)
            doc.select_range(i1, i2)

        return doc.get_text(i1, i2, False)

    def on_find_help(self, action, data=None):
        text = self.get_selection()
        cmd = 'HelpBrowser.openHelpFor(\"' + text + '\");'
        self.__lang.evaluate(cmd, silent=True)

    def on_browse_help(self, action):
        self.__lang.evaluate("HelpBrowser.openBrowsePage;")

    def on_search_help(self, action):
        text = self.get_selection()
        self.__lang.evaluate("HelpBrowser.openSearchPage(\"" + text + "\");")

    def on_method_args(self, action):
        text = self.get_selection()
        self.__lang.evaluate("Help.methodArgs(\"" + text + "\");")

    def on_find_definition(self, action, data=None):
        text = self.get_selection()
        self.__lang.evaluate("" + text + ".openCodeFile", silent=True)

    def on_browse_class(self, action):
        text = self.get_selection()
        self.__lang.evaluate("" + text + ".browse", silent=True)

    def on_inspect_object(self, action, data=None):
        text = self.get_selection()
        self.__lang.evaluate("" + text + ".inspect", silent=True)

    def on_open_dev_file(self, action):
        doc = self.window.get_active_document()
        if doc is None:
            return None
        location = doc.get_location()
        if location is not None and Gedit.utils_location_has_file_scheme(location):
            self.__lang.evaluate("thisProcess.platform.devLoc(\""+location.get_path()+"\").openTextFile", silent=True)

    def on_recompile(self, action):
        self.__lang.stdin.write(bytes("\x18"))

    def on_restart(self, action, data=None):
        if self.__lang.running():
            self.__lang.stop()
        self.__lang.start()
        self.__logger = Logger(self.__lang.stdout, self.__log_panel)

    def on_clear_log(self, action, data=None):
        self.__log_panel.buffer.set_text("")

    def on_start_server(self, action, data=None):
        # FIXME: make these actions possible only if interpreter is running and okay
        self.__lang.evaluate("Server.default.boot;", silent=True)

    def on_stop_server(self, action, data=None):
        # FIXME: make these actions possible only if interpreter is running and okay
        self.__lang.evaluate("Server.default.quit;", silent=True)

    def on_server_gui(self, action, data=None):
        self.__lang.evaluate("Server.default.makeGui;", silent=True)

    def on_front_windows(self, action, data=None):
        self.__lang.evaluate("Window.allWindows.do(_.front);", silent=True)


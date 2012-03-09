# sced (SuperCollider mode for gedit)
#
# Copyright 2012 Jakob Leben
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

import gobject
import gtk
from Settings import Settings

def on_pref_widget_notify_sensitive(widget, spec):
    label = widget.get_data("pref-label")
    if label is not None:
        label.set_sensitive(widget.props.sensitive)

# FIXME: implement custom widget (or custom widget sequence) as well
def create_pref_section(title, wlabels=[], custom=[]):
    vbox = gtk.VBox(spacing=6)

    label = gobject.new(gtk.Label, label="<b>%s</b>" % title,
        use_markup=True,
        xalign=0)
    vbox.pack_start(label, expand=False)
    label.show()

    align = gobject.new(gtk.Alignment, left_padding=12)
    vbox.pack_start(align, expand=False)
    align.show()

    table = gobject.new(gtk.Table,
        n_rows=len(wlabels) + len(custom),
        n_columns=2,
        row_spacing=6,
        column_spacing=12)
    align.add(table)
    table.show()

    for i in range(len(wlabels)):
        l, widget = wlabels[i]
        label = gobject.new(gtk.Label, label=l, xalign=0)
        widget.connect("notify::sensitive", on_pref_widget_notify_sensitive)
        widget.set_data("pref-label", label)

        if l is not None:
            table.attach(label, 0, 1, i, i + 1,
                xoptions=gtk.FILL, yoptions=gtk.FILL)
            table.attach(widget, 1, 2, i, i + 1,
                xoptions=gtk.EXPAND | gtk.FILL, yoptions=gtk.FILL)
        else:
            table.attach(widget, 0, 2, i, i + 1,
                xoptions=gtk.EXPAND | gtk.FILL, yoptions=gtk.FILL)

    table.show_all()
    return vbox

# FIXME: fix notification
class ConfigurationDialog(gtk.Dialog):
    __gsignals__ = {
        "response": "override",
    } # __gsignals__
    def __init__(self, plugin):
        gtk.Dialog.__init__(self, title=_("Sced configuration"),
            flags=gtk.DIALOG_MODAL | gtk.DIALOG_DESTROY_WITH_PARENT,
            buttons = (
                gtk.STOCK_CANCEL, gtk.RESPONSE_REJECT,
                gtk.STOCK_OK, gtk.RESPONSE_ACCEPT
            ))
        self.set_default_response(gtk.RESPONSE_ACCEPT)

        self.__settings = plugin.settings()

        self.__create_page_general()

    def __create_filesystem_entry( self, chooser, action, stock = gtk.STOCK_OPEN ):
        entry = gtk.Entry()
        btn = gtk.Button(stock=stock)
        box = gtk.HBox()
        box.add(entry)
        box.add(btn)

        def run_dialog(btn):
            chooser.set_action(action)
            chooser.set_filename(entry.get_text())
            response = chooser.run()
            chooser.hide()
            if response == gtk.RESPONSE_ACCEPT:
                entry.set_text(chooser.get_filename())

        btn.connect("clicked", run_dialog)

        return (box, entry)



    def __create_page_general(self):
        # create views
        chooser = gtk.FileChooserDialog(
            parent = self,
            title = "Choose interpreter program",
            buttons = (
                gtk.STOCK_CANCEL, gtk.RESPONSE_REJECT,
                gtk.STOCK_OK, gtk.RESPONSE_ACCEPT
            )
        )
        chooser.set_select_multiple(False)

        sc_dir_view, sc_dir_entry = self.__create_filesystem_entry (
            chooser,
            gtk.FILE_CHOOSER_ACTION_SELECT_FOLDER )

        adv_view = gtk.CheckButton()

        cmd_view, cmd_entry = self.__create_filesystem_entry (
            chooser,
            gtk.FILE_CHOOSER_ACTION_OPEN )

        wd_view, wd_entry = self.__create_filesystem_entry (
            chooser,
            gtk.FILE_CHOOSER_ACTION_SELECT_FOLDER )

        def toggle_advanced(advanced):
            sc_dir_view.set_sensitive(not advanced)
            cmd_view.set_sensitive(advanced)
            wd_view.set_sensitive(advanced)

        adv_view.connect("toggled", lambda btn: toggle_advanced(btn.get_active()) )

        # fill in the data
        sets = self.__settings
        if sets.sc_dir is not None:
            sc_dir_entry.set_text(sets.sc_dir)
        adv_view.set_active(sets.advanced is True)
        if sets.sclang_cmd is not None:
            cmd_entry.set_text(sets.sclang_cmd)
        if sets.sclang_work_dir is not None:
            wd_entry.set_text(sets.sclang_work_dir)
        toggle_advanced(sets.advanced is True)

        self.__adv_check = adv_view
        self.__sc_dir_entry = sc_dir_entry
        self.__cmd_entry = cmd_entry
        self.__wd_entry = wd_entry

        # lay out
        section = create_pref_section("Basic", [
            ("SuperCollider folder:", sc_dir_view),
            ("Advanced settings:", adv_view),
        ])
        section.props.border_width = 12
        self.vbox.add(section)
        section.show()
        section = create_pref_section("Interpreter options", [
            ("Command:", cmd_view),
            ("Runtime folder:", wd_view)
        ])
        section.props.border_width = 12
        self.vbox.add(section)
        section.show()

    def do_response(self, response):
        if response == gtk.RESPONSE_ACCEPT:
            sets = self.__settings
            sets.sc_dir = self.__sc_dir_entry.get_text()
            sets.advanced = self.__adv_check.get_active()
            sets.sclang_work_dir = self.__wd_entry.get_text()
            sets.sclang_cmd = self.__cmd_entry.get_text()
            sets.save()

        self.destroy()


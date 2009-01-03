import gobject
import gtk

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
        gtk.Dialog.__init__(self, title=_("Configure sced"),
            flags=gtk.DIALOG_MODAL | gtk.DIALOG_DESTROY_WITH_PARENT,
            buttons=(gtk.STOCK_CLOSE, gtk.RESPONSE_CLOSE))

        self.__settings = plugin.settings
        self.__create_page_general()

    def __create_page_general(self):
        self.__runtime_fc = gobject.new(gtk.FileChooserButton,
            title=_("Choose runtime folder"),
            action=gtk.FILE_CHOOSER_ACTION_SELECT_FOLDER)

        folder = self.__settings.props.runtime_folder
        if folder is not None:
            self.__runtime_fc.set_current_folder(folder)

        self.__runtime_fc.connect("current-folder-changed",
            self.__on_fc_selection_changed)
        #self.__settings.connect("notify::runtime-folder",
        #    self.__on_runtime_folder_changed)

        section = create_pref_section("Interpreter options", [
            ("Runtime folder:", self.__runtime_fc),
        ])
        section.props.border_width = 12

        self.vbox.add(section)
        section.show()

    def __on_fc_selection_changed(self, button):
        folder = button.get_current_folder()
        self.__settings.props.runtime_folder = folder

    #def __on_runtime_folder_changed(self, *args):
    #    self.__runtime_fc.set_current_folder(self.__settings.props.runtime_folder)

    def do_response(self, response):
        self.destroy()

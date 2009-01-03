import gobject
import gconf

# map gconf options to gobject properties
class Settings(gobject.GObject):
    __gproperties__ = {
        "runtime-folder": (gobject.TYPE_STRING,
            "runtime folder",
            "sclang runtime folder",
            None,
            gobject.PARAM_READWRITE),
    } # __gproperties__

    def __init__(self):
        gobject.GObject.__init__(self)

        self.base = "/apps/gedit-2/plugins/sced"
        self.client = gconf.client_get_default()
        self.client.add_dir(self.base, gconf.CLIENT_PRELOAD_NONE)

        self.client.notify_add(self.base + "/runtime_folder",
            self.__on_runtime_folder_changed)

    def do_get_property(self, property):
        if property.name == "runtime-folder":
            return self.client.get_string(self.base + "/runtime_folder")
        else:
            raise AttributeError("Unknown property %s" % property.name)

    def do_set_property(self, property, value):
        if property.name == "runtime-folder":
            self.freeze_notify()
            self.client.set_string(self.base + "/runtime_folder", value)
            self.thaw_notify()
        else:
            raise AttributeError("Unknown property %s" % property.name)

    def __on_runtime_folder_changed(self, client, cnxn_id, entry, user_data):
        self.notify("runtime-folder")

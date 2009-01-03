import gtk

# in order to get oblivion etc. colors, we may use:
#  client.get_string("/apps/gedit-2/preferences/editor/colors/scheme")
#  client = gconf.client_get_default()

class LogPanel(gtk.ScrolledWindow):
    def __init__(self):
        gtk.ScrolledWindow.__init__(self)

        self.props.shadow_type = gtk.SHADOW_IN
        self.props.hscrollbar_policy = gtk.POLICY_AUTOMATIC
        self.props.vscrollbar_policy = gtk.POLICY_AUTOMATIC

        self.buffer = gtk.TextBuffer()
        self.view = gtk.TextView(self.buffer)
        self.view.props.editable = False
        self.view.props.wrap_mode = gtk.WRAP_CHAR

        self.add(self.view)
        self.view.show()

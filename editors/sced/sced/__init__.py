import gedit
import gtk

from WindowHelper import WindowHelper
from Settings import Settings
from ConfigurationDialog import ConfigurationDialog

class ScedPlugin(gedit.Plugin):
    def __init__(self):
        gedit.Plugin.__init__(self)

        self.settings = Settings()
        self.__instances = {}

    def activate(self, window):
        self.__instances[window] = WindowHelper(self, window)

    def deactivate(self, window):
        self.__instances[window].deactivate()
        del self.__instances[window]

    def update_ui(self, window):
        self.__instances[window].update_ui()

    def create_configure_dialog(self):
        dialog = ConfigurationDialog(self)
        return dialog

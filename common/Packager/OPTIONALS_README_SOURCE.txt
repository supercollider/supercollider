INSTALLATION INSTRUCTIONS
-------------------------

QUARKS
------

Quarks are the standard method for handling the distribution of class
library extensions.

1) To install for one user move the quarks directory to
"~/share/SuperCollider". To install for all users, move the quarks
directory to "/usr/local/share/SuperCollider".
      NOTE: you might see a folder called "Extensions" 
      but DON'T put the quarks directory directly in there!

2) Follow the steps outlined inside the Quarks helpfile to actually
install a Quark.You do NOT need to download the quarks (you already
have), at this point, you should be able to start installing Quarks by
running:

Quarks.gui


SC3-PLUGINS
-----------

sc3-plugins are some of the more common extension libraries, maintained
at:

http://sourceforge.net/projects/sc3-plugins/

1) Execute "scons ./distro_linux" in the sc3-plugin-extensions folder
and move the "./sc3-plugin-extensions/Extensions" folder to
"~/share/SuperCollider/Extensions".

2) Restart SuperCollider.


SWINGOSC
--------

SwingOSC is a system for creating graphical user interfaces using Java.
To install it, follow the SwingOSC installation instructions in
"SwingOSC/SuperCollider/readme.html".

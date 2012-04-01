------------------------------------------------------------------------
Supercollider 3.5 for Windows
------------------------------------------------------------------------

This is the Windows version of James McCartney's SuperCollider synthesis 
engine (scsynth) and programming language (sclang). 
Supercollider's main homepage is at
   http://supercollider.sourceforge.net/

There is a lot of useful information and tutorials in the SuperCollider
documentation which you can access online at
    http://doc.sccode.org

To get further information on SuperCollider usage or development, you
should subscribe to the mailing lists:
   http://www.beast.bham.ac.uk/research/sc_mailing_lists.shtml

------------------------------------------------------------------------
System Requirements
------------------------------------------------------------------------

* Windows 7/Vista/XP/2000
* (Optional) Gedit text editor

The preferred way to use SuperCollider on Windows is in combination
with the Gedit text editor. SuperCollider for Windows comes with
Sced - a plugin for Gedit that provides syntax highlighting for
SuperCollider code, and allows easy interaction with SuperCollider
from within the editor.

NOTE: There is currently an issue on Windows XP that the default (local)
synthesis server can not be started from within Gedit. To work around
that, either use the internal server, or start up a server from command
line and connect to it from Gedit. This issue is not present on Windows 7.

Most common sound cards will work fine - the sound card should be one
that is supported by PortAudio (http://www.portaudio.com/). If you have
issues with latency, consult http://www.portaudio.com/docs/latency.html
for advice on how to tune your operating system to achieve the best results
(in short: use ASIO drivers wherever possible).

------------------------------------------------------------------------
Installation
------------------------------------------------------------------------

If you intend to use SuperCollider in combination with Gedit, please
install the latest Gedit version for Windows prior to installing
SuperCollider. You can download Gedit from this address:

http://ftp.gnome.org/pub/GNOME/binaries/win32/gedit/2.30/gedit-setup-2.30.1-1.exe

The SuperCollider installer will ask you whether you also want to install
Sced - the SuperCollider plugin for Gedit. If you decide so, you will be
asked to specify the directory where you have installed Gedit (the installer
will probably guess that automatically).

After installation, you can use SuperCollider in two ways:
A) From within Gedit text editor (if you chose to install Sced).
B) Via the command-line.

The two ways are described below.

------------------------------------------------------------------------
Using SuperCollider with Gedit text editor
------------------------------------------------------------------------

This is the preferred and more powerful way.

1. Start up Gedit, and first enable the Sced plugin in the preferences
dialog: "Edit > Preferences > Plugins". You only need to do this step
once. After that the plugin will stay enabled whenever you restart Gedit.

2. To start up SuperCollider, click on the menu entry
"Tools > SuperCollider Mode". This will start the interpreter and add
a new menu named "SuperCollider", which gives you many options for
further interaction with the interpreter and the sound synthesis server.

------------------------------------------------------------------------
Using SuperCollider in command line mode
------------------------------------------------------------------------

1. Open a Windows command line window (the cmd.exe program).

2. Navigate to the SuperCollider installation folder.

3. Start up the sclang.exe program, to enter the SuperCollider
command line.

4. You can now type SuperCollider code and evaluate it by pressing
the Enter (Return) key. You can browse the previous commands using
the up and down arrow keys.

5. These are the handy keyboard shortcuts:
- Ctrl+T : equivalent to CmdPeriod.run
- Ctrl+X : to recompile the class library
- Ctrl+D: to quit the SuperCollider command line

------------------------------------------------------------------------
Paths and files you may want to be aware of
------------------------------------------------------------------------

- $USER_HOME$\SuperCollider\startup.scd : user startup file

- $USER_HOME$\SuperCollider\Extensions : user extension folder for classes,
plugins and help files

------------------------------------------------------------------------
Outro
------------------------------------------------------------------------

Thanks to James McCartney, for making this great piece of audio
software publically and freely available.

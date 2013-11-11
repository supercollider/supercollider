------------------------------------------------------------------------
Supercollider 3.6 for Windows
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

** Operating System **

Windows 8/7/Vista/XP/2000

** Sound Card **

Most common sound cards will work fine - the sound card should be one
that is supported by PortAudio (http://www.portaudio.com/). If you have
issues with latency, consult http://www.portaudio.com/docs/latency.html
for advice on how to tune your operating system to achieve the best results
(in short: use ASIO drivers wherever possible).

------------------------------------------------------------------------
Installation
------------------------------------------------------------------------

Simply run the installer, and choose a path where all the application
files will be unpacked.

After installation, you can use SuperCollider in two ways, described below:
* Using the provided SuperCollider IDE (recommended)
* In command line mode

NOTE: Location of extensions and configuration files has changed in 3.6.1.
If you want to keep those from previous versions, please read the section
about changed locations below.

------------------------------------------------------------------------
Using the SuperCollider IDE
------------------------------------------------------------------------

To start up the IDE use the Startmenu link or run the scide.exe program 
found within the installation folder.

You can get more help on using the IDE via the Help menu.

On first startup expect several firewall warnings to pop up. The
SuperCollider components communicate through the network, so you will
get a warning for each oc scide, sclang, scsynth and - if available -
supernova.

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
Quarks
------------------------------------------------------------------------
Quarks are an community driven extensions system for SuperCollider. On 
OSX and Linux, SC provides a GUI to download and update Quarks. It is 
started by evaluating `Quarks.gui`. This GUI cannot be used on Windows
though. You need to download Quarks manually and make them available 
to SC. The most common way of doing this is:

1. Download Quarks with your preferred svn-client by running:

svn co https://svn.code.sf.net/p/quarks/code quarks

You can later use svn to update your quarks:

svn update

The complete collection of Quarks will be downloaded into a folder 
`quarks`at the location where you ran the command. Within that 
folder you will find many folders, one for each Quark.
The Quarks you want to install needs to be copied or "hardlinked" into
a folder where SC can find them. Usually a folder called Extensions in
the user application support directory is used for this purpose. It
is located within the folder:

C:\Users\<USERNAME>\AppData\Local\SuperCollider

See below for the variations of this location on different Windows
versions, or evaluate `Platform.userAppSupportDir` in the SC editor
and use the output in the post window.

2. If you don't find an Extensions folder, create it manually as:

YourUserAppSupportDir\Extensions

For example:

C:\Users\<USERNAME>\AppData\Local\SuperCollider\Extensions

3. Copy or "hardlink" the folder containing each Quark you want to install
into this folder (you can create a subfolder within the Extensions folder 
to bundle your Quarks there). "Hardlinking" [1] has the advantage, that on 
subsequent svn updates, SC will automatically get the updated versions.

4. Now look for a file in the folder quarks/DIRECTORY with the same name as 
your Quark (and the extension .quark). Open it in a text editor and look 
for the key \dependencies. If you find it (only some Quarks have dependencies)
add the quark(s) listed there to the mix (i.e. copy or hardlink them to your 
Extensions folder).

5. After recompiling your class library or restarting SC your Quarks should
be available.

------------------------------------------------------------------------
Uninstall
------------------------------------------------------------------------
Should you ever decide to uninstall SuperCollider, it is the easiest to
use the uninstaller executable provided in the application folder, or to 
use the uninstall service provided in the 'Add-/Remove Programs' dialogue. 
This will remove all files from the application folder, and the SC
registry settings - provided things were not changed manually since last 
install. The support files (in AppData\SuperCollider) are *not* 
removed automatically, because they usually contain user specific data
that might be useful in a subsequent install. If they bother you, you 
need to remove them manually. The AppData folder is hidden by default,
so you might need to unhide it to find your folder. In case of an update
or reinstall pay also attention to plugins residing in the support folders. 
They are minor-version specific and cause error messages if they do not 
fit the installed SuperCollider version.


------------------------------------------------------------------------
NOTE: Changed location of extensions and configuration files
------------------------------------------------------------------------

The location of application-related user data has moved in accordance
with Windows policies. This means that from version 3.6.1 on, SuperCollider
will not pick up extensions, startup file, and configuration files from
locations used by previous versions.

To continue using extensions and configuration from versions prior to 3.6.1,
please copy the contents of the old directory to the new one, given below:

The new directory:

  Windows 7 / Vista:
    C:\Users\<USERNAME>\AppData\Local\SuperCollider
  Windows XP:
    C:\Documents and Settings\<USERNAME>\Local Settings\Application Data\SuperCollider

The old directories:

  Between versions 3.6.0 and 3.6.1:
    Windows 7 / Vista:
      C:\Users\<USERNAME>\AppData\Roaming\SuperCollider
    Windows XP:
      C:\Documents and Settings\<USERNAME>\Application Data\SuperCollider

  Prior to version 3.6.0:
    Windows 7 / Vista:
      C:\Users\<USERNAME>\SuperCollider
    Windows XP:
      C:\Documents and Settings\<USERNAME>\SuperCollider

------------------------------------------------------------------------
Outro
------------------------------------------------------------------------

Thanks to James McCartney, for making this great piece of audio
software publically and freely available.

[1] http://schinagl.priv.at/nt/hardlinkshellext/hardlinkshellext.html

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

Windows 7/Vista/XP/2000

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

To start up the IDE, run the scide.exe program found within the
installation folder.

You can get more help on using the IDE via the Help menu.

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
NOTE: Changed location of extensions and configuration files
------------------------------------------------------------------------

The location of application-related user data has moved in accordance
with Windows policies. This means that from version 3.6.1 on, SuperCollider
will not pickup extensions, startup file, and configuration files from
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

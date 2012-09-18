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

After installation, you can use SuperCollider in two ways:
* Using the provided SuperCollider IDE (recommended)
* In command line mode

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

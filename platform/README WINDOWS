------------------------------------------------------------------------
PsyCollider 3.2 (Supercollider 3.2 for Windows)
------------------------------------------------------------------------

This is the Windows version of James McCartney's SuperCollider synthesis 
engine (scsynth) and programming language (sclang). 
Supercollider's main homepage is at
   http://supercollider.sourceforge.net/

To get further information on SuperCollider usage or development, you
should subscribe to the mailing lists:
   http://www.beast.bham.ac.uk/research/sc_mailing_lists.shtml

------------------------------------------------------------------------
System Requirements
------------------------------------------------------------------------

* Windows 2000/XP/Vista
* Windows Installer 

Most common sound cards will work fine - the sound card should be one 
that is supported by PortAudio (http://www.portaudio.com/). If you have 
issues with latency, consult http://www.portaudio.com/docs/latency.html for 
advice how to tune your operating system to achieve the best results 
(in short: use ASIO drivers wherever possible).

------------------------------------------------------------------------
How to install, and how to launch
------------------------------------------------------------------------
To install, simply run "setup.exe" and PsyCollider will install on your 
System (default location is Program Files\PsyCollider).

Start PsyCollider from your start menu.

NOTE: Your system must provide both audio input and audio output. Otherwise 
the synthesis server will not start (see also sc-users mailinglist discussion)

Paths and files you may want to be aware of:
$PSYCOLLIDER_HOME$\startup.sc : system wide startup file
$USER_HOME$\SuperCollider\startup.sc : user startup file
$USER_HOME$\SuperCollider\Extensions : user extension folder for classes, plugins and help files


------------------------------------------------------------------------
    Note: THE FOLLOWING INFORMATION IS FOR DEVELOPERS ONLY!
    Most people don't need to know how to build SC3 from scratch...
------------------------------------------------------------------------
Building
------------------------------------------------------------------------

To compile SuperCollider for yourself...
Check out the latest source from supercollider.sourceforge.net 

The following libraries are needed for compiling
* libsndfile (included in the sc code tree)
* portaudio
* portmidi
* pthread-Win32
* fftw3
* python

You need Visual C++ 2008 (9.0) or the Express edition to build it. 

Let's call ROOT the folder you checked out the SuperCollider code tree (contains "build", "Source" etc.)
(Note that the SuperCollider code folder should be named "SuperCollider3" to avoid problems.)

1: Python 
----------------------
1) go to "http://python.org" and download and install the latest Python version (tested 2.5.1). 
NOTE: get the Windows installer and make sure its the Unicode version and NOT the Ansii version.

2) go to "http://wxpython.org" and download and install the corresponding wxPython binary (tested 2.8.6.1)
NOTE: again you need the Unicode version.

NOTE: make sure you have an environment variable set called PYTHONPATH pointing to the base directory of you installation (Should be done by python installer).

2: Portaudio + ASIO
----------------------
- reminder : you do NOT need to build portaudio. this is taken care of by the msvc project.

1) go to "http://www.portaudio.com/" and download pa_snapshot_v19.tar.gz
(main page --> CVS (left menu) --> pa_snapshot_v19.tar.gz in the "v19-devel" branch section)

2) uncompress it in the ROOT folder. normally, the portaudio folder has the correct name "portaudio".
folders must be like that :

...
ROOT\..\portaudio\docs
ROOT\..\portaudio\src
....

3) go to steinberg.net --> support --> 3rd party Developers (menu on the right) --> ASIO SDK Download. 
If you agree on legal junk, you can enter a valid email address (please note that i do NOT know if you can safely Disagree with Steinberg sending your email address to commercial partners and receive the SDK info anyway...). 

4) once you receive the email (it took 5 minutes here), you can download asiosdk2.zip, copy and uncompress it to a temp folder. you must then copy some parts of the asio2 sdk to the PortAudio source tree.

5) unpack the zip and put it next to portaudio. 

...
ROOT\..\asio-sdk\common
ROOT\..\asio-sdk\driver
...

3: Portmidi
-----------
- reminder : you do NOT need to build portmidi. this is taken care of by the msvc project.

1) checkout version 8 from http://portmedia.svn.sourceforge.net/svnroot/portmedia/portmidi/trunk/

the folder structure needs to be like this:
...
ROOT\..\portmidi\pm_common\
ROOT\..\portmidi\portmidi.dsp\
...

4: Pthread-win32
------------------------
- reminder : you do NOT need to build pthread-win32. this is taken care of by the msvc project.

1) go to : "ftp://sources.redhat.com/pub/pthreads-win32" and get "pthreads-snap-yyyy-mm-dd.tar.gz". (known to work with "pthreads-snap-2005-01-25.tar.gz")
uncompress it and rename the folder to "pthread-win32" so that the source tree looks like :

...
ROOT\..\pthread-win32\tests
ROOT\..\pthread-win32\announce
ROOT\..\pthread-win32\attr.c
...
NOTE : the home page is "http://sources.redhat.com/pthreads-win32"


5: fftw3
-----------------------

1) go to: "http://www.fftw.org/install/windows.html" and download the "PRECOMPILED WINDOWS DLL".

...
ROOT\..\fftw3
...

You also need to run the instructions in the readme that accompany the fftw3 download, which tell you 
how to generate "lib" files for your system.


6: libsndfile
-----------------------

NOTE: THIS SECTION ABOUT LIBSNDFILE MAY BE OUT OF DATE. Libsndfile windows headers are now included by default. Please first try skipping this section, then come back if it doesn't work.

Some libsndfile files are included in the sc3 source distribution - however, you need to do a couple of things to get libsndfile stuff compiling correctly on your system:

* go to http://www.mega-nerd.com/libsndfile/#Download section and download the "Windows zip file including precompiled binaries"
* that zip file contains a readme with some instructions. Follow them.
* When done, place the following files into SuperCollider's "libsndfile" folder:

...
ROOT\xtralibs\win\libsndfile\libsndfile-1.def
ROOT\xtralibs\win\libsndfile\libsndfile-1.dll
ROOT\xtralibs\win\libsndfile\libsndfile-1.exp
ROOT\xtralibs\win\libsndfile\libsndfile-1.lib
ROOT\xtralibs\include\libsndfile\sndfile.h
...


7: actually building and running the thing!
-------------------------------------------

Once all the above is done, the Solution "SC3win.sln" should compile without problems (lots of warnings though...).
As a result you will have the plugins in build\plugins (scx files) and PySCLang in ($PYTHONHOME)\lib\site-packages

(The building of the "PySClang" target should also result in a few necessary files being copied into the "build" 
folder. Namely Psycollider.py, startup.sc, keywords.list, libsndfile.dll, libfftw3f-3.dll.)


Then open a Terminal and run: python.exe Psycollider.py 


------------------------------------------------------------------------
Environment
------------------------------------------------------------------------

Environment variables add directories to the default search path for plugins and synth definitions, respectively. Directory names are separated by ';' and must be set through the Control Panel. 

 * SC_PLUGIN_PATH, SC_SYNTHDEF_PATH

------------------------------------------------------------------------
Reporting bugs
------------------------------------------------------------------------

Please report bugs either to the sc-users or sc-dev mailing lists.

------------------------------------------------------------------------
Outro
------------------------------------------------------------------------

thanks to james mccartney, for making this great piece of audio
software publically and freely available.

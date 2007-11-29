PsyCollider - SuperCollider 3 for Windows - supercollider.sourceforge.net
-------------------------------------------------------------------------

the licenses are in separate text files in the "LICENSES" folder.
-------------------------------------------------------------------------


Welcome to Psycollider a python implementation of SuperCollider3


+---------------+
| installation  |
+---------------+

Simply run "setup.exe" and PsyCollider will install on your System (default is Program Files\PsyCollider).

After that you will have a PsyCollider.exe you can double cklick to run the program.


+-------------+
| compilation |
+-------------+

Check out the latest source from supercollider.sourceforge.net 

The following libraries are needed for compiling
* libsndfile (included in the sc code tree)
* portaudio
* portmidi
* pthread-Win32
* python

You need Visual C++ 2005 (8.0) or the Express edition to build it. 

Let's call ROOT the folder you checked out the SuperCollider code tree (contains "build", "Source" etc.)


python 
----------------------

1) go to "http://python.org" and download and install the latest Python version (tested 2.5.1). 
NOTE: get the Windows installer and make sure its the Unicode version and NOT the Ansii version.

2) go to "http://wxpython.org" and download and install the corresponding wxPython binary (tested 2.8.6.1)
NOTE: again you need the Unicode version.


portaudio + asio
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

3) go to steinberg.net --> support --> 3rd party Developers (menu on the right) --> ASIO SDK Download
if you agree on legal junk, you can enter a valid email address (please note that i do NOT know if you can 
safely Disagree with Steinberg sending your email address to commercial partners and receive the SDK 
info anyway...). 

4) once you receive the email (it took 5 minutes here), you can download asiosdk2.zip, copy and uncompress
it to a temp folder. you must then copy some parts of the asio2 sdk to the PortAudio source tree.

5) unpack the zip and put it next to portaudio. 

...
ROOT\..\asio-sdk\common
ROOT\..\asio-sdk\driver
...


portmidi
-----------
- reminder : you do NOT need to build portmidi. this is taken care of by the msvc project.

1) checkout version 8 from http://portmedia.svn.sourceforge.net/viewvc/portmedia/portmidi/trunk/

the folder structure needs to be like this:
...
ROOT\..\portmidi\pm_common\
ROOT\..\portmidi\portmidi.dsp\
...


pthread-win32
------------------------
- reminder : you do NOT need to build pthread-win32. this is taken care of by the msvc project.

1) go to : "ftp://sources.redhat.com/pub/pthreads-win32" and get "pthreads-snap-yyyy-mm-dd.tar.gz".
(known to work with "pthreads-snap-2005-01-25.tar.gz")
uncompress it and rename the folder to "pthread-win32" so that the source tree looks like :

...
ROOT\..\pthread-win32\tests
ROOT\..\pthread-win32\announce
ROOT\..\pthread-win32\attr.c
...
NOTE : the home page is "http://sources.redhat.com/pthreads-win32"


fftw3
-----------------------
- reminder : you do NOT need to build fftw3. this is taken care of by the msvc project.

1) go to: "http://www.fftw.org/download.html" and download the latest stable version (tested 3.1.)

...
ROOT\..\fftw3
...


Once all that is done, the Solution should compile without problems (lots of warning though...).
As a result you will have the plugins in build\plugins (scx files) and PySCLang in ($PYTHONHOME)\lib\site-packages

To run PsyCollider you have to put a few more things into the build directory:
* Psycollider.py (the Psycollider script from ROOT\Psycollider\Psycollider into ROOT\build\)
* aaa_WinFixesFirst (from ROOT\windows\SCClassLibrary into ROOT\build\SCCLassLibrary)
* startup.sc and keywords.list (from ROOT\windows\ into ROOT\build\)
* libsndfile.dll (from ROOT\libsndfile into ROOT\build\)

Then open a Terminal and run: python.exe Psycollider.py 


+------------+
| deployment |
+------------+

If you would like to make a PsyCollider package for others to use, you will need Visual C++ 2005 (not the Express edition)

1) compile everything as above
2) download and install py2exe (http://www.py2exe.org/)
3) open a Terminal and go to ROOT\Psycollider\Psycollider and run "python.exe setup.py py2exe"
This creates a dist folder with a PsyCollider.exe file and some dlls

4) Checkout a "clean" version of SuperCollider (svn export) so that there are not .svn folders around
...
ROOT\..\SuperCollider_Clean\
...

5) Open the PsyCollider Installer project in ROOT\windows and build the project
This creates a setup.exe and PsyCollider.msi in the Release directory.




Christopher Frauenberger 
frauenberger@iem.at

Original port by:
Benjamin Golinvaux
benjamin.golinvaux@euresys.com
messenger: bgolinvaux@hotmail.com

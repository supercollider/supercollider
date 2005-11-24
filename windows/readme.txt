SuperCollider 3 for Windows - supercollider.sourceforge.net
---------------------------------------------------------------------

the licenses are in separate text files in the "LICENSES" folder.
---------------------------------------------------------------------

+---------------+
| In this build |
+---------------+
- a port of scsynth.exe, with a GUI frontend
  to choose server options (scspawn.exe)
- a lang editor (written in python) and the python extension to make it work

+------------+
| Changelog  |
+------------+

# SC3-WIN32-2005-11-23-14h30 :
# made the post call into Python thread safe (Task postln bug)
# see http://docs.python.org/api/threads.html for details
# made unixCmd work with Win32

# SC3-WIN32-2005-11-16-14h20 :
# -------------------------------------------
# first build that is in sync with the CVS again. 
# The new crossplatform DirUtils were added and a new SC_Win32Utils.h/.cpp structure introduced
# fixed a bug with PortAudio timing - server does not report being years too late anymore 

# SC3-WIN32-2005-11-09-14h20 :
# -------------------------------------------
# added Extensions folder support for windows (HOME/SuperCollider/Extensions) for classes and help files
# added a extMainStartup.sc that provides smooth compiliation of ClassLibrary
# bugfixes in PySCLang

# SC3-WIN32-2004-10-20-21h30 :
# -------------------------------------------
# changed the name to avoid confusion with the scope window in sc osx
# CTRL+RETURN is a shortcut for eval selection
# now allows to open and save text files and open rtf files
# now displays a * sign when a text file is modified. also asks user whether to save when closing modified windows
# ALT+PERIOD now works (stops clocks and frees synths)
# added a log window where SC output is sent , instead of the DOS window
# pressing F1 now opens the help file with the same name as the selection, or the main help file is that fails
# now calls s.serverRunning = True to fix the someSynthDef.play code snippets. this is a temp hack
# choosing File -> Open Code File now opens the source code for the selected class or method
# better color scheme when highlighting braces/parentheses/brackets that are matching 
# MIDI input is now working (not tested a lot, but seems to work ok for more than just me :) )
# F2 now arrange the windows in a basic way : log windows at the bottom third, and code windows above, superimposed
# menu entry : Main.run
# menu entry : open class def (alt+j, similar to cmd+j in the cocoa app)
	# menu entry : compile library (alt+k, similar to cmd+k in the cocoa app)
# help files are converted to look good with Windows standard fonts (Helvetica -> Arial and Monaco -> Courier New)

# SC3-WIN32-2004-10-07-00h59 : first version :
# -------------------------------------------
# very basic MDI environment where SC code can be evaluated

+-------------+
| know issues |
+-------------+
- there are some error messages when starting PsyCollider. that is "normal"
  (related to some missing primitives for GUI or unix file stuff).. don't worry.
- no MIDI out (coming soon)
- no GUI classes

+---------------+
| installation  |
+---------------+
- install python 2.3 from www.python.org
  (currently http://www.python.org/ftp/python/2.3.4/Python-2.3.4.exe)

- install wxPython (currently 2.5.2) from www.wxpython.org
  (http://prdownloads.sourceforge.net/wxpython/wxPythonWIN32-2.5.2.8u-Py23.exe)

- unzip the SC folder anywhere
  in the root of that folder, there is a PySCLang.pyd file. Please copy or move
  it to c:\python23\Lib\site-packages (exact path might differ according to 
  your python version and installation options)

- you also need the following Microsoft DLLs :
    MSVCR71.DLL
    MSVCP71.DLL
    MFC71.DLL
  i haven't included them in the archive because they aren't 
  small and anyway won't change in future win32 build uploads.
  you can get them from www.dll-files.com, for example.
  please copy them to your windows system32 folder and forget
  about them :)


+------------+
| running    |
+------------+
1) first launch SCSpawn.exe which is a small front-end application
that allows you to choose cmd line params, but also provides a way to select
the audio interface (and number of i/o channels) that you wish to use
with scsynth.exe. Just click on a line of the parameters list and you will 
then be able to edit it. please note scroll bars don't work if a list of values
to choose from is too big. use mousewheel or keyboard arrow instead.
when clicking "launch sc3 and close", if all goes well, you should see a 
console ("dos window") appearing with the "SuperCollider 3 is ready..." 
messages . If this doesn't work, it's likely that there is a problem with
your sound card. PLease try to use an asio driver if available. I have successfully
tested my RME digiface using the ASIO driver (both 2 and 8 channels), and 
my laptop integrated audio and usb speakers using asio4all 
(http://michael.tippach.bei.t-online.de/asio4all/), that provides an ASIO
driver using "regular" windows audio drivers. I have NOT succeeded
in using MME devices. This is of course something i need to fix.


2) then double click on Psycollider.py : first it will perhaps ask you to locate
your SCClassLibrary folder (although it won't do that if you launch it from
within the "SuperCollider 3 for Windows" folder).

You'll then see an empty window. Choose File -> New Code Window. An empty
window appears where you can paste or type code.
Press cmd+return to execute the current selection, if any, or the current
line if there is no selection. press alt-. to stop free Synths and 
stop clocks. Please note you can collapse/expand blocks of code if they
are indented (this won't work 100% accurately, for lines containing # 
for example, since i'm cheating and using a syntax coloring algorithm that
is not designed for sc language at this point)

+------------+
| testing    |
+------------+

I have successfully tested the following code :

EVal this synthdef (remember s.initTree; has to be called first) 

(
  SynthDef("BassDrum808",
  {
  arg freq = 140, attack = 5, decay = 3;
  var ugSin, ugEnv;
  ugEnv = EnvGen.ar(Env.perc(0.001, 1, 1, -4), gate:1.0, levelScale:0.5, levelBias:0.0, timeScale:1.0, doneAction:2);
  ugSin = SinOsc.ar(freq, 0, ugEnv);
  Out.ar(0, ugSin);
  }).send(s);
);

Then try it :

Synth("BassDrum808");

you should hear something. don't worry if this doesn't sound like a 808 bassdrum 
(as long as it doesn't sound like a very distorted guitar) :)

Please note multichannel ASIO should work. This has been tested.


a forum has been set up at http://www.crm1opdracht.tk/ 
to discuss particular SC WIN32 issues and post bug 
reports or suggestions.


+-------------+
| compilation |
+-------------+

The following libraries are needed for compiling
libsndfile
portaudio
portmidi
pthread-Win32
supercollider, obviously

You need Visual C++ 7.1 to build it. I plan to create makefiles that make it possible to build SC3 with the free Microsoft compiler toolkit ($$$todo).

let's call ROOT the folder that contains the SuperCollider3 folder (which, in turn,
contains "build", "doc", "English.lproj", etc...

folders must thus be like that :

ROOT\SuperCollider3\build
ROOT\SuperCollider3\doc

IMPORTANT NOTE :
you do NOT need to build any of the support libs listes below, neither do you need
to download binary distributions of these... all compilations are taken care of by the msvc projects.

libsndfile
-----------
- reminder : you do NOT need to build libsndfile. this is taken care of by the msvc project.

1) go to "http://www.mega-nerd.com/libsndfile/" and download the .tar.gz package
($$$todo) sc3 is known to build with libsndfile-1.0.11.tar.gz. please report issues 
(or better, fix them) with newer versions.

2) uncompress it in ROOT and rename its libsndfile-x.y.zz (eg : libsndfile-1.0.11)  folder to "libsndfile". so that the folders are like that :

...
ROOT\libsndfile\doc
ROOT\libsndfile\examples
....

3) copy "ROOT\libsndfile\Win32\sndfile.h"  and "ROOT\libsndfile\Win32\config.h"
to the "ROOT\libsndfile\src\" folder

portaudio + asio
----------------------
- reminder : you do NOT need to build portaudio. this is taken care of by the msvc project.

1) go to "http://www.portaudio.com/" and download pa_snapshot_v19.tar.gz
(main page --> CVS (left menu) --> pa_snapshot_v19.tar.gz in the "v19-devel" branch section)

2) uncompress it in the ROOT folder. normally, the portaudio folder has the correct name "portaudio".
folders must be like that :

...
ROOT\portaudio\docs
ROOT\portaudio\pa_asio
....

3) go to steinberg.net --> support --> 3rd party Developers (menu on the right) --> ASIO SDK Download
if you agree on legal junk, you can enter a valid email address (please note that i do NOT know if you can 
safely Disagree with Steinberg sending your email address to commercial partners and receive the SDK 
info anyway...). 

4) once you receive the email (it took 5 minutes here), you can download asiosdk2.zip, copy and uncompress
it to a temp folder. you must then copy some parts of the asio2 sdk to the PortAudio source tree.

5) the "asiosdk2.zip\asiosdk2\host" folder must be copied to "ROOT\portaudio\pa_win\msvc\"

so that the source tree looks like that :

...
ROOT\portaudio\pa_win\msvc\host\ASIOConvertSamples.cpp"
...

6) the "asiosdk2.zip\asiosdk2\common" folder must be copied to "ROOT\portaudio\pa_win\msvc\"

so that the source tree looks like that :

...
ROOT\portaudio\pa_win\msvc\common\asiosys.h"
...

portmidi
-----------
- reminder : you do NOT need to build portmidi. this is taken care of by the msvc project.

1) go to the "http://www-2.cs.cmu.edu/~music/portmusic/portmidi/" web folder
retrieve the latest version (known to work with portmidi17nov04.zip )

2) extract the zip file to ROOT so that folders look like that :

...
ROOT\portmidi\pm_common\
ROOT\portmidi\portmidi.dsp\
...



pthread-win32
------------------------
- reminder : you do NOT need to build pthread-win32. this is taken care of by the msvc project.

1) go to : "ftp://sources.redhat.com/pub/pthreads-win32" and get "pthreads-snap-yyyy-mm-dd.tar.gz".
(known to work with "pthreads-snap-2005-01-25.tar.gz")
uncompress it to ROOT, rename the folder to "pthread-win32" so that the source tree looks like :

...
ROOT\pthread-win32\tests
ROOT\pthread-win32\announce
ROOT\pthread-win32\attr.c
...


NOTE : the home page is "http://sources.redhat.com/pthreads-win32"


once all these steps are done, all projects should build correctly. if they fail for some reason (change to SC code that i haven't caught or 
update to the support libs), please let me know at my email address (and maybe on the sc-dev mailing too, but please make sure to send an email) :

Benjamin Golinvaux
benjamin.golinvaux@euresys.com
messenger: bgolinvaux@hotmail.com

Christopher Frauenberger 
frauenberger@iem.at

or simply to the develope list, if you are subscribed
sc-dev@create.ucsb.edu



Win32 build how-to
-------------------------

You need Visual C++ 7.1 to build it. I plan to create makefiles that make it possible
to build SC3 with the free Microsoft compiler toolkit ($$$todo).

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


----------------------------------------------------------------------------------------------------
----------------------------------------------------------------------------------------------------
----------------------------------------------------------------------------------------------------
----------------------------------------------------------------------------------------------------

once all these steps are done, all projects should build correctly.
if they fail for some reason (change to SC code that i haven't caught or 
update to the support libs), please let me know at my email address (and
maybe on the sc-dev mailing too, but please make sure to send an email) :
benjamin.golinvaux@euresys.com

thanks

----------------------------------------------------------------------------------------------------
----------------------------------------------------------------------------------------------------
----------------------------------------------------------------------------------------------------
----------------------------------------------------------------------------------------------------
----------------------------------------------------------------------------------------------------



prerequisites :

http://sources.redhat.com/pthreads-win32/

("change in config.h to "#define HAVE_C_INLINE"??" : no, we're not using config file.. maybe have a look at using __inline for INLINE)

http://www.mega-nerd.com/libsndfile/

copy sndfile.h and config.h from the Win32 subfolder to the upper src folder


rename portaudio folder to "portaudio"

rename other folders too ($$$todo)

download the ASIO sdk from ...

copier le SDK asio là où le fichier portaudio/pa_win/msvc/readme.txt l'explique


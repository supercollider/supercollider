
set QT_HOME=C:\Qt\5.4
set PA_HOME=C:\portaudio

set CMAKE_PREFIX_PATH=%QT_HOME%\msvc2013_64;%PA_HOME%

set CMAKE_LIBRARY_ARCHITECTURE=x64

set SNDFILE_INCLUDE_DIRS="C:\Program Files\Mega-Nerd\libsndfile\include"
set SNDFILE_LIBRARIES=C:\Program Files\Mega-Nerd\libsndfile\lib\libsndfile-1.lib"

mkdir build
cd build

cmake -G "Visual Studio 12 2013 Win64" ..\..\.

cd ..

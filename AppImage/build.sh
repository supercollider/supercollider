#!/bin/bash

#exit on error
set -e

# this should be run on a build dir
# i.e. mkdir build

cmake .. -DCMAKE_INSTALL_PREFIX=/usr -DLINUX_APPIMAGE=ON -DBUILD_TESTING=OFF ..
make "-j$(nproc)"
make install DESTDIR=AppDir

wget https://github.com/linuxdeploy/linuxdeploy/releases/download/continuous/linuxdeploy-x86_64.AppImage
chmod +x linuxdeploy-x86_64.AppImage

wget https://github.com/linuxdeploy/linuxdeploy-plugin-qt/releases/download/continuous/linuxdeploy-plugin-qt-x86_64.AppImage
chmod u+x linuxdeploy-plugin-qt-x86_64.AppImage

export OUTPUT=SuperCollider.AppImage
./linuxdeploy-x86_64.AppImage --appdir AppDir --output appimage --custom-apprun=../AppImage/AppRun --plugin qt

mkdir SuperCollider.AppImage.home
tar cvzf SuperCollider.AppImage.tar.gz SuperCollider.AppImage SuperCollider.AppImage.home

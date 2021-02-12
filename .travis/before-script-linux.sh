#!/bin/bash

$TRAVIS_BUILD_DIR/.travis/lint.sh $TRAVIS_BUILD_DIR

EXTRA_CMAKE_FLAGS=

if $USE_SYSLIBS; then
    EXTRA_CMAKE_FLAGS="-DSYSTEM_BOOST=ON -DSYSTEM_YAMLCPP=ON"
fi

if $SHARED_LIBSCSYNTH; then
    EXTRA_CMAKE_FLAGS="-DLIBSCSYNTH=ON $EXTRA_CMAKE_FLAGS"
fi

if [[ "$CLANG_VERSION" =~ ^[456].0$ ]]; then
    echo "using clang with libstdc++ instead of libc++"
    EXTRA_CMAKE_FLAGS="-DSC_CLANG_USES_LIBSTDCPP=ON $EXTRA_CMAKE_FLAGS"
fi

if [[ "$CLANG_VERSION" == "4.0" ]]; then
    echo "turning off Ableton link due to 'linux' predef issue"
    # clang 4.0 predefines the token "linux" as 1, but Ableton has a 'namespace linux' which
    # is broken by that. Undefining the token from our own header does not fix it.
    EXTRA_CMAKE_FLAGS="-DSC_ABLETON_LINK=OFF $EXTRA_CMAKE_FLAGS"
fi

if [[ "$1" != "--qt=true" ]]; then
    EXTRA_CMAKE_FLAGS="-DSC_QT=OFF -DSC_IDE=OFF $EXTRA_CMAKE_FLAGS"
else
    source /opt/qt514/bin/qt514-env.sh
fi

cmake $EXTRA_CMAKE_FLAGS -DSC_EL=ON -DSC_VIM=ON -DSC_ED=ON -DCMAKE_INSTALL_PREFIX:PATH=$TRAVIS_BUILD_DIR/BUILD/Install -DCMAKE_BUILD_TYPE=Release $TRAVIS_BUILD_DIR --debug-output

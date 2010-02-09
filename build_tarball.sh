#!/bin/bash

VERSION="$1"
TARBALL=$VERSION.tar

bash ./git-archive-all.sh --prefix $VERSION/ $TARBALL

# we only need one instance of boost
tar -f $TARBALL --delete $VERSION/libs/nova-tt/boost_lockfree/
tar -f $TARBALL --delete $VERSION/source/sc/sc3-plugins

bzip2 -k --best $TARBALL &
lzma  -k --best $TARBALL &

wait
rm $TARBALL
[![Build Status](https://travis-ci.org/supercollider/supercollider.svg?branch=master)](https://travis-ci.org/supercollider/supercollider)

Welcome to SuperCollider!
=========================

**SuperCollider** is a platform for audio synthesis and algorithmic composition, used by musicians, artists, and researchers working with sound. It is free and open source software available for Windows, Mac OS X, and Linux.

SuperCollider features three major components:

- scsynth, a real-time audio server, forms the core of the platform. It features 400+ unit generators ("UGens") for analysis, synthesis, and processing. Some of the audio techniques it supports include additive synthesis, subtractive, FM, granular, FFT, and physical modelling. You can write your own UGens in C++, and users have already contributed several hundred more to the sc3-plugins repository.
- sclang, an interpreted programming language, controls scsynth via Open Sound Control. You can use sclang for algorithmic sequencing, connecting your app to external hardware including MIDI controllers, or writing GUIs and visual displays. sclang has a stock of user-contributed extensions called Quarks.
- scide is an editor for sclang with an integrated help system.

SuperCollider was developed by James McCartney and originally released in 1996. In 2002, he generously released it as free software under the GNU General Public License. It is now maintained and developed by an active and enthusiastic community.

http://supercollider.github.io/

This README is a first help for anyone new to SuperCollider. There are many
good resources available for learning SuperCollider, with up to date links from
the site listed above, including links to forums and mailing lists.


Usage
-----

SuperCollider consists of three separate components:

1. scsynth or supernova - audio engine (the "server")
2. sclang - programming language runtime interpreter including Qt graphical user interfaces
3. IDE (integrated development environment) - an editor for writing code and running supercollider

To begin using SuperCollider, you usually start up the IDE:

- on Mac OS this is `SuperCollider.app`
- on Linux and Windows, this is the `scide` executable

You can get further help by using the IDE's integrated help
system which can be invoked via the "Help" menu or using the key combination
`Ctrl+D` (on Linux, Windows) or `Cmd+D` (on Mac OS). In the Help menu, you will
also find an action that will take you directly to the help page on using the
IDE.

When starting the SuperCollider IDE, the audio server is not started
automatically. You can start it using the "Language > Boot Server" menu action,
or using the key combination `Ctrl+B` (on Linux, Windows) or `Cmd+B` (on Mac OS).
This is just to remind you in case you're impatient and can't understand why
you're not immediately getting sound.

We suggest you to proceed by reading the tutorials available in the help
system. Please note that some help pages are not up-to-date with the latest
development of SuperCollider, especially when mentioning the code editing
environment. Regarding this, it is best to refer to the help page specifically
about the new SuperCollider IDE.

Also, please explore the IDE menus, which will let you discover a lot of
functionality of the coding environment.

To get further information on SuperCollider usage or development, you should subscribe
to the mailing lists:

http://www.birmingham.ac.uk/facilities/ea-studios/research/supercollider/mailinglist.aspx

We hope you enjoy SuperCollider; please get involved in the active community!


Reporting bugs
--------------

Please report bugs to the github issue tracker or discuss on the sc-users mailing list.

https://github.com/supercollider/supercollider/issues


Building the Source Code
------------------------

Platform specific build and install instructions can be found in the following files:

 - [README_OS_X.md](README_OS_X.md)
 - [README_LINUX.md](README_LINUX.md)
 - [README_WINDOWS.md](README_WINDOWS.md)
 - [README_IPHONE.md](README_IPHONE.md)
 - [README_JAILBROKEN_IPHONE.md](README_JAILBROKEN_IPHONE.md)


License
-------

SuperCollider is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version. See [COPYING](COPYING) for the license text.

- SuperCollider 3.1 was released on October 31, 2007
- SuperCollider 3.2 was released on February 19, 2008
- SuperCollider 3.3 was released on April 30, 2009
- SuperCollider 3.4 was released on July 15, 2010
- SuperCollider 3.5 was released on March 16, 2012
- SuperCollider 3.6 was released on November 28, 2012
- SuperCollider 3.7 was released on March 13, 2016

Outro
-----

Thanks to James McCartney, for making this great piece of audio
software publicly and freely available!

Welcome to SuperCollider 3!
***************************

SuperCollider is an environment and programming language for real time audio synthesis and algorithmic composition. It provides an interpreted object-oriented language which functions as a network client to a state of the art, realtime sound synthesis server.

SuperCollider was written by James McCartney (audiosynth.com) over a period of many years. It is now an open source GPL'd project maintained and developed by James and various others. It is used by musicians, scientists, and artists working with sound.

http://supercollider.sourceforge.net/


This README is a first help for anyone new to SuperCollider. There are many good resources available for learning SuperCollider, with up to date links from the site listed above, including links to forums and mailing lists. Within the SuperCollider application directory you will find a Help directory containing HTML files which you can browse in a web browser or within SuperCollider itself.

The exact details of using the SuperCollider environment will depend on your platform. Some immediate comments:

1) SuperCollider is really two applications: a sound synthesiser (the Server), and a programming language (the Lang) with its own developer environment. You can use both of these or one or the other. When using SC (SuperCollider) from within the developer environment SC Lang, the SC Server is used automatically for sound synthesis as long as it's turned on! This is accomplished by pressing the 'Boot' button on a Server graphic or running the code s.boot. Further details are in the help tutorials; but we're just warning you now in case you're impatient and can't understand why you're not immediately getting sounds.

2) The Main Help page in SuperCollider (Help/Help.html) gives some immediate key commands and should be enough to help you start navigating the help system. Please also check the different menu options available with the application on your system.

Platform specific comments:

[OS X]  To execute a line in OS X, put the cursor on a line and press the Enter key; this is distinct from the Return key!

And be careful you're not using TextEdit or a web browser instead of the SuperCollider application when you try to execute code; SC's text editor interface is a lot like TextEdit, but only the SC editor can run code when you hit Enter.


[Windows] The windows version of SuperCollider is called PsyCollider. Key commands differ from the Mac, but almost all code should work the same. The SwingOSC Java based GUI library is used here, however, for GUIs. You must use JMouseX/JMouseY in particular; PsyCollider does not support MouseX/MouseY UGens.


[Linux] Most people use SuperCollider from emacs; there are some alternative editors available. For GUI building, the SwingOSC Java framework is a standard option. For more information on Linux specific issues, check the "platform/README LINUX" file included in the source package, and the README in the folder editors/scel for information on the emacs editor.


Building the Source Code
========================

Platform specific build and install instructions can be found in the README_${PLATOFORM}.txt files

We hope you enjoy SuperCollider; please get involved in the active community!


License
=======

SuperCollider is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version. See "COPYING" for the license text.



SuperCollider 3.1 was released on October 31, 2007
SuperCollider 3.2 was released on February 19, 2008
SuperCollider 3.3 was released on April 30, 2009
SuperCollider 3.4 was released on July 15, 2010

Sced
====

This is Sced, a SuperCollider interaction plugin for gedit.

Sced aims to simplify SuperCollider experience on Linux systems as far as possible.


Installation
------------

To install sced via cmake, configure the build directory with
```
$> cmake -DSC_ED=ON
```

This will install sced for the most recent version of gedit available on your platform (version 3 on Linux and Mac OS X, and version 2 on Windows).

If you specifically want to install sced for gedit version 2 instead of 3,
configure the build directory with
```
$> cmake -DSC_ED=2
```

When installing into `/usr/local` (which is the default), gedit won't find
the plugin since it only looks in `/usr`. To fix this, simply symlink
the plugin-files to `~/.local/share/gedit/plugins`:

```
$> mkdir -p ~/.local/share/gedit/plugins
$> cd ~/.local/share/gedit/plugins
$> ln -sf /usr/local/lib/gedit/plugins/supercollider.plugin .
$> ln -sf /usr/local/lib/gedit/plugins/supercollider.py .
```


OSX
---

Sced works fine for gedit3 on OSX, if you follow these instructions:

Copy or symlink `supercollider.plugin` and `supercollider.py` from
`<build>/Install/lib/gedit/plugins` into `~/.local/share/gedit/plugins`

Place wrapper-scripts for sclang and scsynth into `/usr/local/bin`.

 - for sclang:

```
#!/bin/sh
cd /full/path/to/SuperCollider.app/Contents/MacOS
exec ./sclang $*
```

 - for scsynth:

```
#!/bin/sh
cd /full/path/to/SuperCollider.app/Contents/Resources
exec ./scsynth $*
```

You need to make the scripts executable:

```
$> chmod +x sclang
$> chmod +x scsynth
```


Usage
-----

To enable sced, open gedit's preferences and go to the Plugins tab and
check the SuperCollider plugin checkbox. You only need to do this once.

When sced is enabled, you activate it with the menu `Tools>SuperCollider` mode.

The menu items describe themselves. You're going to use the `Ctrl+E` shortcut
a lot. By default, this command evaluates the current line of code. If you
select several lines, hitting Ctrl+E will evaluate them all.

You can also select and run a ()-block. Just place the cursor on the opening
bracket and press `Ctrl+E`, i. e.:

```
( // press Ctrl+E here
"hello".postln;
"world".postln;
);
```

Sced is released under the GNU General Public License (GPL) version 3, see
the file 'COPYING' for more information.

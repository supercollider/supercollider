Scvim
=====

Created by Alex Norman, it is in its early stages, use at your own risk
This is SCVIM-git
The webpage is here: http://www.x37v.info/projects/scvim
If you have improvements/suggestions email `alex at x37v dot info`


Requirements
------------

- vim
- supercollider (sclang)
- ruby (sclangpipe_app, scvim)
- cmake
- pidof (for killing spawned processes, when closing)

### Optional:

- gvim or MacVim


Installation steps
------------------

Note: If you installed SuperCollider from source and didn't disable scvim when running cmake, scvim should already be installed.

If you'd like to install scvim directly, without using the whole SuperCollider install process you can do:

```
$> cmake . && sudo make install
```

You will have to enter your password in order to finalize the installation.

Some people prefer to use a build directory [though scvim doesn't actually build anything, but cmake does create some output files] to do that, from the scvim directory, type:

```
$> mkdir build && cd build
$> cmake .. && sudo make install
```

Now you should be ready to use scvim, e.g. by running `scvim` or `scvim -g` from a command prompt.


Optional extra installation steps
---------------------------------

* If you want to be able to read supercollider files with syntax highlighting, 
have the ability to start the language and have the keybindings you've set
up during a normal vim editing session [without launching as 'scvim'] then
source your scvimrc file in your `~/.vimrc`.

For example, if you have a `~/.scvimrc` file you could put this line in your `~/.vimrc`:

   `so ~/.scvimrc`

* I put this in my `~/.config/SuperCollider/startup.scd` so that each time I start sclang I have updated
Object completion and highlighting, as well as Object Definition Lookup:

  ```
  //set up the scvim stuff
  SCVim.updateCaches;
  ```

You'll probably want to execute this at least once after an install of
supercollider to get the most up-to-date Object info

* sclang's working directory defaults to /tmp on linux and
/Applications/SuperCollider on mac when using scvim. If you want to
set it to something else you can set the bash environment variable
`SCLANG_RUNDIR`

* I've put the colorscheme that I use in the "extra" folder. This makes
`strings, symbols and chars variations on the same color but all look
different.  Feel free to use/modify it to your liking.


Usage
-----

Defaults (most work in both command and insert mode):

`:SClangStart` starts/restarts the interpreter in an xterm
(if you want to recompile the supercollider library just type `:SClangStart` and you'll
kill the current interpreter, start up a new one and have a recompiled library)
`:SClangKill` kills the xterm/interpreter (exiting vim also kills the interpreter)
`:SCresize [-]N` resizes the sclang output if scvim is used from within terminal multiplexers like tmux or screen by N lines

`F1` calls the SuperCollider Help on the keyword under the current cursor
`F5` sends a whole block (looks for the outermost parens and sends that data)
`F6` sends a single line (the line the cursor is currently on)
`F7` calls `TempoClock.default.clear;`
`F8` calls `s.freeAll;`
`F12` calls `thisProcess.stop;` (i.e. the same as `cmd-.` on Mac)

In visual mode `F5` and `F6` both send the whole visual block

Use `:SChelp` to get to SC help topics, or use `K` on a word (in command mode)
   (this has tab completion)
use `:SCdef` to see the class definitions for an object, or use `^k` (in command or normal mode)
on a word (this also has tab completion)

These key bindings can all be changed through editing your scvimrc file.


Help docs
---------

`scvim` uses preprocessed versions of the help files (converted to plain-text),
and to update these help files from the main files you can run the following
line in sclang:

  ```
  SCVim.updateHelpCache;
  ```


Troubleshooting
---------------

If you cannot find the scvim executable, make sure that `/usr/local/bin` is
in your PATH, this is where scvim is installed by default.

If you get errors about _FileWrite (like the error printed below) when you run
`SCVim.updateHelpCache` or `SCVim.updateCaches` it is likely that you don't have
write permissions for your scvim_cache_dir.
This directory is normally: `~/.scvim`
You can verify that by executing: `SCVIM.scvim_cache_dir`
Most often the solution is to do this: `sudo chown -R <yourusername> ~/.scvim`

```
ERROR: Primitive '_FileWrite' failed.
Failed.
RECEIVER:
Instance of File { (B7837DD0, gc=40, fmt=00, flg=00, set=01)
 instance variables [1]
 fileptr : nil
}
```


Thanks
------

- Dan Stowell for his help:
Converting the SuperCollider code into a class, improving the README,
getting scvim into the SuperCollider svn, etc.

- John Yates for providing the indent functionality [which I have changed slightly] and help with the help file processor

- Andrzej Kopec for work on removing html from help

- Renick Bell for testing out the install process and providing notes.

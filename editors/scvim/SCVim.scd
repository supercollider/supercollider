SCVim   SuperCollider in the VIM editor.

created by Alex Norman, SuperCollider class created by Dan Stowell
the webpage is here: http://www.x37v.info/scvim/
if you have improvements/suggestions email alex at x37v dot info

View the scvim README for installation instructions.
________________________________________________________________________________

Quick intro:

In order for syntax highlighting and class definition lookup to work you must
run:

SCVim.updateCaches;
	
it is best to put this in your ~/.sclang.sc file.  But for now you can just
put the cursor over that line and hit F6 to send it to the language.
After you've done that you can update the syntax highlighting info by executing
the following (in vim):
runtime syntax/supercollider.vim

________________________________________________________________________________

USAGE (inside vim):

defaults (most work in both command and insert mode): 

:SClangStart starts/restarts the interpreter in an xterm
   (if you want to recompile the supercollider library just type :SClangStart
   and you'll kill the current interpreter, start up a new one and have a
   recompiled library)
:SClangKill   kills the xterm/interpreter
(exiting vim also kills the interpreter)

F5 sends a whole block (looks for the outermost parens and sends that data)
F6 sends a single line (the line the cursor is currently on)
F7 calls TempoClock.default.clear;
F8 calls s.freeAll;
F12 calls thisProcess.stop; (i.e. the same as cmd-. on Mac)

in visual mode F5 and F6 both send the whole visual block

use :SChelp to get to SC help topics, or use K on a word (in command mode)
   (this has tab completion)
use :SCdef to see the class definitions for an object, or use ^k (in command or normal mode)
   on a word (this also has tab completion)

These key bindings can all be changed through editing your scvimrc file.

________________________________________________________________________________

HELP DOCS:

scvim uses preprocessed versions of the help files (converted to plain-text), 
and to update these help files from the main files you can run the following
line in sclang (put the cursor on line and hit F6):

  SCVim.updateHelpCache

This actually calls a Ruby script called scvim_make_help.rb, so can be used 
outside SC if desired. You can do something like:

  ./scvim_make_help.rb -c -s /usr/local/share/SuperCollider/Help

This will delete any help docs that are in your ~/.scvim/docs dir, and copy in the
more up to date [hopefully] docs from /usr/local/share/SuperCollider/Help to
that dir [after they are formatted].  If you have help docs in more than one
location you can now copy in more help docs by doing

  ./scvim_make_help -s <other location>

or you can do it all in one go by specifying multiple sources

  ./scvim_make_help -c -s /usr/local/share/SuperCollider/Help -s <other location>


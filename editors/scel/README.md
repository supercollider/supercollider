Scel
====

SuperCollider/Emacs interface


Installation requirements
-------------------------

For the HTML help system, you will need emacs-w3m support.


Installation (default)
----------------------

By default emacs-lisp files are installed in

`$prefix/share/emacs/site-lisp`

SuperCollider files are put in

`$prefix/share/SuperCollider/Extensions/scel`

The only thing you need to do is loading the sclang interface in your `~/.emacs`:

```
(require 'sclang)
```

For the HTML help system to fully function also add
```
(require 'w3m)
```


Installation (detailed)
-----------------------

Put all `*.el` files in emacs' load-path. e.g. if you put them in
`~/emacs/`, add the following lines to `~/.emacs` (or whatever your init
file is called):

```
(add-to-list 'load-path "~/emacs")
(require 'sclang)
```

for the HTML help system to fully function also add
```
(require 'w3m)
```

now put all `*.sc` files in sclang's library path, e.g. if you put them
in a non-standard location, such as `~/SuperCollider/Emacs`, add the
following to `~/.config/SuperCollider/sclang.conf.yaml`:

```
includePaths:
    [~/SuperCollider/Emacs]
```

(note normally this is not needed as they are put into sclang's library
path during installation with scons).


Usage
-----

In order to automatically start sclang when invoking emacs, use the following command line:

```
$> emacs -sclang
```

you're now ready to edit, inspect and execute sclang code!


Getting help
------------

Inside an sclang-mode buffer (e.g. by editing a .sc file), execute

`C-h m`

and a window with key bindings in sclang-mode will pop up.

`C-x C-h` lets you search for a help file

`C-M-h` opens or switches to the Help browser (if no Help file has been opened, the default Help file will be opened).

`E` copies the buffer, puts it in text mode and sclang-minor-mode, to enable you to edit the code parts to try out variations of the provided code in the help file. With `C-M-h` you can then return to the Help browser and browse further from the Help file.

`C-c C-e` allows you to edit the source of the HTML file, for example if you want to improve it and commit it to the repository.

To enable moving around in the help file with arrow keys add the following
in your `~/.emacs`:

```
(eval-after-load "w3m"
 '(progn
 (define-key w3m-mode-map [left] 'backward-char)
 (define-key w3m-mode-map [right] 'forward-char)
 (define-key w3m-mode-map [up] 'previous-line)
 (define-key w3m-mode-map [down] 'next-line)))
```

This ensures that the arrow keys are just for moving through the document, and not from hyperlink to hyperlink, which is the default in w3m-mode.


Customization
-------------

To fine-tune the installation from within emacs' graphical customization interface, type:

`M-x sclang-customize`

In particular, you will have to customize `sclang-runtime-directory'.


Server control
--------------

In the post buffer window, right-click on the server name; by default the two servers `internal` and `localhost` are available. You will get a menu with common server control operations.

To select another server, step through the server list by left-clicking on the server name.

Servers instantiated from the language will automatically be available
in the mode line.
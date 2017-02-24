#!/bin/bash
# USE: symlinks.sh create <target> <link>
# USE: symlinks.sh remove <link>
# For both, link and target, the parent folder of the folder group SCClassLibrary, HelpSource, examples and sounds has to be given
# rm -r does *not* remove the files contained in the target dir ;). This seems to be a peculiarity of msys2 symlink support

if [ "$1" = "create" ]; then
  if [ ! -e "$3/SCClassLibrary" ]; then ln -s "$2/SCClassLibrary" "$3/SCClassLibrary"; fi
  if [ ! -e "$3/HelpSource" ]; then ln -s "$2/HelpSource" "$3/HelpSource"; fi
  if [ ! -e "$3/examples" ]; then ln -s "$2/examples" "$3/examples"; fi
  if [ ! -e "$3/sounds" ]; then ln -s "$2/sounds" "$3/sounds"; fi
elif [ "$1" = "remove" ]; then
  if [ -e "$2/SCClassLibrary" ]; then rm -r "$2/SCClassLibrary"; fi
  if [ -e "$2/HelpSource" ]; then ln rm -r "$2/HelpSource"; fi
  if [ -e "$2/examples" ]; then ln rm -r "$2/examples"; fi
  if [ -e "$2/sounds" ]; then ln rm -r "$2/sounds"; fi
else
  echo 'ERROR: First argument must be "create" or "remove"';
fi

#!/bin/bash
# USE: junctions.sh create <target> <link>
# USE: junctions.sh remove <link>
# For both, link and target, the parent folder of the folder group SCClassLibrary, HelpSource, examples and sounds has to be given
# rmdir does *not* remove the files contained in the target dir. It's the "correct" way to delete junctions in Windows.

if [ "$1" = "create" ]; then
  if [ ! -e "$3/SCClassLibrary" ]; then cmd /c "mklink /J \"$3/SCClassLibrary\" \"$2/SCClassLibrary\""; fi
  if [ ! -e "$3/HelpSource" ]; then cmd /c "mklink /J \"$3/HelpSource\" \"$2/HelpSource\""; fi
  if [ ! -e "$3/examples" ]; then cmd /c "mklink /J \"$3/examples\" \"$2/examples\""; fi
  if [ ! -e "$3/sounds" ]; then cmd /c "mklink /J \"$3/sounds\" \"$2/sounds\""; fi
elif [ "$1" = "remove" ]; then
  if [ -e "$2/SCClassLibrary" ]; then cmd /c "rmdir \"$2/SCClassLibrary\""; fi
  if [ -e "$2/HelpSource" ]; then cmd /c "rmdir \"$2/HelpSource\""; fi
  if [ -e "$2/examples" ]; then cmd /c "rmdir \"$2/examples\""; fi
  if [ -e "$2/sounds" ]; then cmd /c "rmdir \"$2/sounds\""; fi
else
  echo 'ERROR: First argument must be "create" or "remove"';
fi

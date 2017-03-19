# How to take care of Boost libraries in this repo

The SuperCollider source repository packages many boost headers.
To update boost, run `extract_boost.sh`, found in this directory,
from the root directory of this repository. See that file for
full details.

Some patches have been made by SuperCollider devs to fix compiler
errors and improve performance. The patch `boost_sc_changes.patch`
has these changes and may be git-apply'd when updating boost.
See the diffs in that file for the exact details on what has
changed. Here is a quick summary:

- A patch for boost.thread that resolves an internal compiler
  error with gcc 6.2.

- Inlining of several functions in boost.chrono for performance
  enhancement. 

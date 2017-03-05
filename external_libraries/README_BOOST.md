# How to take care of Boost libraries in this repo

The SuperCollider source repository packages many boost headers.
To update boost, run `extract_boost.sh`, found in this directory,
from the root directory of this repository. See that file for
full details.

Some patches have been made by SuperCollider devs to fix compiler
errors and improve performance. Any future updates to boost
should take these patches into consideration. See commits 989446ed 
and 294927ba for the exact changes. Here is a quick summary:

- A patch for boost.thread that resolves an internal compiler
  error with gcc 6.2.

- Inlining of several functions in boost.chrono for performance
  enhancement. 

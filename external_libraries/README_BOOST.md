# How to take care of Boost libraries in this repo

The SuperCollider source repository packages many boost headers.
To update boost, run `extract_boost.sh`, found in this directory,
from the root directory of this repository. See that file for
full details.

The necessary `bcp` binary is part of the boost distribution
and needs to be built.

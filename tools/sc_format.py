"""Formats the sclang files given, in place, with sc_format, for pre-commit.

sc_format is built with SuperCollider (target sc_format_cli). This uses the one in $SC_FORMAT, else the newest in a
build directory of this repository, else the one on the PATH. Without one it does nothing, and the ctest
langutils.sc_format_repo_check catches what it would have.
"""

import glob
import os
import shutil
import subprocess
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))


def find_sc_format():
    if os.environ.get("SC_FORMAT"):
        return os.environ["SC_FORMAT"]
    names = ("sc_format", "sc_format.exe")
    built = [
        path
        for pattern in ("build*/langutils/sc_format/*", "build*/langutils/sc_format/*/*")
        for path in glob.glob(os.path.join(ROOT, pattern))
        if os.path.basename(path) in names and os.path.isfile(path) and os.access(path, os.X_OK)
    ]
    if built:
        return max(built, key=os.path.getmtime)
    return shutil.which("sc_format")


def main(files):
    sc_format = find_sc_format()
    if not sc_format:
        print("sc_format not found, so sclang files weren't formatted: build the sc_format_cli target or set SC_FORMAT")
        return 0
    return subprocess.call([sc_format, "-i", "--allow-unformatted", *files])


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))

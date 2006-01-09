# setup.py for use with py2exe to create a Psycollider.exe
#
# created: 10.12.2005
#
# commandline is: python setup.py py2exe -b1 -O2 -c

from distutils.core import setup
import py2exe

setup(
    version = "0.1",
    description = "Psycollider, the SuperCollider3 Lang implementation on Windows",
    name = "Psycollider",
    zipfile=None,
    # needs to be console to prevent empty cmd-windows for scsynth...
    console = [ 
        { 
            "script": "Psycollider.py", 
            "icon_resources": [(1, "Psycollider.ico")] 
        } 
    ], 
    )

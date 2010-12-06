# setup.py for use with py2exe to create a Psycollider.exe
#
# created: 10.12.2005
#
# commandline is: python setup.py py2exe
#
# NOTE:	you must copy the PySCLang module into the dist directory manually as it is excluded
# 	this is due to a problem with py2exe when using option bundle_files 1

from distutils.core import setup
import py2exe

setup(
    version = "3.3 alpha",
    description = "SuperCollider 3 for Windows (using Psycollider)",
    name = "SuperCollider",
    zipfile=None,
    # needs to be console to prevent empty cmd-windows for scsynth...
    windows = [
        {
            "script": "Psycollider.py",
            "icon_resources": [(1, "Psycollider.ico")]
        }
    ],
    # options for py2exe
    options = {"py2exe": {
    		       "compressed": 0,
                       "optimize": 2,
		       "bundle_files": 1,
		       #"excludes": ["PySCLang"],
                       "dll_excludes": ["MSVCP90.dll", "MSVCP80.dll", "MSVCR80.dll", "libsndfile-1.dll"]
                       }
    }
)

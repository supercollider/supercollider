from distutils.core import setup
import py2exe

setup(
    version = "0.1",
    description = "Psycollider, the SuperCollider3 Lang implementation on Windows",
    name = "Psycollider",

    # targets to build
    console = ["Psycollider.py"],
    )

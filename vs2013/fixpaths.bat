::
:: This one does a find and replace of __SC_GITHUB_HOME__ in a
:: bunch of files with your folder (that you specify below)
::
:: It was an attempt to let you use my preconfigured solution in the solution folder.
::
:: in the end it doesn't help though, cmake ends up rebuilding and overwriting
:: the solution and projects :(
::

set SC_GITHUB_HOME=C:/Users/lucas/Documents/GitHub/supercollider

:: this does a find and replace, you can only do it once.
PowerShell -NoProfile -ExecutionPolicy Bypass -Command "& '.\fixpaths.ps1' '%SC_GITHUB_HOME%'"

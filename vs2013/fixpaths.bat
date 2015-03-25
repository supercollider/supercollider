set SC_GITHUB_HOME=C:/Users/lucas/Documents/GitHub/supercollider

:: this does a find and replace, you can only do it once.
PowerShell -NoProfile -ExecutionPolicy Bypass -Command "& '.\fixpaths.ps1' '%SC_GITHUB_HOME%'"

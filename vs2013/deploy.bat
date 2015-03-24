set SolutionDir=%1
set Configuration=%2
set Platform=%3

set SC_HOME=C:\Supercollider
set SC_DLL=%SolutionDir%\..\dll\%Platform%

set ROBO_FLAGS=/S /V /FP /NP

:: deploy dlls
robocopy %SC_DLL% %SC_HOME% %ROBO_FLAGS%

:: deploy build files
robocopy %SolutionDir%\editors\sc-ide\%Configuration% %SC_HOME% scide.exe %ROBO_FLAGS%
robocopy %SolutionDir%\lang\%Configuration%\ %SC_HOME% sclang.exe %ROBO_FLAGS%
robocopy %SolutionDir%\server\scsynth\%Configuration% %SC_HOME% scsynth.exe %ROBO_FLAGS%

:: class library
robocopy %SolutionDir%\..\..\SCClassLibrary %SC_HOME%\SCClassLibrary %ROBO_FLAGS%

:: ugens
if %Configuration% == Debug (
  SET PLUGINS=plugins_debug
) else (
  SET PLUGINS=plugins
)

robocopy %SolutionDir%\server\plugins\%Configuration% %SC_HOME%\%PLUGINS% *.scx %ROBO_FLAGS%

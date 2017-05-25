@ECHO OFF

REM USE: junctions.bat create <link> <target>
REM USE: junctions.bat remove <link>
REM For both, link and target, the parent folder of the folder group SCClassLibrary, HelpSource, examples and sounds has to be given

GOTO %1

:CREATE
IF NOT EXIST %2\SCClassLibrary (mklink /J %2\SCClassLibrary %3\SCClassLibrary)
IF NOT EXIST %2\HelpSource (mklink /J %2\HelpSource %3\HelpSource)
IF NOT EXIST %2\examples (mklink /J %2\examples %3\examples)
IF NOT EXIST %2\sounds (mklink /J %2\sounds %3\sounds)
GOTO END

:REMOVE
IF EXIST %2\SCClassLibrary (rmdir %2\SCClassLibrary)
IF EXIST %2\HelpSource (rmdir %2\HelpSource)
IF EXIST %2\examples (rmdir %2\examples)
IF EXIST %2\sounds (rmdir %2\sounds)

:END

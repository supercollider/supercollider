function robo {
  param([string] $src, [string] $dst, [string] $filespec="*.*")
  & robocopy $src $dst $filespec /S /V /FP /NP

  if ($LastExitCode -gt 7) {
    write-host -Fore Red "Robocopy died with $code"
    exit $code
  }
}

$SolutionDir=$args[0]
$Configuration=$args[1]
$Platform=$args[2]

$SC_HOME="C:\Supercollider"
$SC_DLL="$SolutionDir\..\dll\$Platform"
$QT_HOME="C:\Qt\5.4"

<# deploy dlls #>
robo $SC_DLL $SC_HOME
robo "$QT_HOME\msvc2013_64\bin" $SC_HOME "*.dll"

<# deploy build files #>
robo "$SolutionDir\editors\sc-ide\$Configuration\" $SC_HOME "scide.exe"
robo "$SolutionDir\lang\$Configuration\" $SC_HOME "sclang.exe"
robo "$SolutionDir\server\scsynth\$Configuration\" $SC_HOME "scsynth.exe"

<# class library #>
robo "$SolutionDir\..\..\SCClassLibrary" "$SC_HOME\SCClassLibrary"

<# ugens #>
if ($Configuration.Equals("Debug")) {
  $Plugins="plugins_debug"
} else {
  $Plugins="plugins"
}

robo "$SolutionDir\server\plugins\$Configuration" "$SC_HOME\$Plugins" "*.scx"
exit 0

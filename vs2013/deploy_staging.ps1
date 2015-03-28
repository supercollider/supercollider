function robo {
  param([string] $src, [string] $dst, [string] $filespec="*.*")
  & robocopy $src $dst $filespec /S /V /FP /NP

  if ($LastExitCode -gt 7) {
    write-host -Fore Red "Robocopy died with $code"
    exit $code
  }
}

#$staging_dir=$args[0]
#$install_dir=$args[1]
#$qt_dir=$args[2]

$staging_dir=".\build\staging"
$install_dir="C:\SuperCollider"
$qt_dir="C:\Qt\5.4"


Write-Host "Deploying SuperCollider"
robo $staging_dir $install_dir

Write-Host "Deploying QT dlls"
robo "$qt_dir\msvc2013_64\bin" "$install_dir\SuperCollider" "*.dll"

Write-Host "Deploying Portaudio and libsndfile dlls"
robo "dll\x64" "$install_dir\SuperCollider"

exit 0

cd build
msbuild INSTALL.vcxproj
cd ..
PowerShell -NoProfile -ExecutionPolicy Bypass -File deploy_staging.ps1 %1 %2 %3

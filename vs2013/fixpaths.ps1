$regex = "__SC_GITHUB_HOME__"

[string] $sc_github_home = $Args[0]
if (-not(Test-Path $sc_github_home)) {break}

$files = Get-ChildItem "solution" -rec

foreach ($file in $files) {
    $x = $file.Extension
    if ($x -eq ".txt" -or $x -eq ".cmake" -or $x -eq ".depend" -or $x -eq ".log" -or $x -eq ".cpp" -or $x -eq ".list" -or $x -eq ".parameters" -or $x -eq ".cpp_parameters") {
      $p = $file.FullName
      echo "Fixing $p"
      (Get-Content $file.PSPath |
        Foreach-Object {$_ -replace $regex, $sc_github_home} ) |
          Set-Content $file.PSPath
    }
}

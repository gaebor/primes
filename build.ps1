Get-ChildItem -Path .\*\build.ps1 | Foreach-Object { 
    $_.FullName
}

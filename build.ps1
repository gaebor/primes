Get-ChildItem -Path .\*\build.ps1 | Foreach-Object { 
    Set-Location -Path $_.Directory
    Start-Process $_.Name -Wait
}

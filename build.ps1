Get-ChildItem -Path .\*\build.ps1 | Foreach-Object {
    echo [ $_.FullName ]
    Set-Location -Path $_.Directory
    Start-Process $_.Name -Wait
}

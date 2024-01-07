Get-ChildItem -Path .\*\build.ps1 | Foreach-Object {
    Write-Host [ $_.FullName ]
    Set-Location -Path $_.Directory
    Invoke-expression -Command $_.Name
}

Get-ChildItem -Path .\*\build.cmd | Foreach-Object
{
    echo $_.FullName
}

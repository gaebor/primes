for /D %%s in (*) do
(
    SET directory=%%s    
    IF %directory:~1,2% NEQ "."
    (
        cd %~dp0\%directory%
        build.cmd
    )
)

cmake .
make

Copy-Item -Path ".\Release\*.exe" -Destination ".\"

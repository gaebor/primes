cmake .
make

Copy-Item -Filter *.exe -Path ".\Release" -Destination ".\"

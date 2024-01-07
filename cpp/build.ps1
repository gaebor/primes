cmake .
msbuild primes.sln -property:Configuration=Relese

Copy-Item -Filter *.exe -Path ".\Release" -Destination ".\"

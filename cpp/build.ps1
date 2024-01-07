cmake .
msbuild primes.sln /p:Configuration=Release

Copy-Item -Filter "*.exe" -Path ".\Release\*" -Recurse -Destination ".\"

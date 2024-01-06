Invoke-WebRequest -UseBasicParsing -Uri ' https://www.python.org/ftp/python/3.10.11/python-3.10.11-amd64.exe' -OutFile 'python-installer.exe'

$ErrorActionPreference = 'Stop'
$ProgressPreference = 'SilentlyContinue'

Start-Process python-installer.exe -ArgumentList '/quiet InstallAllUsers=1 PrependPath=1 Include_test=0' -Wait

python -m pip install 'pyopencl[pocl]' tqdm

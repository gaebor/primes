FROM self-hosted-runner

ADD https://registrationcenter-download.intel.com/akdlm/IRC_NAS/faf10bb4-a1b3-46cf-ae0b-986b419e1b1c-opencl/w_opencl_runtime_p_2023.2.0.49500.exe intel-opencl-installer.exe
ADD https://www.python.org/ftp/python/3.10.11/python-3.10.11-amd64.exe python-installer.exe

SHELL ["powershell", "-Command", "$ErrorActionPreference = 'Stop'; $ProgressPreference = 'SilentlyContinue';"]

# from https://stackoverflow.com/a/77263255/3583290
RUN Start-Process python-installer.exe -ArgumentList '/quiet InstallAllUsers=1 PrependPath=1 Include_test=0' -Wait

RUN Start-Process intel-opencl-installer.exe -ArgumentList '--silent --remove-extracted-files yes --a /norestart /quiet' -Wait

SHELL ["cmd", "/C", "/S"]

RUN ["python", "-m", "pip", "install", "pyopencl", "tqdm"]

ENTRYPOINT ["python", "-c", "from pyopencl import get_platforms; print(*(platform.name for platform in get_platforms()))"]

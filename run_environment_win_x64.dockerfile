FROM mcr.microsoft.com/windows-cssc/python:3.9-windows-ltsc2019

RUN ["python3", "-m", "pip", "install", "pyopencl[pocl]", "tqdm"]

FROM ubuntu:22.04

RUN ["python3", "-m", "pip", "install", "pyopencl[pocl]", "tqdm"]

FROM self-hosted-runner

RUN ["python3", "-m", "pip", "install", "pyopencl[pocl]", "tqdm"]

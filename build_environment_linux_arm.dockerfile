FROM self-hosted-runner

RUN apt-get update && apt-get -y upgrade && apt-get install -y build-essential cmake

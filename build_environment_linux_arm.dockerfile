FROM ubuntu:22.04

RUN apt-get update && apt-get -y upgrade && apt-get install -y build-essential cmake

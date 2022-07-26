# Docker

With Dockerfile, you can build and run the AutoStream converter for Autoware without having to install dependecies on your system.
The steps to build and run a docker image are given below.

## System setup 
- Install Docker by following the instructions on [the official Docker website](https://docs.docker.com/engine/install/).
- Install other essential tools (Git and Tar)
  ```bash
  sudo apt-get update && sudo apt-get install -y git tar
  ```

## Clone and build the converter
- Clone the converter source code
  ```bash
  git clone https://github.com/tomtom-international/AutoStreamForAutoware.git
  ```
- Unpack AutoStreamClient folder into AutoStreamForAutoware (Refer to the instructions on how to get the library and credentials on [README.md](/README.md)
  ```bash
  tar -xvf TomTom_AutoStream-linux_gnu_x86_64-<VERSION>-<COMMIT_HASH>.tar.gz -C AutoStreamForAutoware
  ```
- Build a Docker image
  ```bash
  cd AutoStreamForAutoware
  docker build -t autostream-coverter:latest .
  ```
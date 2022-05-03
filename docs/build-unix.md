# Linux

The converter has been developed and tested with Ubuntu 20.04 and ROS Foxy (see [dependencies](dependencies.md)). However, it may work with other versions of Ubuntu/ROS as well (untested). 

## Prerequisite
### Basic system setup
```bash
sudo su &&\
apt-get clean && \
apt-get update && \
apt-get --allow-unauthenticated install -y \
          wget curl git cmake make gcc g++
```
Installing Lanelet2
- Install ROS2 foxy by following the instructions from [here](https://docs.ros.org/en/foxy/Installation/Ubuntu-Install-Debians.html).
- Install Lanelet2
```bash
sudo apt install ros-foxy-lanelet2
```

## Clone AutoStream for Autoware converter
Clone the source code for the converter from git:
```bash
git clone https://github.com/tomtom-international/AutoStreamForAutoware.git
cd AutoStreamForAutoware
```

## Building with CMake
Create a build folder and navigate to this folder
```bash
mkdir build
cd build
```
Run cmake and compile the code
```bash
source /opt/ros/foxy/setup.bash
cmake .. -DAUTOSTREAM_CLIENT_SDK_PATH=<path-to-extracted-autostream-client-library>
make -j
```

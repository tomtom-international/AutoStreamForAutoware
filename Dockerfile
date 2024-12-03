###########################################
# ROS2 Foxy base image 
###########################################
FROM ubuntu:20.04 AS foxy-base

# Enable noninteractive apt
ENV DEBIAN_FRONTEND=noninteractive

# Install language
RUN apt-get update && apt-get install -y \
  locales \
  && locale-gen en_US.UTF-8 \
  && update-locale LC_ALL=en_US.UTF-8 LANG=en_US.UTF-8 \
  && rm -rf /var/lib/apt/lists/*
ENV LANG en_US.UTF-8

# Install timezone
RUN ln -fs /usr/share/zoneinfo/UTC /etc/localtime \
  && apt-get update && apt-get install -y \
  tzdata \
  && dpkg-reconfigure --frontend noninteractive tzdata \
  && rm -rf /var/lib/apt/lists/*

# Install ROS2
RUN apt-get update && apt-get install -y \
    curl \
    gnupg2 \
    lsb-release \
  && curl -sSL https://raw.githubusercontent.com/ros/rosdistro/master/ros.key -o /usr/share/keyrings/ros-archive-keyring.gpg \
  && echo "deb [arch=$(dpkg --print-architecture) signed-by=/usr/share/keyrings/ros-archive-keyring.gpg] http://packages.ros.org/ros2/ubuntu $(lsb_release -cs) main" | tee /etc/apt/sources.list.d/ros2.list > /dev/null \
  && apt-get update && apt-get install -y \
    ros-foxy-ros-base \
  && rm -rf /var/lib/apt/lists/*

# Set ROS2 environment variables
ENV ROS_DISTRO=foxy
ENV AMENT_PREFIX_PATH=/opt/ros/foxy
ENV COLCON_PREFIX_PATH=/opt/ros/foxy
ENV LD_LIBRARY_PATH=/opt/ros/foxy/lib
ENV PATH=/opt/ros/foxy/bin:$PATH
ENV PYTHONPATH=/opt/ros/foxy/lib/python3.8/site-packages
ENV ROS_PYTHON_VERSION=3
ENV ROS_VERSION=2

###########################################
#  Converter image 
###########################################
FROM foxy-base

# Install dependencies for AutoStream converter
RUN apt-get update && apt-get install -y \
  wget \ 
  curl \
  git \
  cmake \
  make \
  gcc \
  g++ \
  ros-foxy-lanelet2 \
  vim \
  bash-completion \
  && rm -rf /var/lib/apt/lists/*

# Copy source files and AutoStreamClient
COPY . /AutoStreamForAutoware

# Set working directory
WORKDIR /AutoStreamForAutoware

# Build
RUN mkdir build \
    && cd build \
    && cmake .. -DAUTOSTREAM_CLIENT_SDK_PATH=/AutoStreamForAutoware/AutoStreamClient \
    && make -j 

ENTRYPOINT [ "/AutoStreamForAutoware/build/Application/AutoStreamToLaneletApp" ]
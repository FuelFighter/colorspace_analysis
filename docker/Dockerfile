From ros:kinetic-ros-base-xenial

ENV DEBIAN_FRONTEND noninteractive
ENV APT "apt-get -yqq -o=Dpkg::Use-Pty=0"

RUN $APT update && \
  $APT install --no-install-recommends apt-utils && \
  $APT upgrade && \
  $APT install build-essential tmux vim htop iputils-ping

# Install OpenCV with v4l/v4l2 support
RUN  apt-get update && \
  apt-get upgrade -y && \
  apt-get install -y --no-install-recommends python python-dev python-pip build-essential cmake git pkg-config libjpeg8-dev libtiff5-dev libjasper-dev libpng12-dev libgtk2.0-dev libavcodec-dev libavformat-dev libswscale-dev libv4l-dev libatlas-base-dev gfortran libavresample-dev libgphoto2-dev libgstreamer-plugins-base1.0-dev libdc1394-22-dev  && \
  pip install numpy && \
  cd /opt && \
  git clone https://github.com/opencv/opencv_contrib.git && \
  cd opencv_contrib && \
  git checkout 3.4.0 && \
  cd /opt && \
  git clone https://github.com/opencv/opencv.git && \
  cd opencv && \
  git checkout 3.4.0 && \
  mkdir build && \
  cd build && \
  cmake   -D CMAKE_BUILD_TYPE=RELEASE \
    -D BUILD_NEW_PYTHON_SUPPORT=ON \
    -D CMAKE_INSTALL_PREFIX=/usr/local \
    -D INSTALL_C_EXAMPLES=OFF \
    -D INSTALL_PYTHON_EXAMPLES=OFF \
    -D OPENCV_EXTRA_MODULES_PATH=/opt/opencv_contrib/modules \
    -D PYTHON_EXECUTABLE=/usr/bin/python2.7 \
    -D BUILD_EXAMPLES=OFF /opt/opencv && \
  make -j $(nproc) && \
  make install && \
  ldconfig && \
  apt-get clean && rm -rf /var/lib/apt/lists/* && \
  rm -rf /opt/opencv*

# SDL
RUN $APT update && \
  $APT install libsdl2-dev freeglut3-dev libglew-dev libeigen3-dev libsoil-dev

# Debug
RUN $APT update && \
  $APT install gdb valgrind

## Building PCL 
#RUN apt-get install -y software-properties-common
#RUN apt-get update && apt-get install -y \
#git build-essential linux-libc-dev \
#cmake cmake-gui \
#libusb-1.0-0-dev libusb-dev libudev-dev \
#mpi-default-dev openmpi-bin openmpi-common \
#libflann1.8 libflann-dev \
#libeigen3-dev \
#libboost-all-dev \
#libvtk5.10-qt4 libvtk5.10 libvtk5-dev \
#libqhull* libgtest-dev \
#freeglut3-dev pkg-config \
#libxmu-dev libxi-dev \
#qt-sdk openjdk-8-jdk openjdk-8-jre \
#openssh-client


# PCL - build from source and install
#RUN cd /opt \
#&& git clone https://github.com/PointCloudLibrary/pcl.git pcl-trunk \
#&& ln -s /opt/pcl-trunk /opt/pcl \
#&& cd /opt/pcl && git checkout pcl-1.8.0 \
#&& mkdir -p /opt/pcl-trunk/release \
#&& cd /opt/pcl/release && cmake -DCMAKE_BUILD_TYPE=None -DBUILD_GPU=ON -DBUILD_apps=ON -DBUILD_examples=ON .. \
#&& cd /opt/pcl/release && make -j3 \
#&& cd /opt/pcl/release && make install \
#&& cd /opt/pcl/release && make clean

## For lidar
RUN $APT update && \
  $APT install libeigen3-dev \
  libvtk6-dev

RUN $APT update && \
  $APT install \
  ros-kinetic-tf2-geometry-msgs \
  ros-kinetic-tf \
  ros-kinetic-tf-conversions \
  ros-kinetic-geodesy ros-kinetic-pcl-ros ros-kinetic-nmea-msgs ros-kinetic-libg2o \
  ros-kinetic-rviz
  #ros-kinetic-pcl-ros \
  #ros-kinetic-pcl-conversions \
  #ros-kinetic-pcl-msgs \
  #python-vtk \
  #tcl-vtk

ADD config.sh /root/config.sh
ENTRYPOINT /bin/bash -c '. /ros_entrypoint.sh && . /root/config.sh && cd $WORKDIR && /bin/bash'

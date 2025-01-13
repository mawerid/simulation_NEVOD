FROM ubuntu:22.04

# Set environment variables
ENV DEBIAN_FRONTEND=noninteractive \
    TZ=Europe/Moscow \
    ROOT_VERSION=6.34.02 \
    GEANT4_VERSION=11.3.0 \
    ROOTSYS=/usr/local/root \
    PATH="${PATH}:/usr/local/root/bin:/usr/local/geant4/bin" \
    LD_LIBRARY_PATH="${LD_LIBRARY_PATH}:/usr/local/root/lib:/usr/local/geant4/lib" \
    PYTHONPATH="${PYTHONPATH}:/usr/local/root/lib" \
    Geant4_DIR=/usr/local/geant4/lib/Geant4-${GEANT4_VERSION}

# Update and install dependencies
RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential cmake wget curl git unzip tar binutils gnupg \
    software-properties-common libx11-dev libxmu-dev libxpm-dev libxft-dev \
    libxext-dev libglew-dev libjpeg-dev libpng-dev libtiff-dev \
    libgif-dev libfftw3-dev qtbase5-dev libqt5widgets5 \
    libqt5opengl5-dev libssl-dev libxml2-dev \
    python3 python3-pip python3-dev gfortran && \
    rm -rf /var/lib/apt/lists/*

# Install ROOT
RUN mkdir /tmp/root && \
    wget -qO- https://root.cern/download/root_v${ROOT_VERSION}.source.tar.gz | tar xz -C /tmp/root --strip-components=1 && \
    mkdir /tmp/root-build && cd /tmp/root-build && \
    cmake -DCMAKE_INSTALL_PREFIX=/usr/local/root -DCMAKE_BUILD_TYPE=Release /tmp/root && \
    cmake --build . --target install -j4 && \
    rm -rf /tmp/root /tmp/root-build

# Install GEANT4
RUN mkdir /tmp/geant4 && \
    wget -qO- https://github.com/Geant4/geant4/archive/refs/tags/v${GEANT4_VERSION}.tar.gz | tar xz -C /tmp/geant4 --strip-components=1 && \
    mkdir /tmp/geant4-build && cd /tmp/geant4-build && \
    cmake -DCMAKE_INSTALL_PREFIX=/usr/local/geant4 \
    -DGEANT4_INSTALL_DATA=OFF \
    -DGEANT4_USE_QT=ON \
    -DGEANT4_USE_OPENGL_X11=ON \
    -DGEANT4_USE_RAYTRACER_X11=ON \
    -DGEANT4_BUILD_MULTITHREADED=ON \
    /tmp/geant4 && \
    cmake --build . --target install -j$(nproc) && \
    rm -rf /tmp/geant4 /tmp/geant4-build

COPY ./config /workspace/config
COPY ./include /workspace/include
COPY ./src /workspace/src
COPY ./test /workspace/test
COPY ./CMakeLists.txt /workspace/CMakeLists.txt
COPY ./main.cc /workspace/main.cc
WORKDIR /workspace

RUN mkdir build && cd build && \
    cmake .. && \
    make -j$(nproc)
ENTRYPOINT ["/workspace/build/nevod"]

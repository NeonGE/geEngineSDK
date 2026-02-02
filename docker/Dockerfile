FROM lscr.io/linuxserver/openvscode-server:latest

USER root

RUN apt-get update && apt-get install -y \
    cmake \
    build-essential \
    ninja-build \
    gdb \
    git \
    pkg-config \
    clang \
    lld \
    libicu-dev \
    python3 \
    && rm -rf /var/lib/apt/lists/*

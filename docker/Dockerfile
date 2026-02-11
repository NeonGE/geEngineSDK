FROM lscr.io/linuxserver/openvscode-server:latest

USER root

RUN apt-get update && apt-get install -y --no-install-recommends \
    # --- toolchain / build ---
    ca-certificates \
    cmake \
    build-essential \
    ninja-build \
    gdb \
    git \
    pkg-config \
    clang \
    lld \
    python3 \
    rsync \
    # --- Dependencies ---
    libicu-dev \
    # --- SFML + X11/OpenGL deps (for building) ---
    libsfml-dev \
    libx11-dev \
    libxrandr-dev \
    libxi-dev \
    libudev-dev \
    libgl1-mesa-dev \
    libfreetype6-dev \
    libopenal-dev \
    libvorbis-dev \
    libflac-dev \
    && rm -rf /var/lib/apt/lists/*
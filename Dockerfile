# Install build dependencies for WSL Ubuntu
FROM ubuntu:24.04@sha256:786a8b558f7be160c6c8c4a54f9a57274f3b4fb1491cf65146521ae77ff1dc54

RUN apt-get update && apt-get install --no-install-recommends -y \
  git \
  build-essential \
  ninja-build \
  cmake \
  gcc-arm-none-eabi \
  libstdc++-arm-none-eabi-newlib \
  openocd \
  gdb-multiarch \
  binutils-arm-none-eabi \
  clang-format \
  elfutils \
  less \
  tree \
  file \
  zip \
  unzip \
  curl \
  wget \
  python3 \
  python3-pip \
  ccache \
  clangd \
  minicom \
  ca-certificates \
  sudo \
  usbutils \
  udev \
  && rm -rf /var/lib/apt/lists/*

# Create team development user.
RUN userdel -r ubuntu 2>/dev/null || true && \
  groupadd --gid 1000 lbr && \
  useradd --uid 1000 --gid 1000 -ms /bin/bash lbr && \
  usermod -aG sudo,dialout,plugdev lbr && \
  echo "lbr ALL=(ALL) NOPASSWD:ALL" > /etc/sudoers.d/lbr

# Verify installations
RUN git --version && \
  ninja --version && \
  cmake --version && \
  arm-none-eabi-gcc --version && \
  arm-none-eabi-g++ --version && \
  openocd --version && \
  gdb-multiarch --version && \
  clang-format --version && \
  clangd --version && \
  update-ca-certificates

WORKDIR /workspace

CMD ["/bin/bash"]
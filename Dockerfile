
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
  clang-format \
  clangd \
  && rm -rf /var/lib/apt/lists/*

# Verify installations
RUN git --version && \
  ninja --version && \
  cmake --version && \
  arm-none-eabi-gcc --version && \
  arm-none-eabi-g++ --version && \
  openocd --version && \
  gdb-multiarch --version && \
  clang-format --version && \
  clangd --version


# Set working directory
WORKDIR /workspace

# Default command
CMD ["/bin/bash"]



#!/bin/bash

set -e

if [[ "$MSYSTEM" == "MINGW32" ]]; then
    export MSYS2_ARCH="i686"
else
    export MSYS2_ARCH="x86_64"
fi

# Update everything
pacman --noconfirm --sync --refresh --refresh pacman
pacman --noconfirm --sync --refresh --refresh --sysupgrade --sysupgrade

# Remove ADA and ObjC packages breaking the depenencies
pacman -R --noconfirm mingw-w64-i686-gcc-ada mingw-w64-i686-gcc-objc || true;
pacman -R --noconfirm mingw-w64-x86_64-gcc-ada mingw-w64-x86_64-gcc-objc || true;

# Install the required packages
pacman --noconfirm -Syyu --needed \
    mingw-w64-$MSYS2_ARCH-gcc \
    mingw-w64-$MSYS2_ARCH-meson \
    mingw-w64-$MSYS2_ARCH-ninja \
    mingw-w64-$MSYS2_ARCH-pkg-config \

# Build
meson _build -Dstatic=true
cd _build
ninja

# Test
export MUTEST_OUTPUT=tap
meson test || {
  cat meson-logs/testlog.txt
  exit 1
}

#!/bin/bash

set -x

builddir="_build"
srcdir=`pwd`

meson --prefix /usr "$@" $builddir $srcdir || exit $?

ninja -C $builddir || exit $?
meson test -C $builddir || exit $?

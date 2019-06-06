#!/bin/bash

set -x

builddir="_build"
srcdir=`pwd`

CFLAGS='-coverage -ftest-coverage -fprofile-arcs'

meson --prefix /usr "$@" $builddir $srcdir || exit $?

ninja -C $builddir || exit $?
meson test -C $builddir || exit $?

cpp-coveralls -r . -b $builddir -i src -i ../src -e tests -e ../tests --gcov-options='\-lp'

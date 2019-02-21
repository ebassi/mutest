#!/bin/bash

set -x

meson --prefix /usr "$@" _build . || exit $?

cd _build

ninja || exit $?
meson test || exit $?

cd ..

rm -rf _build

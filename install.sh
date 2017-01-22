#!/bin/sh
JOBS=$(getconf _NPROCESSORS_CONF)

# Remove previously built 
rm -rf ./lib
rm -rf ./src

#clone llvm and apply patches
git clone git@github.com:vgvassilev/cling.git src
cd src/tools/packaging
./cpt.py --with-workdir=./build/ --current-dev=tar --no-test
mkdir ./lib
cp -rf ./build/builddir/{bin,include,lib,libexec,share} ./lib
cp -r ./build/cling-src/include/llvm/* ./lib/include/llvm/.
cp -r ../../include/cling ./lib/include/.
mv ./lib ./../../../.

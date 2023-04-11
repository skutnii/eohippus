#!/bin/bash

git clone --recurse-submodules  --branch boost-1.81.0 https://github.com/boostorg/boost.git $1
cd $1
./bootstrap.sh
./b2
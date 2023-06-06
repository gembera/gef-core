#!/bin/bash

# Install wasienv
#curl https://raw.githubusercontent.com/wasienv/wasienv/master/install.sh | sh

mkdir -p debug
cd debug
wasimake cmake -DCMAKE_BUILD_TYPE=Debug ../..
cd ..
make -C debug

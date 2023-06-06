#!/bin/bash

# Install wasienv
#curl https://raw.githubusercontent.com/wasienv/wasienv/master/install.sh | sh

mkdir -p release
cd release
wasimake cmake ../..
cd ..
make -C release

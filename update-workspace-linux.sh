#!/bin/sh
#Alt linux commands:
sudo apt-get update
sudo apt-get install cmake glibc-devel-static libstdc++-devel-static clang libSDL2_image libSDL2_mixer libSDL2_image-devel libSDL2_mixer-devel -y
#sudo apt -qq update
#sudo apt install --yes cmake build-essential libsdl2-dev libsdl2-mixer-dev libsdl2-image-dev
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo ..
cd ..

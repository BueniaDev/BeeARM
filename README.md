# BeeARM
ARM emulation engine, sorta

An emulation of the ARM processor, written in C++17.

# Features

Extremely accurate - passes all the test ROMs at my disposal

Platform-independent and architecture-independent code

Easily customizable interface

Simple debug output

And more to come!

# Building Instructions

The BeeARM library (and the testing framework, if desired) does not have any dependencies and can be compiled with MinGW on Windows, and (presumably) both GCC and Clang on Linux, as well as (presumably) AppleClang on OSX.

## Linux:

Step 1: Install dependencies:

Core dependencies:

Compiler: GCC or Clang. You only need one of those two:

GCC 10.2.0+ (earlier versions not tested):

Debian (not tested): `sudo apt-get install build-essential`

Arch (not tested): `sudo pacman -S base-devel`

Fedora (not tested): `sudo dnf install gcc-c++`

OpenSUSE (not tested): `sudo zypper in gcc-c++`

Clang (not tested):

Debian: `sudo apt-get install clang clang-format libc++-dev` (in some distros, clang-5.0)

Arch: `pacman -S clang`, `libc++` is in the AUR. Use pacaur or yaourt to install it.

Fedora: `dnf install clang libcxx-devel`

OpenSUSE: `zypper in clang`

Other core dependencies:

Git (if not installed already) and CMake 3.1+:

Debian (not tested): `sudo apt-get install git cmake`

Arch (not tested): `sudo pacman -S git`

Fedora (not tested): `sudo dnf install git cmake`

OpenSUSE (not tested): `sudo zypper in git cmake extra-cmake-modules`

Step 2: Clone the repository:

`git clone --recursive https://github.com/BueniaDev/BeeARM.git`

`cd BeeARM`

Step 3: Compile:

`mkdir build && cd build`

`cmake .. -G "Unix Makefiles" -DCMAKE_BUILD_TYPE="<Debug/Release>"`

`make -j$(nproc --all)`


## Mac OS (not tested):

You will need [homebrew](https://brew.sh), a recent version of Xcode and the Xcode command-line tools to build BeeARM.
Please note that due to personal financial constraints, BeeARM has not been tested on Mac OS as of yet.

Step 1: Install dependencies:

`brew install git cmake pkg-config`

Step 2: Clone the repository:

`git clone --recursive https://github.com/BueniaDev/BeeARM.git`

`cd BeeARM`

Step 3: Compile:

`mkdir build && cd build`

`cmake .. -G "Unix Makefiles" -DCMAKE_BUILD_TYPE="<Debug/Release>"`

`make -j$(sysctl -n hw.ncpu)`

## Windows:

You will need [MSYS2](https://msys2.github.io) in order to install BeeARM.
Make sure to run `pacman -Syu` as needed.

Step 1: Install dependencies:

`pacman -S base-devel mingw-w64-x86_64-toolchain mingw-w64-x86_64-cmake git`

Step 2: Clone the repository:

`git clone --recursive https://github.com/BueniaDev/BeeARM.git`

`cd BeeARM`

Step 3: Compile:

`mkdir build && cd build`

`cmake .. -G "MSYS Makefiles" -DCMAKE_BUILD_TYPE="<Debug/Release>"`

`(mingw32-)make -j$(nproc --all)`

# Plans

## Near-term

Complete ARMv5(TExP/xM) support

## Medium-term

Dynamic disassembly
Support for ARMv1 and ARMv2

## Long-term

Graphical debugger (possibly via Qt5?)
Support for other ARM architecture versions

# License

<img src="https://www.gnu.org/graphics/gplv3-127x51.png" alt="drawing" width="150"/>

BeeARM is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

# Copyright

(C) 2022 BueniaDev. This project is not affiliated in any way with ARM. ARM is a registered trademark of ARM Ltd.

For information regarding BeeARM's stance on copyright infringement, as well as proof of BeeARM's legality, see the LEGAL.md file in this repository.
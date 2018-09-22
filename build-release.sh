#!/bin/sh
# This little script creates a build/ directory, enters it
# and builds BibleTime there.
#
# BibleTime will be installed to /usr/local by default (see CMAKE_INSTALL_PREFIX below).

if [ ! -d build ]; then mkdir build; fi
cd build
cmake -D CMAKE_BUILD_TYPE=Release -D CMAKE_INSTALL_PREFIX=/usr/local ..  || exit 1
make clean
make -j4 install || exit 1
cd ..

echo
echo "BibleTime has been installed to /usr/local."
echo "You can run it by typing /usr/local/bin/bibletime."
echo

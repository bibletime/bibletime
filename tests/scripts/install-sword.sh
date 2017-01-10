#!/bin/bash
set -ve
svn checkout --non-interactive http://crosswire.org/svn/sword/trunk sword-trunk
cd sword-trunk
mkdir b
cd b
cmake ..
make
sudo make install

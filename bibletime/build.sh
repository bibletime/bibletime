# This little script creates a build/ directory, enters it
# and builds BibleTime there.

if [ ! -d build ]; then mkdir build; fi
cd build
VERBOSE=1 cmake ..
#cmake ..
make

# This little script creates a build/ directory, enters it
# and builds BibleTime there.

#TMP FIX; expand path to let kde4-config be found
PATH="$PATH:/usr/lib/kde4/bin"

if [ ! -d build ]; then mkdir build; fi
cd build
VERBOSE=1 cmake ..
#cmake ..
make install

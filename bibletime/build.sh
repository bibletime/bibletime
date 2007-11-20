# This little script creates a build/ directory, enters it
# and builds BibleTime there.

#TMP FIX; expand path to let kde4-config be found
PATH="$PATH:/usr/lib/kde4/bin"

if [ ! -d build ]; then mkdir build; fi
cd build
VERBOSE=1 cmake -D MODE=RELEASE -D CMAKE_INSTALL_PREFIX=/usr ..  || exit 1
#cmake .. || exit 1
VERBOSE=1 make install

#!/bin/bash
# Author: Gregory Hellings
# Copyright: BibleTime development team
# Initial creation date: 23 April 2009
# This is a Mac-only shell script that makes it possible for us to avoid having to run
#   lots of annoying commands on the Qt frameworks for the Mac build of BibleTime.
#   It is inspired and draws heavily from th example at http://www.cmake.org/Wiki/CMake:CPackPackageGenerators

BT_BUNDLE="`echo "$0" | sed -e 's/\/Contents\/MacOS\/BibleTime//'`"

echo "running $0"
echo "BT_BUNDLE: $BT_BUNDLE"

export DYLD_LIBRARY_PATH=$BT_BUNDLE/Contents/Resources/Library
export DYLD_FRAMEWORK_PATH=$BT_BUNDLE/Contents/Resources/Library/Frameworks
export PATH=$BT_BUNDLE/Contents/Resources/bin:$PATH
export QT_PLUGIN_PATH=$BT_BUNDLE/Contents/plugins

cd $BT_BUNDLE/Contents/Resources/bin
# Run BibleTime
exec "bibletime"

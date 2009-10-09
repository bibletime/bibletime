#!/bin/sh
#
# This script can be used to reformat BibleTime's codebase
# It requires the following package to be installed on your system
# * astyle
#
# $LastChangedDate: 2009-10-09 04:43:08 +0200 (Fr, 09 Okt 2009) $

cd  `dirname $0`/../../src/

ASTYLE_OPTIONS="--indent=spaces=4 --brackets=attach --indent-classes --indent-switches --brackets=break-closing --pad=oper --suffix=none --options=none"

astyle ${ASTYLE_OPTIONS}  `find . -name "*.cpp"` `find . -name "*.h"`
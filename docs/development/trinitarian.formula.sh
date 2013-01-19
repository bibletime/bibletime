#!/bin/sh
#########
#
# In the name of the Father, and of the Son, and of the Holy Spirit.
#
# This file is part of BibleTime'\''s source code, http://www.bibletime.info/.
#
# Copyright 1999-2011 by the BibleTime developers.
# The BibleTime source code is licensed under the GNU General Public License version 2.0.
#
##########
if man -Pcat sed | grep -q .-in-place ; then
	d=src
	while [ ! -d "$d" ]; do
		echo "$d"
		if [ $(echo "$d" | wc -c) -gt 9 ] ; then
			echo 'Having difficulty finding your src directory.'
			exit
		fi
		d="../$d"
	done
	echo "$d"
	find "$d" -type f -iname '*.h' | while read h ; do
		if grep -qi copyright $h ; then
			sed --in-place --regexp-extended '{
			N
			N
			N
			N
			s_(/\*+\n\*\s*\n)(\*\s+[abefhilmopr-tBT\ ]+.[b-fhil-pr-uw,:/\ \.]+\n\*\s*\n\*\s+Copyright)_\1* In the name of the Father, and of the Son, and of the Holy Spirit.\n*\n\2_
			}' $h
		else
			echo '/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime'\''s source code, http://www.bibletime.info/.
*
* Copyright 1999-2011 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/' > "$h.withcc"
			cat "$h" >> "$h.withcc"
			mv "$h.withcc" "$h"
		fi
	done
else
	echo 'You do not appear to have a version of sed appropriate for this script.'
fi


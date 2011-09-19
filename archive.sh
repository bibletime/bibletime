#!/bin/sh
FULLNAME="bibletime-$1"
TARNAME="${FULLNAME}.tar"
git archive --format=tar "--prefix=${FULLNAME}/" -o "$TARNAME" HEAD && bzip2 --best "$TARNAME" || exit
TARBZ2NAME="${TARNAME}.bz2"
BYTES=`stat '--format=%s' "$TARBZ2NAME"`
echo 'Details:'
echo "  Filename:  ${TARBZ2NAME}"
echo "  File size: $BYTES bytes`echo $BYTES|awk '{s[2**30]="G";s[2**20]="M";hum[1024]="k";for(x=2**30;x>=1024;x/=1024){if($1>=x){printf " (%.2f %sB)\n",$1/x,s[x];break}}}'`"
echo "  MD5SUM:    `md5sum \"${TARBZ2NAME}\" |cut -b -32`"
echo "  SHA1SUM:   `sha1sum \"${TARBZ2NAME}\" |cut -b -40`"
echo "  SHA256SUM: `sha256sum \"${TARBZ2NAME}\" |cut -b -64`"

#!/bin/sh
FULLNAME="bibletime-$1"
TARXZNAME="${FULLNAME}.tar.xz"
git archive --format=tar "--prefix=${FULLNAME}/" HEAD | xz -9e > "$TARXZNAME" || exit 1
BYTES=`stat '--format=%s' "$TARXZNAME"`
echo 'Details:'
echo "  Filename:  ${TARXZNAME}"
echo "  File size: $BYTES bytes`echo $BYTES|awk '{s[2**30]="G";s[2**20]="M";hum[1024]="k";for(x=2**30;x>=1024;x/=1024){if($1>=x){printf " (%.2f %sB)\n",$1/x,s[x];break}}}'`"
echo "  MD5SUM:    `md5sum \"${TARXZNAME}\" |cut -b -32`"
echo "  SHA1SUM:   `sha1sum \"${TARXZNAME}\" |cut -b -40`"
echo "  SHA256SUM: `sha256sum \"${TARXZNAME}\" |cut -b -64`"

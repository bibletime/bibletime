#!/bin/bash
set -euo pipefail

FULLNAME="bibletime-${1:-custom}"

if ! true "${FULLNAME@K}"; then
  echo '@K variable transformation not supported!' 1>&2
  exit 1
fi

# Use a temporary directory:
TMPDIR=$(mktemp -d)
# shellcheck disable=SC2064
trap "rm -rf ${TMPDIR@K}" EXIT

make_tarball() {
    COMPRESSOR="$1"
    OUT="${TMPDIR}/$2"
    git archive --format=tar "--prefix=${FULLNAME}/" HEAD | ${COMPRESSOR} > "${OUT}" || exit 1
    BYTES=$(stat '--format=%s' "${OUT}")
    PRETTY_BYTES=$(echo "${BYTES}" | awk '{s[2**30]="G";s[2**20]="M";hum[1024]="k";for(x=2**30;x>=1024;x/=1024){if($1>=x){printf "%.2f %sB",$1/x,s[x];break}}}')
    MD5SUM=$(md5sum "${OUT}"|cut -b -32)
    SHA1SUM=$(sha1sum "${OUT}"|cut -b -40)
    SHA256SUM=$(sha256sum "${OUT}"|cut -b -64)
    cat << END_OF_DETAILS

Details of $2:
  File size: ${BYTES} bytes (${PRETTY_BYTES})
  MD5SUM:    ${MD5SUM}
  SHA1SUM:   ${SHA1SUM}
  SHA256SUM: ${SHA256SUM}
END_OF_DETAILS
    mv -i "${OUT}" .
    gpg --armor --detach-sig "$2"
}

echo "Creating archives for ${FULLNAME}..."
make_tarball 'xz -9e' "${FULLNAME}.tar.xz"
make_tarball 'lzip --best' "${FULLNAME}.tar.lz"

#!/bin/sh
TARGETFILE="$1"
HEADER="$2"
TMPFILE=$(mktemp)
awk '
  BEGIN { a = 0 }

  /^\/\*/ {
    if (a == 0)
      a = 1
  }
  /\*\// {
    if (a == 1)
      a = 2
  }

  {
    if (a == 2) {
      a = 3
    } else if (a == 3) {
      print
    }
  }
' "$TARGETFILE" > "$TMPFILE"
cat "$HEADER" "$TMPFILE" > "$TARGETFILE"
rm "$TMPFILE"

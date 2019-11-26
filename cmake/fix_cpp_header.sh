#!/bin/sh
set -euo pipefail

ON_EXIT_ITEMS=()

on_exit() {
    while [ ${#ON_EXIT_ITEMS[*]} -ne 0 ]; do
        eval "${ON_EXIT_ITEMS[${#ON_EXIT_ITEMS[@]}-1]}"
        unset ON_EXIT_ITEMS[${#ON_EXIT_ITEMS[@]}-1]
    done
}

add_on_exit() {
    local N=${#ON_EXIT_ITEMS[*]}
    ON_EXIT_ITEMS["${N}"]="$*"
    if [[ "${N}" -eq 0 ]]; then
        trap on_exit EXIT
    fi
}

TARGETFILE="$1"
HEADER="$2"
TMPFILE=$(mktemp)
add_on_exit "rm -f '${TMPFILE}'"

cat "$HEADER" > "$TMPFILE"

awk '
  BEGIN { state = 0 }

  /^\/\*/ {
    if (state == 0) {
      state = 1
    }
  }

  /\*\// {
    if (state == 1) {
      $0 = substr($0, index($0, "*/") + 2)
      sub(/^[[:blank:]]*/, "")
      state = 2
    }
  }

  {
    if (state != 1) {
      print
      state = 2
    }
  }
' "$TARGETFILE" | \
awk '
  BEGIN { state = 0 }
  /^[[:blank:]]*$/ { if (state == 1) print }
  /[^[:blank:]]/ { state = 1; print }
' >> "$TMPFILE"
cp -f "$TMPFILE" "$TARGETFILE"

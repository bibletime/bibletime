#!/bin/bash
set -euo pipefail

echo2() { echo "$1" 1>&2; }
log_info() { echo "$1"; }
log_error() { echo2 "ERROR: $1"; }
log_fatal() { echo2 "FATAL ERROR: $1"; }

if [ "$#" -lt 1 ] || [ "$#" -gt 2 ]; then
    log_fatal "Illegal number of parameters!"
    echo2 "Usage: $0 <install_prefix> [num_parallel_jobs]"
    exit 1
fi

case "$1" in
    /*) SWORD_INSTALL_PREFIX="$1" ;;
    *) SWORD_INSTALL_PREFIX="${PWD}/$1" ;;
esac

NUM_PARALLEL_JOBS="${2:-1}"
case "$NUM_PARALLEL_JOBS" in
    ''|*[!0-9]*)
        log_fatal "Invalid [num_parallel_jobs] argument given!"
        exit 1
        ;;
    *) ;;
esac

SWORD_SVN='http://crosswire.org/svn/sword/trunk'
SWORD_SVN_GIT_MIRROR='https://github.com/bibletime/crosswire-sword-mirror.git'

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

TMP_DIR=$(mktemp -d)
add_on_exit "rm -rf '${TMP_DIR}'"
log_info "Created temporary directory \"${TMP_DIR}\" for fetching and building Sword sources."

SRC_DIR="${TMP_DIR}/source"

log_info "Attempting to fetch Sword from ${SWORD_SVN_GIT_MIRROR} ..."
if ! git clone --depth 1 "$SWORD_SVN_GIT_MIRROR" "$SRC_DIR"; then
    log_fatal "Failed to fetch Sword from ${SWORD_SVN_GIT_MIRROR}"
    rm -rf "$SRC_DIR"

    log_info "Attempting to fetch Sword from ${SWORD_SVN} ..."
    if ! svn checkout --non-interactive "$SWORD_SVN" "$SRC_DIR"; then
        log_error "Failed to fetch Sword from ${SWORD_SVN}"
    fi
fi

BIN_DIR="${TMP_DIR}/build"

# This only works for CMake 3.13 and later, so we have to work around:
#cmake "-DCMAKE_INSTALL_PREFIX=${SWORD_INSTALL_PREFIX}" -S "$SRC_DIR" -B "$BIN_DIR"
mkdir "$BIN_DIR"
pushd "$BIN_DIR"
cmake "-DCMAKE_INSTALL_PREFIX=${SWORD_INSTALL_PREFIX}" "$SRC_DIR"
popd

# The --verbose flag was added in CMake 3.14, so we have to do without:
#cmake --build "$BIN_DIR" -j "$NUM_PARALLEL_JOBS" --verbose
cmake --build "$BIN_DIR" -- -j "$NUM_PARALLEL_JOBS"

# The --install flag was added in CMake 3.15, so we have to use --build instead:
#cmake --install "$BIN_DIR" --verbose
cmake --build "$BIN_DIR" -- -j "$NUM_PARALLEL_JOBS" install

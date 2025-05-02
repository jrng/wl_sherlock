#! /bin/sh

### BEGIN OF CONFIG #########

COMPILER="clang"
CUI_ROOT="$HOME/opt/cui"

### END OF CONFIG ###########

compile () {
    echo "[COMPILE] $1"
    if [ "${NO_JOBS}" = "-no_jobs" ]; then
        $1
    else
        $1 &
    fi
}

package () {
    echo "[PACKAGE] $1"
    rm -rf "$1.app"

    mkdir -p "$1.app/Contents/MacOS"
    mkdir -p "$1.app/Contents/Resources"

    cp "$2" "$1.app/Contents/MacOS/"
    cp "../src/$2.Info.plist" "$1.app/Contents/Info.plist"
}

REL_PATH=$(dirname "$0")

COMPILER_FLAGS="-std=c99 -I${CUI_ROOT}/include"
LINKER_FLAGS="-L${CUI_ROOT}/lib64 -lcui -framework AppKit -framework Quartz -framework Metal"

if [ "$(uname)" != "Darwin" ]; then
    echo "It doesn't look like your running on macos. You might want to check that."
fi

NO_JOBS="$1"
BUILD_TYPE="$2"

if [ "${NO_JOBS}" != "-no_jobs" ]; then
    BUILD_TYPE="${NO_JOBS}"
fi

case "${BUILD_TYPE}" in
    "reldebug")
        COMPILER_FLAGS="${COMPILER_FLAGS} -g -O2 -DCUI_DEBUG_BUILD=1"
        ;;

    "release")
        COMPILER_FLAGS="${COMPILER_FLAGS} -O2 -DNDEBUG"
        ;;

    *)
        COMPILER_FLAGS="${COMPILER_FLAGS} -pedantic -Wall -Wextra -g -O0 -DCUI_DEBUG_BUILD=1"
        ;;
esac

if [ ! -d "${REL_PATH}/build" ]; then
    mkdir "${REL_PATH}/build"
fi

cd "${REL_PATH}/build"

if [ "$(uname -m)" = "arm64" ]; then

    compile "${COMPILER} ${COMPILER_FLAGS} -o wl_sherlock-arm64 -target arm64-apple-macos11 ../src/wl_sherlock.c ${LINKER_FLAGS}"
    compile "${COMPILER} ${COMPILER_FLAGS} -o wl_sherlock-x86_64 -target x86_64-apple-macos10.14 ../src/wl_sherlock.c ${LINKER_FLAGS}"

    wait

    compile "lipo -create -output wl_sherlock wl_sherlock-arm64 wl_sherlock-x86_64"

elif [ "$(uname -m)" = "x86_64" ]; then

    echo "This machine can't compile for aarch64"

    compile "${COMPILER} ${COMPILER_FLAGS} -o wl_sherlock -target x86_64-apple-macos10.14 ../src/wl_sherlock.cpp ${LINKER_FLAGS}"

fi

wait

package "wl_sherlock" "wl_sherlock"

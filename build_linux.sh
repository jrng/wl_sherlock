#! /bin/sh

### BEGIN OF CONFIG #########

COMPILER="clang"
CUI_ROOT="$HOME/opt/cui"

### END OF CONFIG ###########

compile () {
    echo "[COMPILE] $1"
    $1
}

REL_PATH=$(dirname "$0")

COMPILER_FLAGS="-std=c99 -I${CUI_ROOT}/include"
LINKER_FLAGS="-L${CUI_ROOT}/lib64 -lcui -lm -pthread -ldl -lX11 -lXext -lXrandr -lwayland-client -lwayland-cursor -lxkbcommon -lEGL -lGLESv2 -lwayland-egl"

if [ "$(uname)" != "Linux" ]; then
    echo "It doesn't look like your running on linux. You might want to check that."
fi

BUILD_TYPE="$1"

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

compile "${COMPILER} ${COMPILER_FLAGS} -o wl_sherlock ../src/wl_sherlock.c ${LINKER_FLAGS}"

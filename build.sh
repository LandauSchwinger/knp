#!/bin/bash

VERBOSE=0
RELEASE=0
CMAKE_BINARY=cmake
CMAKE_ADD_OPTS=${CMAKE_ADD_OPTS:-""}
MAKE_ADD_OPTS=${MAKE_ADD_OPTS:-""}

CURDIR="$(dirname "$(readlink -f "$0")")"

usage() { echo "Usage: $0 [-v] [-r] [-c]" 1>&2; }


build()
{
  [ $VERBOSE == 1 ] && MAKE_ADD_OPTS="${MAKE_ADD_OPTS} VERBOSE=1"
  if [[ $RELEASE == 1 ]]; then
    echo "Building release..."
    BUILD_DIR="build_release"
    CMAKE_ADD_OPTS="${CMAKE_ADD_OPTS} -DCMAKE_BUILD_TYPE=Release"
  else
    echo "Building debug..."
    CMAKE_ADD_OPTS="${CMAKE_ADD_OPTS} -DCMAKE_BUILD_TYPE=Debug"
    BUILD_DIR="build"
  fi
  [ "${CLEAN}" == 1 ] && BUILD_ADD_OPTS="${BUILD_ADD_OPTS} --clean-first"
  set -e
  set -o xtrace

  # mkdir build 2> /dev/null

  export CCACHE_DISABLE=1
  export ${MAKE_ADD_OPTS}
#        -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++ \
  "${CMAKE_BINARY}" \
        -DKNP_BUILD_TESTS=true \
        -DCMAKE_INSTALL_PREFIX="install" \
        -DCMAKE_EXPORT_COMPILE_COMMANDS=On \
        ${CMAKE_ADD_OPTS} \
        -B "${BUILD_DIR}" \
        -S . && \
  "${CMAKE_BINARY}" --build "${BUILD_DIR}" --parallel ${BUILD_ADD_OPTS}
}


while getopts "rcvh" o; do
  case "${o}" in
    h)
      usage
      exit 0
    ;;
    c)
      CLEAN=1
    ;;
    v)
      VERBOSE=1
    ;;
    r)
      RELEASE=1
    ;;
    *)
      usage
      exit 1
    ;;
  esac
done

shift $((OPTIND-1))

build

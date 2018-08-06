#!/bin/bash

BUILD_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null && pwd )/build"
BUILD_TARGET=${1:-all}; shift
BUILD_DEBUG=`echo $@ | grep -q '\-\-debug' && echo true`

rm -rf ${BUILD_DIR} 2 > /dev/null
test -d ${BUILD_DIR} || mkdir ${BUILD_DIR}

if [[ ! "${BUILD_DEBUG}" == "true" ]]; then
    export CC=`which arm-linux-gnueabi-gcc`
fi

EXIT_STATUS=0
cd ${BUILD_DIR}
    cmake -DDEBUG=${BUILD_DEBUG} .. && \
        make ${BUILD_TARGET}

    EXIT_STATUS=$?
cd - >/dev/null

exit ${EXIT_STATUS}
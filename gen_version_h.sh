#!/bin/bash

FILE="src/common/version.h"
GIT_SHORT_HASH=$(git rev-parse --short --verify HEAD)
GIT_LONG_HASH=$(git rev-parse --verify HEAD)
DATE=$(date +'%Y-%m-%d@%H:%M:%S')
OPCODES=$(cat src/common/opcodes.h | grep OPCODE | wc -l)

define() {
    echo "#define ${1} \"${2}\"" >> $FILE
}

rm -f $FILE
touch $FILE

define GIT_SHORT_HASH $GIT_SHORT_HASH
define GIT_LONG_HASH $GIT_LONG_HASH
define DATE $DATE
define OPCODE_COUNT $OPCODES


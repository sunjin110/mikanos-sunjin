#!/bin/bash

if [ "$(uname)" == 'Darwin' ]; then
OS='Mac'
elif [ "$(expr substr $(uname -s) 1 5)" == 'Linux' ]; then
OS='Linux'
endif

ACTIVE_PLATFORM="MikanLoaderPkg/MikanLoaderPkg.dsc"
TARGET="DEBUG"
TARGET_ARCH="X64"
# TOOL_CHAIN_TAG="CLANG38" # linuxのみ利用かのう
TOOL_CHAIN_TAG="CLANGPDB"
_3="0x3"

CONFIG_FILE_PATH="${HOME}/develop/edk2/Conf/target.txt"

# backup
cp ${CONFIG_FILE_PATH} ${CONFIG_FILE_PATH}.backup

update_or_add_line() {
    local key=$1
    local value=$2

    if grep -q "^${key} " ${CONFIG_FILE_PATH}; then
        # sed -i '' "s#^${key}[[:space:]]*=[[:space:]]*.*#${key} = ${value}#" ${CONFIG_FILE_PATH}
        sed "s#^${key}[[:space:]]*=[[:space:]]*.*#${key} = ${value}#" ${CONFIG_FILE_PATH}
    else
        echo "${key} = ${value}" >> ${CONFIG_FILE_PATH}
    fi
}

update_or_add_line "ACTIVE_PLATFORM" ${ACTIVE_PLATFORM}
update_or_add_line "TARGET" $TARGET
update_or_add_line "TARGET_ARCH" $TARGET_ARCH
update_or_add_line "TOOL_CHAIN_TAG" ${TOOL_CHAIN_TAG}
update_or_add_line "3" ${_3}

#!/bin/sh
set -e

TOOLCHAIN_URL='https://toolchains.bootlin.com/downloads/releases/toolchains/armv7-eabihf/tarballs/armv7-eabihf--musl--stable-2020.08-1.tar.bz2'
TOOLCHAIN_NAME='armv7_eabihf_musl_toolchain'
TOOLCHAIN_ARCHIVE=$TOOLCHAIN_NAME.tar.bz2
TOOLCHAIN_DIR=$TOOLCHAIN_NAME

download_toolchain() {
    curl -o $TOOLCHAIN_ARCHIVE $TOOLCHAIN_URL
}

extract_toolchain() {
    extract_folder=$(tar -tf $TOOLCHAIN_ARCHIVE | head -n 1)
    tar -xf $TOOLCHAIN_ARCHIVE

    mv $extract_folder $TOOLCHAIN_DIR

    rm "$TOOLCHAIN_ARCHIVE"
}

get_toolchain() {
    download_toolchain
    extract_toolchain
}

if [ ! -d "$TOOLCHAIN_DIR" ]
then
    get_toolchain
fi

echo -e "export ARCH=arm\nexport CROSS_COMPILE=$PWD/$TOOLCHAIN_DIR/bin/arm-buildroot-linux-musleabihf-" > toolchain.env
echo 'Please run ``source toolchain.env``'

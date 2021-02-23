# mfrc522-linux
Linux driver for the MFRC522 chip, written in C and Rust

## Setup
There are two solutions to build this driver:
- Using a cross compilation toolchain
- Using a docker container

### Build with a cross compilation toolchain
If you don't have a cross compilation toolchain, run ``activate.sh`` to download an arm toolchain.
If you already have one, don't forget to set ``ARCH`` and ``CROSS_COMPILE`` environment variables.

### Build and run the docker container
#### Build the docker image
`docker build . --build-arg user_id=$(id -u $USER) --build-arg user_name=$USER -t linux-build-container # or any other tag you want`

You only need to build the image once, as long as you don't change anything in the Dockerfile.

#### Start the container in interactive mode
`docker run -it -v $PWD:/app linux-build-container # or the above tag`

This will pop a shell and allow you to build linux from a Debian environment with the
necessary packages.

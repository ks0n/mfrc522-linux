# mfrc522-linux
Linux driver for the MFRC522 chip, written in C and Rust

## Running the container

### Build the docker image

`docker build . -t linux-build-container # or any other tag you want`

You only need to build the image once, as long as you don't change anything in the Dockerfile

### Start the container in interactive mode

`docker run -it linux-build-container # or the above tag`

This will pop a shell and allow you to build linux from a Debian environment with the
necessary packages

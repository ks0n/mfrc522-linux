# mfrc522-linux

Linux driver for the MFRC522 chip, written in C and Rust

# General instructions

Remember to clone the submodules as well. This can be done by using one of the following
commands:
- `git clone --recursive https://github.com/ks0n/mfrc522-linux`
- Cloning the project and running `git submodule update --init`

## Module Features

```sh
sudo insmod mfrc522.ko
```

A few commands are available:

|Command|Arguments|Example|Description|
|---|---|---|---|
|``mem_read``|None|``mem_read``|Read the internal memory of the MFRC522 to the driver's internal buffer. Access the content of this buffer by ``read``ing the device, e.g `cat /dev/mfrc522_misc`|
|``mem_write``|[Length of the data]:[Extra]|``mem_write:4:mfrc``|Write to the internal memory of the MFRC522|
|``get_version``|None|``get_version``|Display the MFRC522's hardware version (v1 or v2)|
|``gen_rand_id``|None|``gen_rand_id``|Generate a 10-byte-wide random number and store it in the MFRC522's internal memory. Use ``mem_read`` to read it|
|``debug``|[Mode (On/Off)]|``debug:on``|Enable debug information upon MFRC522 memory writes or reads(Only available in the C module)|

You can also fetch statistics via the ``sysfs`` about the driver's amount of read and written bits
(Only available in the C module).

## C module

### Setup

If you don't have a cross compilation toolchain, run ``activate.sh`` to download an arm toolchain.
If you already have one, don't forget to set ``ARCH`` and ``CROSS_COMPILE`` environment variables.
Remember to source ``toolchain.env`` to set the different environment variables expected
by the kernel

### Compiling the Raspberry kernel

This project was developed using
[the Raspberry fork of linux](https://github.com/raspberrypi/linux/), but also works with
the mainline kernel in 64 bits.

```sh
cd linux/
make bcm2709_defconfig
make
```

You will need to tweak the DTS in order for the Raspberry Pi to associate the MFRC522 with
our driver. A basic example is provided in the root of the repository. You can compile it
and use it on your Pi as is, by replacing it in ``/boot/bcm2710-rpi-3-b-plus.dtb``.

Follow [Raspberry's guide](https://www.raspberrypi.org/documentation/linux/kernel/building.md)
to flash your newly compiled kernel onto your SD Card.

### Compiling the driver

```sh
cd module/
make
```

This will produce a file named ``mfrc522.ko``. Copy this file to your Raspberry Pi.

## Rust module

*Welcome to the crab zone*

### Setup

Make sure that llvm and clang are at least in version 10.0.1.

Follow [this quickstart guide](https://github.com/ks0n/linux/blob/rust/Documentation/rust/quick-start.rst)
to get started on compiling a kernel with Rust support.

Remember to use the provided ``rust_kernel_config`` provided at the root of the repository.

### Compiling the ks0n kernel

```sh
cp rust_kernel_config linux-next/.config
cd linux-next
make LLVM=1 LLVM_IAS=1 ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu-
```

Be careful, the ethernet card will be disabled but Wifi will be working.

Since we are using LLVM, no need to download a cross compiling toolchain. We are also
compiling a 64 bit kernel, so be careful of that.

You will need to tweak the DTS in order for the Raspberry Pi to associate the MFRC522 with
our driver. A basic example is provided in the root of the repository. You can compile it
and use it on your Pi as is, by replacing it in ``/boot/bcm2710-rpi-3-b-plus.dtb``.

Follow [Raspberry's guide](https://www.raspberrypi.org/documentation/linux/kernel/building.md)
to flash your newly compiled kernel onto your SD Card.

### Compiling the driver

```sh
cd rust-module/
make LLVM=1 LLVM_IAS=1 ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu-
```

This will produce a file named ``mfrc522.ko``. Copy this file to your Raspberry Pi.

### Known issues

The rust driver has a non-deterministic rate of success. It will sometimes explode right
after SPI initialization, and sometimes only upon the module's deletion. We are currently
working on bringing an abstraction for the SPI part of the Kernel into the rust-for-linux
kernel, and development is quite hard and imprevisible. While the module does not always
work, we are close to completing it, and almost all C features are implemented!

// SPDX-License-Identifier: GPL-2.0

#![no_std]
#![feature(allocator_api)]

mod command;
mod parser;

use alloc::boxed::Box;
use core::pin::Pin;
use kernel::prelude::*;
use kernel::{
    chrdev, cstr,
    file_operations::{FileOpener, FileOperations},
    spi,
    spi_method,
};

module! {
    type: Mfrc522Driver,
    name: b"mfrc522",
    author: b"ks0n",
    description: b"MFRC522 NFC Tag reader SPI Driver",
    license: b"GPL v2",
    params: {
    },
}

spi_method! {
    fn mfrc522_probe(spi_device: SpiDevice) -> KernelResult {
        pr_info!("Rust SPI is ON!!\n");

        Err(kernel::Error::from_kernel_errno(-1))
    }
}

struct Mfrc522FileOps;

impl FileOpener<()> for Mfrc522FileOps {
    fn open(_ctx: &()) -> KernelResult<Self::Wrapper> {
        pr_info!("rust file was opened!\n");
        Ok(Box::try_new(Self)?)
    }
}

impl FileOperations for Mfrc522FileOps {
    type Wrapper = Box<Self>;

    kernel::declare_file_operations!();
}

struct Mfrc522Driver {
    _dev: Pin<Box<chrdev::Registration<1>>>,
}

impl KernelModule for Mfrc522Driver {
    fn init() -> KernelResult<Self> {
        pr_info!("Rust character device sample (init)\n");

        let mut spi_reg =
            spi::DriverRegistration::new(&THIS_MODULE, cstr!("mfrc522")).with_probe(mfrc522_probe);
        spi_reg.register()?;

        let mut chrdev_reg =
            chrdev::Registration::new_pinned(cstr!("rust_chrdev"), 0, &THIS_MODULE)?;

        chrdev_reg.as_mut().register::<Mfrc522FileOps>()?;

        Ok(Mfrc522Driver { _dev: chrdev_reg })
    }
}

impl Drop for Mfrc522Driver {
    fn drop(&mut self) {
        pr_info!("Rust character device sample (exit)\n");
    }
}

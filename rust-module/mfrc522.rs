// SPDX-License-Identifier: GPL-2.0

#![no_std]
#![feature(allocator_api)]

mod command;
mod mfrc522_spi;
mod parser;

use command::CommandSuccess;
use parser::Parser;

use alloc::boxed::Box;
use core::pin::Pin;
use kernel::prelude::*;
use kernel::{
    cstr,
    file_operations::{File, FileOpener, FileOperations},
    miscdev, spi,
    spi::SpiDevice,
    spi_method,
    user_ptr::{UserSlicePtrReader, UserSlicePtrWriter},
    Error,
};

pub static mut SPI_DEVICE: Option<SpiDevice> = None;

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
    fn mfrc522_probe(mut spi_device: SpiDevice) -> KernelResult {
        pr_info!("[MFRC522-RS] SPI Registered\n");

        let version = match mfrc522_spi::get_version(&mut spi_device) {
            Ok(v) => v,
            Err(_) => return Err(kernel::Error::from_kernel_errno(-1))
        };

        pr_info!("[MFRC522-RS] MFRC522 {:?} detected\n", version);

        unsafe {SPI_DEVICE = Some(spi_device)};

        Ok(())
    }
}

struct Mfrc522FileOps;

impl FileOpener<()> for Mfrc522FileOps {
    fn open(_ctx: &()) -> KernelResult<Self::Wrapper> {
        pr_info!("[MFRC522-RS] File opened\n");

        Ok(Box::try_new(Self)?)
    }
}

impl FileOperations for Mfrc522FileOps {
    type Wrapper = Box<Self>;

    kernel::declare_file_operations!(read, write);

    fn read(
        &self,
        _file: &File,
        _data: &mut UserSlicePtrWriter,
        _offset: u64,
    ) -> KernelResult<usize> {
        pr_info!("[MFRC522-RS] Being read from\n");

        Ok(0)
    }

    fn write(&self, data: &mut UserSlicePtrReader, _: u64) -> KernelResult<usize> {
        let kernel_vec = data.read_all()?;

        // FIXME: Should we use from_utf8 and return an error on invalid UTF8?
        // If not, document SAFETY here too
        let user_input = unsafe { core::str::from_utf8_unchecked(&kernel_vec) };

        pr_info!("[MFRC522-RS] Being written to: {}\n", user_input);

        // TODO: Implement core::convert::To<KernelError> for ParseError?
        // let cmd = parser::Parse(...)?;
        let cmd = match Parser::parse(user_input) {
            Err(_) => return Err(Error::EINVAL),
            Ok(cmd) => cmd,
        };

        if unsafe { SPI_DEVICE.is_none() } {
            pr_info!("[MFRC522-RS] Can not talk to the device, MFRC522 not present");
            return Err(Error::EPERM);
        }

        let mut answer = [0u8; command::MAX_DATA_LEN];

        // FIXME: We need to store the answer, if some bytes were written, into the driver's
        // state
        match cmd.execute(&mut answer) {
            Err(_) => Err(Error::EINVAL),
            // FIXME: Once the Command API is reworked, this will make more sense
            Ok(CommandSuccess::BytesWritten(amount)) => Ok(amount),
            Ok(CommandSuccess::NoAnswer) => Ok(0),
            _ => Err(Error::EINVAL),
        }
    }
}

struct Mfrc522Driver {
    _spi: Pin<Box<spi::DriverRegistration>>,
    _misc: Pin<Box<miscdev::Registration>>,
}

impl KernelModule for Mfrc522Driver {
    fn init() -> KernelResult<Self> {
        pr_info!("[MFRC522-RS] Init\n");

        let misc =
            miscdev::Registration::new_pinned::<Mfrc522FileOps>(cstr!("mfrc522_chrdev"), None, ())?;

        let mut spi = spi::DriverRegistration::new_pinned(&THIS_MODULE, cstr!("mfrc522"))?;

        Ok(Mfrc522Driver {
            _spi: spi,
            _misc: misc,
        })
    }
}

impl Drop for Mfrc522Driver {
    fn drop(&mut self) {
        pr_info!("[MFRC522-RS] Exit\n");
    }
}

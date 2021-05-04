use kernel::spi::{Spi, SpiDevice};
use kernel::{KernelResult, Error};
use core::{slice, mem};

enum Mfrc522Register {
    Version = 0x37,
}

enum Mfrc522Version{
    Version1 = 0x91,
    Version2 = 0x92,
    NotMfrc522,
}

impl From<u8> for Mfrc522Version {
    fn from(value: u8) -> Self {
        match value {
            0x91 => Self::Version1,
            0x92 => Self::Version2,
            _ => Self::NotMfrc522,
        }
    }
}

enum AddressByteMode {
    Read = 0,
    Write = 1,
}

#[repr(packed)]
struct AddressByte {
    addr: Mfrc522Register,
    mode: AddressByteMode,
}

impl AddressByte {
    fn new(addr: Mfrc522Register, mode: AddressByteMode) -> Self {
        AddressByte {
            addr,
            mode,
        }
    }
}

fn register_read(dev: &mut SpiDevice, reg: Mfrc522Register, read_buf: &mut [u8], read_len: u8) -> KernelResult {
    let address_byte = AddressByte::new(reg, AddressByteMode::Read);
    let address_byte = unsafe { 
        slice::from_raw_parts((&address_byte as *const AddressByte) as *const u8, mem::size_of::<AddressByte>())
    };

    for i in 0..read_len as usize {
        let ret = Spi::write_then_read(dev, address_byte, 1, &mut read_buf[i..i+1], 1);

        if ret.is_err() {
            return ret;
        }
    }

    Ok(())
}

fn get_version(dev: &mut SpiDevice) -> Result<Mfrc522Version, Error> {
    let mut version = [0 as u8];

    let ret = register_read(dev, Mfrc522Register::Version, &mut version, 1);

    match ret {
        Err(e) => return Err(e),
        _ => ()
    };

    match Mfrc522Version::from(version[0]) {
        NotMfrc522 => Err(Error::EINVAL),
        val => Ok(val),
    }
}

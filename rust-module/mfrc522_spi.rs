use core::{mem, slice};
use kernel::spi::{Spi, SpiDevice};
use kernel::{Error, KernelResult};

/// Address of the MFRC522 registers, Table 20 section 9.2
#[derive(Clone, Copy)]
pub enum Mfrc522Register {
    /// VersionReg register, section 9.3.4.8
    Version = 0x37,
}

/// Describe the different possible value of VersionReg register, section 9.3.4.8
#[derive(Debug)]
pub enum Mfrc522Version {
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

/// Represent the SPI address byte mode, Table 8 of section 8.1.2.3
#[derive(Clone, Copy)]
#[repr(u8)]
enum AddressByteMode {
    Read = 0,
    Write = 1,
}

/// Represent the SPI address byte, section 8.1.2.3
#[repr(packed)]
struct AddressByte {
    addr: Mfrc522Register,
    mode: AddressByteMode,
}

impl AddressByte {
    fn new(addr: Mfrc522Register, mode: AddressByteMode) -> Self {
        AddressByte { addr, mode }
    }

    /// Convert the AddressByte to a real byte encoded as described in Table 8
    /// of section 8.1.2.3
    fn to_byte(&self) -> u8 {
        (self.addr as u8 & 0b00111111) << 1 | self.mode as u8 & 0b1
    }
}

/// Read an MFRC522 register
fn register_read(
    dev: &mut SpiDevice,
    reg: Mfrc522Register,
    read_buf: &mut [u8],
    read_len: u8,
) -> KernelResult {
    let address_byte = AddressByte::new(reg, AddressByteMode::Read).to_byte();
    let address_byte_slice = &[address_byte];

    for i in 0..read_len as usize {
        Spi::write_then_read(dev, address_byte_slice, 1, &mut read_buf[i..i + 1], 1)?;
    }

    Ok(())
}

/// Get the MFRC522 version stored in VersionReg register, section 9.3.4.8
pub fn get_version(dev: &mut SpiDevice) -> Result<Mfrc522Version, Error> {
    let mut version = [0u8];

    register_read(dev, Mfrc522Register::Version, &mut version, 1)?;

    match Mfrc522Version::from(version[0]) {
        NotMfrc522 => Err(Error::EINVAL),
        val => Ok(val),
    }
}

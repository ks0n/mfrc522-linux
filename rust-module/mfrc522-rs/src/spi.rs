#![allow(dead_code)]

#[repr(u8)]
pub enum ReadWrite {
    Read = 0,
    Write,
}

#[repr(u8)]
pub enum RegAddress {
    Command = 0x1,
    FifoData = 0x9,
    FifoLevel = 0xA,
    Version = 0x37,
}

pub struct AddressByte;

impl AddressByte {
    pub fn new(addr: RegAddress, rw: ReadWrite) -> u8 {
        // The LSb is always 0
        (rw as u8 & 1) << 7 | (addr as u8 & 0b00111111) << 1
    }
}

pub enum Version {
    Unknown,
    Version1,
    Version2,
}

pub enum SpiError {
    Fuck,
}

pub struct Spi;

impl Spi {
    fn register_read(/* FIXME: Client */
        reg: RegAddress,
        buff: &mut [u8],
        read_len: u8) -> Result<u8, i32> {
        let mut ret = 0i32;
        let addr_byte = AddressByte::new(reg, ReadWrite::Read);

        for i in 0..read_len {
            // unsafe {
            //     ret = spi_write_then_read(/* client , */ &addr_byte, 1, (buff + i).as_ptr(), 1)
            // }
            if ret < 0 {
                return Err(ret);
            }
        }

        Ok(read_len)
    }

    pub fn get_version() -> Result<Version, SpiError> {
        let mut version = [0u8;1];

        match Spi::register_read(RegAddress::Version, &mut version, 1) {
            Err(_) => Err(SpiError::Fuck),
            Ok(v) => Ok(match v {
                0x91 => Version::Version1,
                0x92 => Version::Version2,
                _ => Version::Unknown,
            })
        }
    }
}

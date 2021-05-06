use kernel::spi::{Spi, SpiDevice};
use kernel::{pr_info, Error, KernelResult};

use super::{Mfrc522CommandByte, Mfrc522Command, Mfrc522PowerDown, Mfrc522Receiver};

/// Address of the MFRC522 registers, Table 20 section 9.2
#[derive(Clone, Copy)]
#[allow(dead_code)]
pub enum Mfrc522Register {
    /// VersionReg register, section 9.3.4.8
    Version = 0x37,
    /// FIFO Level register, section 9.3.1.11
    FifoLevel = 0xA,
    /// FIFO Data register, section FIXME
    FifoData = 0x9,
    /// Command register, section FIXME
    Command = 0x1,
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

/// SPI API specific to the MFRC522
pub struct Mfrc522Spi;

impl Mfrc522Spi {
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

    /// Write to an MFRC522 register
    fn register_write(dev: &mut SpiDevice, reg: Mfrc522Register, value: u8) -> KernelResult {
        let address_byte = AddressByte::new(reg, AddressByteMode::Write).to_byte();
        let data = &[address_byte, value];

        Spi::write(dev, data, 2)
    }

    /// Get the MFRC522 version stored in VersionReg register, section 9.3.4.8
    pub fn get_version(dev: &mut SpiDevice) -> KernelResult<Mfrc522Version> {
        let mut version = [0u8];

        Mfrc522Spi::register_read(dev, Mfrc522Register::Version, &mut version, 1)?;

        match Mfrc522Version::from(version[0]) {
            Mfrc522Version::NotMfrc522 => Err(Error::EINVAL),
            val => Ok(val),
        }
    }

    /// Get the current FIFO level of the MFRC522
    pub fn fifo_level(dev: &mut SpiDevice) -> KernelResult<u8> {
        let mut level = [0u8];

        Mfrc522Spi::register_read(dev, Mfrc522Register::FifoLevel, &mut level, 1)?;

        let fifo_level = level[0] & 0x7F; // FIFO Level Mask

        pr_info!("[MFRC522-RS] Fifo level: {}\n", fifo_level);

        Ok(fifo_level)
    }

    /// Read data from the MFRC522's FIFO
    pub fn fifo_read(dev: &mut SpiDevice, data: &mut [u8]) -> KernelResult<u8> {
        let fifo_level = Self::fifo_level(dev)?;

        Self::register_read(dev, Mfrc522Register::FifoData, data, fifo_level)?;

        Ok(fifo_level)
    }

    /// Write data to the MFRC522's FIFO
    pub fn fifo_write(dev: &mut SpiDevice, data: &[u8]) -> KernelResult {
        for byte in data {
            Mfrc522Spi::register_write(dev, Mfrc522Register::FifoData, *byte)?;
        }

        Ok(())
    }

    pub fn fifo_flush(dev: &mut SpiDevice) -> KernelResult {
        let flush_byte: u8 = 1u8 << 7;

        Mfrc522Spi::register_write(dev, Mfrc522Register::FifoLevel, flush_byte)?;

        Ok(())
    }

    /// Wait for a command to finish executing
    fn wait_for_command(dev: &mut SpiDevice) -> KernelResult {
        loop {
            let current_cmd = Mfrc522Spi::read_command(dev)?;
            if current_cmd == Mfrc522Command::Idle {
                break;
            }
        }

        Ok(())
    }

    /// Send a command to the MFRC522
    pub fn send_command(dev: &mut SpiDevice, cmd: Mfrc522Command) -> KernelResult {
        let cmd_byte =
            Mfrc522CommandByte::new(cmd, Mfrc522PowerDown::Off, Mfrc522Receiver::On).to_byte();

        Mfrc522Spi::register_write(dev, Mfrc522Register::Command, cmd_byte)?;

        Mfrc522Spi::wait_for_command(dev)
    }

    /// Read the current command byte
    pub fn read_command(dev: &mut SpiDevice) -> KernelResult<Mfrc522Command> {
        let mut cmd_byte = [0u8];

        Mfrc522Spi::register_read(dev, Mfrc522Register::Command, &mut cmd_byte, 1)?;

        Ok(Mfrc522CommandByte::from_byte(cmd_byte[0]).cmd)
    }
}


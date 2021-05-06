const REC_SHIFT: u8 = 5;
const PWR_SHIFT: u8 = 4;
const PWR_MASK: u8 = 0x1;
const CMD_MASK: u8 = 0xF;

pub struct Mfrc522CommandByte {
    pub cmd: Mfrc522Command,
    pub power: Mfrc522PowerDown,
    pub receiver: Mfrc522Receiver,
}

#[repr(u8)]
#[derive(Clone, Copy)]
#[allow(dead_code)]
pub enum Mfrc522PowerDown {
    Off = 0x0,
    On = 0x1,
}

#[repr(u8)]
#[derive(Clone, Copy)]
#[allow(dead_code)]
pub enum Mfrc522Receiver {
    Off = 0x0,
    On = 0x1,
}

#[repr(u8)]
#[derive(Clone, Copy, PartialEq)]
#[allow(dead_code)]
pub enum Mfrc522Command {
    Idle = 0b0000,
    Mem = 0b0001,
    Generaterandomid = 0b0010,
    CalcCrc = 0b0011,
    Transmit = 0b0100,
    NoCmdChange = 0b0111,
    ReCeive = 0b1000,
    Transceive = 0b1100,
    MfAuthent = 0b1110,
    SoftReset = 0b1111,
}

impl Mfrc522CommandByte {
    pub fn new(cmd: Mfrc522Command, power: Mfrc522PowerDown, receiver: Mfrc522Receiver) -> Self {
        Self {
            cmd,
            power,
            receiver,
        }
    }

    pub fn to_byte(&self) -> u8 {
        (self.receiver as u8) << REC_SHIFT | (self.power as u8) << PWR_SHIFT | (self.cmd as u8)
    }

    pub fn from_byte(byte: u8) -> Self {
        let receiver = byte >> REC_SHIFT;
        let power = byte >> PWR_SHIFT & PWR_MASK;
        let cmd = byte & CMD_MASK;

        // FIXME: Implement TryFrom for enums instead
        macro_rules! from_byte {
            ($value:ident, $t:ty) => {
                unsafe { core::mem::transmute::<u8, $t>($value) }
            };
        }

        Mfrc522CommandByte::new(
            from_byte!(cmd, Mfrc522Command),
            from_byte!(power, Mfrc522PowerDown),
            from_byte!(receiver, Mfrc522Receiver),
        )
    }
}

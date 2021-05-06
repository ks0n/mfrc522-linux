use crate::mfrc522_inner::{Mfrc522Spi, Mfrc522Command};

/// Maximum amount of bytes handled by the MFRC522's internal memory
pub const MAX_DATA_LEN: usize = 25;

/// Answer type in which the executor will write, if necessary. This represents the inner
/// memory of the MFRC522, and is not guaranteed to be valid ASCII or UTF-8.
pub type Answer = [u8; MAX_DATA_LEN];

/// Result of a successful command execution
pub enum CommandSuccess {
    /// Amount of bytes written if any
    BytesWritten(usize),
    /// Amount of bytes read if any
    BytesRead(usize),
    /// The command returned successfully without an answer
    NoAnswer,
}

/// Return type of command execution functions
pub type CommandResult = Result<CommandSuccess, kernel::Error>;

/// Possible commands accepted by the MFRC522
#[derive(Debug, PartialEq)]
pub enum Cmd {
    /// Write data to the MFRC522's internal buffer
    MemWrite,
    /// Read data from the MFRC522's internal buffer
    MemRead,
    /// Display the version of the connected MFRC522
    GetVersion,
    /// Generate a 10-byte wide random ID
    GenRand,
}

impl Cmd {
    /// Get the corresponding Cmd from a text command, None if invalid
    pub fn from_str(cmd: &str) -> Option<Cmd> {
        match cmd {
            "mem_read" => Some(Cmd::MemRead),
            "mem_write" => Some(Cmd::MemWrite),
            "get_version" => Some(Cmd::GetVersion),
            "gen_rand_id" => Some(Cmd::GenRand),
            _ => None,
        }
    }

    /// Does the command need arguments or not
    pub fn has_args(&self) -> bool {
        match &self {
            Cmd::MemRead | Cmd::GenRand | Cmd::GetVersion => false,
            Cmd::MemWrite => true,
        }
    }
}

/// Argument of a command, if present
#[derive(Debug, PartialEq)]
struct CommandArg {
    /// Length of the data as given by the user
    data_len: u8,
    /// Extra data given by the user
    data: [u8; MAX_DATA_LEN],
}

/// User command, containing a command and a possible argument
#[derive(Debug, PartialEq)]
pub struct Command {
    /// Command required by the user
    cmd: Cmd,
    /// Argument as required by the user
    arg: Option<CommandArg>,
}

impl CommandArg {
    /// Create a new command argument from data and its length
    fn new(data_len: u8, data: [u8; MAX_DATA_LEN]) -> CommandArg {
        CommandArg { data_len, data }
    }
}

impl Command {
    /// Create a new complete command wiht an argument
    pub fn new(cmd: Cmd, data_len: u8, data: [u8; MAX_DATA_LEN]) -> Command {
        Command {
            cmd,
            arg: Some(CommandArg::new(data_len, data)),
        }
    }

    /// Create a simple command without an argument
    pub fn new_simple(cmd: Cmd) -> Command {
        Command { cmd, arg: None }
    }

    fn mem_write(&self) -> CommandResult {
        let dev = unsafe { &mut crate::SPI_DEVICE.unwrap() };

        // FIXME: No unwrap
        Mfrc522Spi::fifo_write(dev, &self.arg.as_ref().unwrap().data)?;
        Mfrc522Spi::send_command(dev, Mfrc522Command::Mem)?;

        Ok(CommandSuccess::BytesWritten(MAX_DATA_LEN))
    }

    fn mem_read(&self, answer: &mut Answer) -> CommandResult {
        todo!()
    }

    fn get_version(&self) -> CommandResult {
        // As we can't get here if the device is not present, unwrap is safe
        Mfrc522Spi::get_version(unsafe { &mut crate::SPI_DEVICE.unwrap() })?;

        Ok(CommandSuccess::NoAnswer)
    }

    fn generate_random_id(&self) -> CommandResult {
        todo!()
    }

    /// Execute the required command, sending and receiving information to the MFRC522.
    pub fn execute(&self, answer: &mut Answer) -> CommandResult {
        match &self.cmd {
            Cmd::MemWrite => self.mem_write(),
            Cmd::MemRead => self.mem_read(answer),
            Cmd::GetVersion => self.get_version(),
            Cmd::GenRand => self.generate_random_id(),
        }
    }
}

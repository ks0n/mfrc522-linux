/// Maximum amount of bytes handled by the MFRC522's internal memory
pub const MAX_DATA_LEN: usize = 25;

/// Possible commands accepted by the MFRC522
#[repr(u8)]
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
        CommandArg {
            data_len,
            data
        }
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
        Command {
            cmd,
            arg: None,
        }
    }

    fn mem_write(&self) -> Option<String> {
        todo!()
    }

    fn mem_read(&self) -> Option<String> {
        todo!()
    }

    fn get_version(&self) -> Option<String> {
        todo!()
    }

    fn generate_random_id(&self) -> Option<String> {
        todo!()
    }

    /// Execute the required command, sending and receiving information to the MFRC522
    pub fn execute(&self) -> Option<String> {
        match &self.cmd {
            Cmd::MemWrite => self.mem_write(),
            Cmd::MemRead => self.mem_read(),
            Cmd::GetVersion => self.get_version(),
            Cmd::GenRand => self.generate_random_id(),
        }
    }
}

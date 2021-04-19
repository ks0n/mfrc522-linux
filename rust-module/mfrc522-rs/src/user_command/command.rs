const MAX_DATA_LEN: usize = 25;

#[repr(u8)]
#[derive(Debug, PartialEq)]
pub enum Cmd {
    MemWrite,
    MemRead,
    GetVersion,
    GenRand,
}

#[derive(Debug, PartialEq)]
struct CommandArg {
    data_len: u8,
    data: [u8; MAX_DATA_LEN],
}

#[derive(Debug, PartialEq)]
pub struct Command {
    cmd: Cmd,
    arg: Option<CommandArg>,
}

impl CommandArg {
    pub fn new(data_len: u8, data: [u8; MAX_DATA_LEN]) -> CommandArg {
        CommandArg {
            data_len,
            data
        }
    }
}

impl Command {
    pub fn new(cmd: Cmd, data_len: u8, data: [u8; MAX_DATA_LEN]) -> Command {
        Command {
            cmd,
            arg: Some(CommandArg::new(data_len, data)),
        }
    }

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

    pub fn execute(&self) -> Option<String> {
        match &self.cmd {
            Cmd::MemWrite => self.mem_write(),
            Cmd::MemRead => self.mem_read(),
            Cmd::GetVersion => self.get_version(),
            Cmd::GenRand => self.generate_random_id(),
        }
    }
}

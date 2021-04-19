const MFRC522_MAX_DATA_LEN: usize = 25;

struct CommandArg {
    data_len: u8,
    data: [u8; MFRC522_MAX_DATA_LEN],
}

pub struct Command {
    cmd: u8,
    arg: Option<CommandArg>,
}

use super::command::{Cmd, Command};

#[derive(Debug, PartialEq)]
pub enum ParseError {
    UnknownCommand,
    InvalidArgNumber,
}

pub struct Parser;

impl Parser {
    pub fn parse(_input: &str) -> Result<Command, ParseError> {
        Ok(Command::new_simple(Cmd::MemRead))
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn valid_mem_read() {
        let cmd = Parser::parse("mem_read");

        assert_eq!(cmd, Ok(Command::new_simple(Cmd::MemRead)));
    }

    #[test]
    fn valid_get_version() {
        let cmd = Parser::parse("get_version");

        assert_eq!(cmd, Ok(Command::new_simple(Cmd::GetVersion)));
    }

    #[test]
    fn valid_gen_rand_id() {
        let cmd = Parser::parse("gen_rand_id");

        assert_eq!(cmd, Ok(Command::new_simple(Cmd::GenRand)));
    }

    #[test]
    fn valid_mem_write() {
        let cmd = Parser::parse("mem_write:3:Hey");

        assert_eq!(cmd, Ok(Command::new_simple(Cmd::MemWrite)));
    }

    #[test]
    fn invalid_mem_write() {
        let cmd = Parser::parse("mem_write");

        assert!(cmd.is_err());
    }

    #[test]
    fn invalid_mem_read() {
        let cmd = Parser::parse("mem_read:3:Hey");

        assert!(cmd.is_err());
    }

    #[test]
    fn invalid_len_arg() {
        let cmd = Parser::parse("mem_write:NotANumber:Hey");

        assert!(cmd.is_err());
    }

    #[test]
    fn invalid_len_arg_float() {
        let cmd = Parser::parse("mem_write:3.0:Hey");

        assert!(cmd.is_err());
    }

    #[test]
    fn invalid_len_arg_negative() {
        let cmd = Parser::parse("mem_write:-15:Hey");

        assert!(cmd.is_err());
    }

    #[test]
    fn invalid_len_arg_too_big() {
        let cmd = Parser::parse("mem_write:3599:Hey");

        assert!(cmd.is_err());
    }

    #[test]
    fn invalid_cmd() {
        let cmd = Parser::parse("not_a_cmd");

        assert!(cmd.is_err());
    }
}

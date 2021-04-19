use super::command::{Cmd, Command};

const SEPARATOR: char = ':';

/// Possible errors when parsing user input
#[derive(Debug, PartialEq)]
pub enum ParseError {
    /// No input was provided to the parser
    EmptyInput,
    /// The command required does not exist
    UnknownCommand,
    /// The command expected a different amount of arguments
    InvalidArgNumber,
}

/// Empty parser struct for static methods
pub struct Parser;

/// Result type used by the parser, containing a Command or a ParseError
pub type ParseResult = Result<Command, ParseError>;

impl Parser {
    fn parse_simple(input: &str) -> ParseResult {
        match Cmd::from_str(input) {
            None => Err(ParseError::UnknownCommand),
            Some(cmd) => Ok(Command::new_simple(cmd)),
        }
    }

    fn parse_complex(input: &str, idx: usize) -> ParseResult {
        todo!()
    }

    /// Parse an MFRC522 command from the user's input
    pub fn parse(input: &str) -> ParseResult {
        match input.find(SEPARATOR) {
            None => Parser::parse_simple(input),
            Some(first_sep_idx) => Parser::parse_complex(input, first_sep_idx),
        }
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

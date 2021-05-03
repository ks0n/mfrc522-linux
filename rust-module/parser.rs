use super::command::{Cmd, Command};
use core::str::FromStr;

const SEPARATOR: char = ':';
const INPUT_SIZE: usize = 25;

/// Possible errors when parsing user input
#[derive(Debug, PartialEq)]
pub enum ParseError {
    /// No input was provided to the parser
    EmptyInput,
    /// The command required does not exist
    UnknownCommand,
    /// The command expected a different amount of arguments
    InvalidArgNumber,
    /// The extra data len argument was not a number
    InvalidDataLen,
    /// The extra data len was a number, but was too important
    DataLenTooBig,
}

/// Empty parser struct for static methods
pub struct Parser;

/// Result type used by the parser, containing a Command or a ParseError
pub type ParseResult = Result<Command, ParseError>;

impl Parser {
    fn get_cmd(input: &str) -> Result<Cmd, ParseError> {
        match Cmd::from_str(input) {
            None => {
                if input.is_empty() {
                    Err(ParseError::EmptyInput)
                } else {
                    Err(ParseError::UnknownCommand)
                }
            }
            Some(cmd) => Ok(cmd),
        }
    }

    fn parse_simple(input: &str) -> ParseResult {
        let cmd = Parser::get_cmd(input)?;

        match cmd.has_args() {
            false => Ok(Command::new_simple(cmd)),
            true => Err(ParseError::InvalidArgNumber),
        }
    }

    fn get_data_len(input: &str) -> Result<u8, ParseError> {
        match u8::from_str(input) {
            Ok(value) => {
                if value > INPUT_SIZE as u8 {
                    Err(ParseError::DataLenTooBig)
                } else {
                    Ok(value)
                }
            }
            Err(_) => {
                if input.is_empty() {
                    Err(ParseError::InvalidArgNumber)
                } else {
                    Err(ParseError::InvalidDataLen)
                }
            }
        }
    }

    fn get_data(input: &str) -> Result<[u8; INPUT_SIZE], ParseError> {
        use core::cmp::min;

        let mut data: [u8; INPUT_SIZE] = [0; INPUT_SIZE];
        let len = min(input.len(), INPUT_SIZE);
        let tmp_data = &input.as_bytes()[0..len];

        for idx in 0..len {
            data[idx] = tmp_data[idx];
        }

        match len {
            0 => Err(ParseError::InvalidArgNumber),
            _ => Ok(data),
        }
    }

    fn parse_complex(input: &str) -> ParseResult {
        let mut split = input.split(SEPARATOR);
        let cmd = Parser::get_cmd(split.next().unwrap_or(""))?;
        let data_len = Parser::get_data_len(split.next().unwrap_or(""))?;
        let data = Parser::get_data(split.next().unwrap_or(""))?;

        match cmd.has_args() {
            true => Ok(Command::new(cmd, data_len, data)),
            false => Err(ParseError::InvalidArgNumber),
        }
    }

    /// Parse an MFRC522 command from the user's input
    pub fn parse(input: &str) -> ParseResult {
        match input.find(SEPARATOR) {
            None => Parser::parse_simple(input),
            Some(_) => Parser::parse_complex(input),
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
        let mut ref_data = [0u8; INPUT_SIZE];
        ref_data[0] = 'H' as u8;
        ref_data[1] = 'e' as u8;
        ref_data[2] = 'y' as u8;

        assert_eq!(cmd, Ok(Command::new(Cmd::MemWrite, 3, ref_data)));
    }

    #[test]
    fn invalid_mem_write() {
        let cmd = Parser::parse("mem_write");

        assert_eq!(cmd, Err(ParseError::InvalidArgNumber));
    }

    #[test]
    fn invalid_mem_read() {
        let cmd = Parser::parse("mem_read:3:Hey");

        assert_eq!(cmd, Err(ParseError::InvalidArgNumber));
    }

    #[test]
    fn data_len_over_25() {
        let cmd = Parser::parse("mem_write:26:Hey");

        assert_eq!(cmd, Err(ParseError::DataLenTooBig));
    }

    #[test]
    fn invalid_len_arg() {
        let cmd = Parser::parse("mem_write:NotANumber:Hey");

        assert_eq!(cmd, Err(ParseError::InvalidDataLen));
    }

    #[test]
    fn invalid_len_arg_float() {
        let cmd = Parser::parse("mem_write:3.0:Hey");

        assert_eq!(cmd, Err(ParseError::InvalidDataLen));
    }

    #[test]
    fn invalid_len_arg_negative() {
        let cmd = Parser::parse("mem_write:-15:Hey");

        assert_eq!(cmd, Err(ParseError::InvalidDataLen));
    }

    #[test]
    fn invalid_len_arg_too_big() {
        let cmd = Parser::parse("mem_write:3599:Hey");

        assert_eq!(cmd, Err(ParseError::InvalidDataLen));
    }

    #[test]
    fn invalid_cmd() {
        let cmd = Parser::parse("not_a_cmd");

        assert_eq!(cmd, Err(ParseError::UnknownCommand));
    }

    #[test]
    fn empty_input() {
        let cmd = Parser::parse("");

        assert_eq!(cmd, Err(ParseError::EmptyInput));
    }
}

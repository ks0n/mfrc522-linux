#![no_std]

pub mod parser;
pub mod command;

#[cfg(target_arch = "arm")]
use core::panic::PanicInfo;

#[cfg(target_arch = "arm")]
#[panic_handler]
fn mfrc522_panic(_info: &PanicInfo) -> ! {
    loop {}
}

#[no_mangle]
pub fn hello_rust() -> i32 {
    1
}

#[cfg(test)]
mod tests {
    #[test]
    fn it_works() {
    }
}

// Considering we're developing a kernel module, we do not have access to the standard
// library (which, for example, uses malloc for allocation). We will need to bridge the
// gap between the kernel headers and this rust file in order to use kernel functions,
// such as kalloc, strcpy, strncpy...
#![no_std]

pub mod command;
pub mod parser;

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

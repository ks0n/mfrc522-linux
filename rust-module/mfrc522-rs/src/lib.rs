// Considering we're developing a kernel module, we do not have access to the standard
// library (which, for example, uses malloc for allocation). We will need to bridge the
// gap between the kernel headers and this rust file in order to use kernel functions,
// such as kalloc, strcpy, strncpy...
#![no_std]

mod kernel;
use kernel::Kernel;

pub mod command;
pub mod parser;

use parser::Parser;

#[cfg(target_arch = "arm")]
use core::panic::PanicInfo;

#[cfg(target_arch = "arm")]
#[panic_handler]
fn mfrc522_panic(_info: &PanicInfo) -> ! {
    loop {}
}

#[no_mangle]
pub extern "C" fn mfrc522_read_rs() -> i32 { Kernel::printk("Coucou kernel"); 0 }

#[no_mangle]
pub extern "C" fn mfrc522_write_rs(buffer: *const u8, len: usize) -> i32 {
    use core::str;

    let input = unsafe {
        // SAFETY: The buffer and len is what the kernel gives us. It is valid
        let s = core::slice::from_raw_parts(buffer, len);

        // SAFETY: We know that it is valid ASCII. The Linux kernel does not handle
        // unicode input, as it gives us a `const char *` and not any form of wchar or other.
        str::from_utf8_unchecked(s)
    };

     match Parser::parse(input) {
         Ok(_) => 0,
         Err(_) => -1,
     }
}

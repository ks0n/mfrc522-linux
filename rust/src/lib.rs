#![no_std]

extern "C" {
    fn print(msg: *const u8) -> ();
}

#[no_mangle]
// #[link_section = ".init"]
pub extern "C" fn mfrc522_init_rs() -> i32 {
    unsafe { print("Hello from Rust\n\0".as_bytes().as_ptr()) };
    return 0;
}

#[no_mangle]
// #[link_section = ".exit"]
pub extern "C" fn mfrc522_exit_rs() {}

#[panic_handler]
fn my_panic(_info: &core::panic::PanicInfo) -> ! {
    loop {}
}

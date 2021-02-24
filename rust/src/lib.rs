#![no_std]

#[crate_type = "staticlib"]

#[no_mangle]
pub extern "C" fn mfrc522_init_rs() -> i32 {
    return 1
}

#[no_mangle]
pub extern "C" fn mfrc522_exit_rs() {
}

#[panic_handler]
fn my_panic(_info: &core::panic::PanicInfo) -> ! {
    loop {}
}

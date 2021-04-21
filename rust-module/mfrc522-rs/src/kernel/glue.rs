extern "C" {
    pub fn printk_stub(s: *const u8, len: usize);
}

#[no_mangle] extern "C" fn __aeabi_unwind_cpp_pr0() -> ! { panic!() }
#[no_mangle] extern "C" fn __aeabi_unwind_cpp_pr1() -> ! { panic!() }

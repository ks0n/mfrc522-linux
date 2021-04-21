mod glue;

pub struct Kernel;

impl Kernel {
    pub fn printk(s: &str) {
        unsafe { glue::printk_stub(s.as_ptr() as *const u8, s.len()) }
    }
}

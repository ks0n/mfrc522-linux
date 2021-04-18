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

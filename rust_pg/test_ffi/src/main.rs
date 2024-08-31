extern crate core;
use core::ffi::c_int;

extern "C" {
    fn add(a: c_int, b: c_int) -> c_int;
    fn static_call(a: c_int, b: c_int) -> c_int;
    fn dylib_call(a: c_int, b: c_int) -> c_int;
}

fn main() {
    println!("[Rust] Hello from Rust! 🦀");

    unsafe {
        println!("[Rust] Calling function in C..");

        let result = add(5000, 5);

        println!("[Rust] Result: {}", result);

        println!("[Rust static_call] Calling function in C..");

        let result = static_call(5000, 5);

        println!("[Rust static_call] Result: {}", result);

        println!("[Rust dylib_call] Calling function in C..");

        let result = dylib_call(5000, 5);

        println!("[Rust dylib_call] Result: {}", result);
    }
}

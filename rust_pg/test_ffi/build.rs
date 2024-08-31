use std::fs;

extern crate cc;

// src 不能包含 lib.rs 否则编译不成功
fn main() {
  
    cc::Build::new().file("c/add.c").compile("add");

    let libs_dir = "./c_libs";
    println!("cargo:rustc-link-search=native={}", libs_dir);
    println!("cargo:rustc-link-lib=dylib=dylib_for_rust");
    println!("cargo:rustc-link-lib=static=static_for_rust");
    
    if cfg!(target_os = "windows"){

        fs::copy("./c_libs/dylib_for_rust.dll", "./target/debug/dylib_for_rust.dll").expect("copy dll");
    }else{
        fs::copy("./c_libs/libdylib_for_rust.so", "./target/debug/libdylib_for_rust.so").expect("copy so");
    }
}

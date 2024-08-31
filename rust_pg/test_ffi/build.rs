extern crate cc;

fn main() {
  
    cc::Build::new().file("c/add.c").compile("add");

    let libs_dir = "./c_libs";
    println!("cargo:rustc-link-search=native={}", libs_dir);
    println!("cargo:rustc-link-lib=dylib=dylib_for_rust");
    println!("cargo:rustc-link-lib=static=static_for_rust");
}

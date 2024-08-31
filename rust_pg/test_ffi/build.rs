extern crate cc;

fn main() {
  
    cc::Build::new().file("c_libs/add.c").compile("add");
}

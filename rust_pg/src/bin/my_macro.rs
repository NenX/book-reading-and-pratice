use my_macro_derive::{HelloMacro,};
pub trait HelloMacroTrait {
    fn hello_macro();
}

#[derive(Debug, HelloMacro)]
struct MyStruct {
    a: i32,
    b: i32,
}
fn main() {
    MyStruct::hello_macro();
}
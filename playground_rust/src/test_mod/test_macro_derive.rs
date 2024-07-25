extern crate my_macro_derive;
use my_macro_derive::AnswerFn;
// use my_macro_derive::HelloMacro;

#[derive(AnswerFn)]
struct Struct;

// #[derive(HelloMacro)]
// struct Sunface;

pub fn test() {
    println!("TODO: test HelloMacro --> {}", answer());
    // println!("test_macro_derive --> {}", hello());
    // Sunface::hello_macro();

}

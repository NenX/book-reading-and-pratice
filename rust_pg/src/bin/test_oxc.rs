#![allow(clippy::print_stdout)]
use std::{fs, path::Path};

use oxc::allocator::Allocator;
// use oxc_allocator::Allocator;
use oxc::parser::{ParseOptions, Parser};
use oxc::span::SourceType;

// Instruction:
// create a `test.js`,
// run `cargo run -p oxc_parser --example parser`
// or `cargo watch -x "run -p oxc_parser --example parser"`

fn main() -> Result<(), String> {

    let name = String::from("test.js");


    let path = Path::new(&name);
    let source_text = fs::read_to_string(path).map_err(|_| format!("Missing '{name}'"))?;
    let source_type = SourceType::from_path(path).unwrap();

    let allocator = Allocator::default();
    let ret = Parser::new(&allocator, &source_text, source_type)
        .with_options(ParseOptions { parse_regular_expression: true, ..ParseOptions::default() })
        .parse();

    if true {
        println!("AST:");
    }

    if true {
        println!("Comments:");
  
    }

    if ret.errors.is_empty() {
        println!("Parsed Successfully.");
    } else {
        for error in ret.errors {
            let error = error.with_source_code(source_text.clone());
            println!("{error:?}");
            println!("Parsed with Errors.");
        }
    }

    Ok(())
}

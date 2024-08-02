use std::{collections::HashMap, fs::File, num::IntErrorKind};

use test_mod::{
    test_error, test_hashmap, test_iterator, test_macro_derive, test_macro_ruls, test_string_and_str
};
mod test_mod;
// use rand::Rng;
static ARR: [i32; 3] = [1, 2, 999];
fn main() {
    // test_macro_ruls::test();
    // test_macro_derive::test();
    // test_string_and_str::test();
    // test_iterator::test();
    //    test_hashmap::test();

    // test_error::test();

    let a:Result<i32,IntErrorKind> = Ok(123);
    let b = a.ok();
    let c = b.ok_or(IntErrorKind::Empty);
}

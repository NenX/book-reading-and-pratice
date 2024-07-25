
use test_mod::{test_iterator, test_macro_derive, test_macro_ruls, test_string_and_str};
mod test_mod;
// use rand::Rng;
static ARR: [i32; 3] = [1, 2, 999];
fn main() {
 
    test_macro_ruls::test();
    test_macro_derive::test();
    test_string_and_str::test();
    test_iterator::test();
}

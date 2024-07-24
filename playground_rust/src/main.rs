use std::io;

use rand::{thread_rng, Rng};
use test_mod::test_macro_ruls;
mod test_mod;
// use rand::Rng;
static ARR: [i32; 3] = [1, 2, 999];
fn main() {
 
    test_macro_ruls::test();
}

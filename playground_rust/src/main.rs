use std::{clone, collections::HashMap, fmt::Debug, fs::File, hash::Hash, num::IntErrorKind};

use test_mod::{
    test_error, test_hashmap, test_iterator, test_macro_derive, test_macro_ruls,
    test_string_and_str,
};
mod test_mod;
// use rand::Rng;
static ARR: [i32; 3] = [1, 2, 999];
fn main() {
    let mut m: HashMap<String, String> = HashMap::new();
    let mut m1: HashMap<i32, i32> = HashMap::new();
    let opt_raw = m.get(&"".to_string());
    let opt_raw1 = m1.get(&11);
    let opt_cpy = opt_raw1.copied();


    let a = Some(1);
    let b: Result<i32, IntErrorKind> = Ok(1);

    let c = a.ok_or(IntErrorKind::Empty);

    let d = b.ok();
}



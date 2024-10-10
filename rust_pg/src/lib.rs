use std::collections::HashMap;

pub mod magic_params;
pub mod my_dns;
pub mod my_ws;
pub mod tokio_util_pg;

pub fn test_lib_fn() -> HashMap<i32, &'static str> {
    let mut m = HashMap::new();
    m.insert(1, "v");
    m
}

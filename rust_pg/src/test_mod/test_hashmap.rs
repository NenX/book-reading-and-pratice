use std::collections::{hash_map::Entry, HashMap};

pub fn test() {
    let mut m = HashMap::new();
    m.insert("a", 2);
    m.insert("b", 4);
    m.insert("c", 6);

    let o = m.get("K");

    let v = o.unwrap_or(&3);
    println!("unwrap_or --> {v}");

    let v = o.copied().unwrap_or(3);
    println!("copied unwrap_or --> {v}");

    let v = o.unwrap_or_else(|| &99);
    println!("unwrap_or_else --> {v}");

    let v = o.copied().unwrap_or_default();
    println!("unwrap_or_else --> {v}");

    let v = m.get_mut("a");
    let mut my_v: i32 = 1;
    let vo = v.unwrap_or(&mut my_v);
    *vo = 99;

    let s = m.entry("c");
    let q = s.or_insert(99);

    println!("map -->  {:?} ", m);
    let s = m.entry("c");
    let q = s.or_insert(99);
    println!("map -->  {m:?}");
}

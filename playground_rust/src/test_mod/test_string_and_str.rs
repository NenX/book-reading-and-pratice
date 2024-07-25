use std::mem::size_of_val;

pub fn test() {
    let c1: char = '我';
    let c2: char = 'a';
    let c3 = b'a';
    println!(
        "================= test_string_and_str ===================== {} {} {}",
        size_of_val(&c1),
        size_of_val(&c2),
        size_of_val(&c3)
    );

    let word = "®a我";
    word.chars().for_each(|c| println!("str.chars {}", c));
    word.bytes().for_each(|c| println!("str.bytes {}", c));


    let word = word.to_string();

    word.chars().for_each(|c| println!("String.chars {}", c));
    word.bytes().for_each(|c| println!("String.bytes {}", c));
}

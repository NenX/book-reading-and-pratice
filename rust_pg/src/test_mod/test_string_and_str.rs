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

    let mut s2: String = String::from(" aabbcc ");
    let mut s1: &str = &mut s2;

    let mut x = s1.to_owned();
    x.push_str("string");
    println!("--->|{x}||{s1}|");
}



// #[derive(Debug)]
// struct Foo;

// impl Foo {
//     fn mutate_and_share(&mut self) -> &Self {
//         &*self
//     }
//     fn share(&self) {}
// }
// #[test]
// fn mainx() {
//     let mut foo = Foo;
//     let x = &mut foo;
//     let y: &Foo = x;
//     let y1: &Foo = &foo;
//     y.share();
//     println!("{:?}", 1);
// }

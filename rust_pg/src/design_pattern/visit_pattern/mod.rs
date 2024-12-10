use data::{Aa, Base, Bb};
use visits::PrintVisit;

mod data;
mod visits;

#[test]
fn test_visit() {
    let a = Aa {
        name: "I'm a".to_owned(),
    };
    let b = Bb { age: 22 };
    let v = PrintVisit;
    let a_str = a.access(&v);
    let b_str = b.access(&v);
    println!("a => {a_str}");
    println!("b => {b_str}");
}

use my_rs::magic_params::{trigger, Context, MyName, MyUrl};

fn main() {
    let c = Context {
        i: 22,
        u: "url".into(),
        n: "name".into(),
    };
    trigger(&c, |MyName(x): MyName, i: i32| println!("?? {x}  {i}"));
    trigger(&c, |MyUrl(x): MyUrl| println!("?? {x} "));
}


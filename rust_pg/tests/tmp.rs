#[test]
fn test() {
    let i = 22;

    let a = match i {
        22 if i < 3 => 22 ,
        22 if i > 3 => 33 ,
        _ => 0
    };
    println!("a ==> {a}")
}


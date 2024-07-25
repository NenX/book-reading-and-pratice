pub fn test() {
    println!(
        "================= test_iterator ===================== ",
    );
    let mut v = vec!["a".to_string(), "b".to_string()];

    for i in &v {
        println!("===> {}", i)
    }
    // for 内部需要获取所有权以将 v 变成 mut,
    let it = v.iter();
    for i in it {
      
        println!("===> {}", i)
    }

    let it = v.into_iter();

    for i in it {
        println!("===> {}", i)
    }
}

use my_macro_rules::find_min;

pub fn test() {
    println!("{}", find_min!(12, 5, 35, 99));
}

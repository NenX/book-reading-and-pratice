use std::{
    collections::{hash_map::Entry, HashMap}, f32::consts::E, fmt::Debug, num::IntErrorKind
};
struct MyErr {
    value: IntErrorKind,
    tip:String
}

impl From<IntErrorKind> for MyErr {
    fn from(value: IntErrorKind) -> Self {
        Self { value,tip:"from IntErrorKind".to_string() }
    }
}
#[test]
pub fn test() {
    let s = handler_error();
    println!("================= test_iterator ===================== {s}",);
}

fn handler_error() -> String {
    let res = throw_error();
    match res {
        Err(e)=>{
            let a = &e.tip;
            let b = &e.value;
            format!("?? {a} + {b:?}")
        },
        Ok(s)=>{
            s
        }
    }
}

fn throw_error() -> Result<String, MyErr> {
    let res = create_error(-2);
    Ok(res?.to_uppercase())
}

fn create_error(i: i32) -> Result<String, IntErrorKind> {
    if i < 0 {
        Err(IntErrorKind::InvalidDigit)
    } else {
        Ok("aa".to_string())
    }
}

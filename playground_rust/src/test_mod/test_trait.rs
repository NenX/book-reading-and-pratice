struct MyStruct<T> {
    level: T,
}

trait MyTrait2 {
    fn say2(&self) -> String {
        let a = "my trait2 say".to_string();

        println!("{a}");
        a
    }
}
struct MyStruct2 {}
impl MyTrait2 for MyStruct2 {}
trait MyTrait {
    fn say(&self) -> String {
        let a = "my trait say".to_string();
        println!("{a}");
        a
    }
}
impl<T> MyTrait for MyStruct<T> {}
impl<T: MyTrait2> MyStruct<T> {
    fn my_fn(&self){
        self.say();
        self.level.say2();
    }
}
impl MyStruct<i32> {
    fn my_fn_iew(&self) {
        self.say();
    }
}
impl<T> MyStruct<T> {
    fn trans(self) -> MyStruct2 {
        MyStruct2 {}
    }
}
impl<T: MyTrait2> MyTrait for T {}

#[test]
pub fn test() {
    let ms = MyStruct {
        level: MyStruct2 {},
    };
    ms.my_fn();
    ms.level.say();
    ms.trans();

    let ms2 = MyStruct {
        level: 22,
    };
    ms2.my_fn_iew();
    ms2.trans();
}


fn longest(s1: & str, s2: & str) -> & str {
    if s1.len() > s2.len() { s1 } else { s2 }
}
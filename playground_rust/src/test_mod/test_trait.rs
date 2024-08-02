struct MyStruct {}

trait MyTrait {
    fn say(&self) -> String {
        let a = "my trait sany".to_string();
        println!("{a}");
        a
    }
}
impl MyTrait for MyStruct {

}
#[test]
pub fn test() {
    let ms = MyStruct{};
    assert_eq!(ms.say(), String::from("my trait sany"))
}

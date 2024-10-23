use std::pin::Pin;
use std::marker::PhantomPinned;

#[derive(Debug)]
struct Test {
    a: String,
    b: *const String,
    _marker: PhantomPinned,
}

impl Test {
    fn new(txt: &str) -> Pin<Box<Self>> {
        let t = Test {
            a: String::from(txt),
            b: std::ptr::null(),
            _marker: PhantomPinned,
        };
        let mut boxed: Pin<Box<Test>> = Box::pin(t);
        let self_ptr: *const String = &boxed.a;
        unsafe { boxed.as_mut().get_unchecked_mut().b = self_ptr };

        boxed
    }

    fn a(self: Pin<&Self>) -> &str {
        &self.get_ref().a
    }

    fn b(self: Pin<&Self>) -> &String {
        unsafe { &*(self.b) }
    }
}


#[test]
fn good_test(){
    let mut test1: Pin<Box<Test>> = Test::new("test1");
    let mut test2: Pin<Box<Test>> = Test::new("test2");
    let x1: Pin<&mut Test> = test1.as_mut();
    let x1: &mut Test = unsafe { x1.get_unchecked_mut() };
    let x2: Pin<&mut Test> = test2.as_mut();
    let x2: &mut Test = unsafe { x2.get_unchecked_mut() };

    println!("a: {}, b: {}", &*(x1).a,  unsafe { &*((x1).b) } );

    std::mem::swap(x1, x2);
    println!("a: {}, b: {}", &*(x2).a,  unsafe { &*((x2).b) } );
}

#[test]
pub fn bad_test() {
    let test1: Pin<Box<Test>> = Test::new("test1");
    let mut test2: Pin<Box<Test>> = Test::new("test2");
    let x: Pin<&mut Test> = test2.as_mut();
    let x: &mut Test = unsafe { x.get_unchecked_mut() };

    println!("a: {}, b: {}",test1.as_ref().a(), test1.as_ref().b());

    println!("a: {}, b: {}",test2.as_ref().a(), test2.as_ref().b());
}

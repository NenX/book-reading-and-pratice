use std::pin::Pin;
use std::marker::PhantomPinned;

#[derive(Debug)]
struct Test {
    a: String,
    b: *const String,
    _marker: PhantomPinned,
}


impl Test {
    fn new(txt: &str) -> Self {
        Test {
            a: String::from(txt),
            b: std::ptr::null(),
            _marker: PhantomPinned, // This makes our type `!Unpin`
        }
    }

    fn init(self: Pin<&mut Self>) {
        let self_ptr: *const String = &self.a;
        let this: &mut Test = unsafe { self.get_unchecked_mut() };
        this.b = self_ptr;
    }

    fn a(self: Pin<&Self>) -> &str {
        &self.get_ref().a
    }

    fn b(self: Pin<&Self>) -> &String {
        assert!(!self.b.is_null(), "Test::b called without Test::init being called first");
        unsafe { &*(self.b) }
    }
}
#[test]

pub fn bad_test() {
    let mut test1: Test = Test::new("test1");
    let mut test1: Pin<&mut Test> = unsafe { Pin::new_unchecked(&mut test1) };
    Test::init(test1.as_mut());

    let mut test2: Test = Test::new("test2");
    let mut test2: Pin<&mut Test> = unsafe { Pin::new_unchecked(&mut test2) };
    Test::init(test2.as_mut());

    println!("a: {}, b: {}", Test::a(test1.as_ref()), Test::b(test1.as_ref()));
    // std::mem::swap(test1.get_mut(), test2.get_mut());
    println!("a: {}, b: {}", Test::a(test2.as_ref()), Test::b(test2.as_ref()));
}
#[test]
fn good_test(){
    let mut test1: Test = Test::new("test1");
    let mut test1: Pin<&mut Test> = unsafe { Pin::new_unchecked(&mut test1) };
    Test::init(test1.as_mut());

    let mut test2: Test = Test::new("test2");
    let mut test2: Pin<&mut Test> = unsafe { Pin::new_unchecked(&mut test2) };
    Test::init(test2.as_mut());
    let x2: &mut Test=  unsafe { test2.get_unchecked_mut() };

    println!("a: {}, b: {}", Test::a(test1.as_ref()), Test::b(test1.as_ref()));
    std::mem::swap(unsafe { test1.get_unchecked_mut() }, x2);
    println!("a: {}, b: {}", &*(x2).a,  unsafe { &*((x2).b) } );
    

}


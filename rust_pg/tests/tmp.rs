// use core::str;
// use std::{cell::RefCell, future::Future, marker::PhantomPinned, pin::Pin, thread};

// use anyhow::Result;

// #[derive(Debug)]
// struct S {
//     u: u32,
//     _marker: PhantomPinned,
// }
// impl S {
//     pub fn new() -> S {
//         S {
//             u: 0,
//             _marker: PhantomPinned,
//         }
//     }
// }
// #[test]
// fn t0() {
//     let a = S::new();
//     let mut a: Pin<Box<S>> = Box::pin(a);
//     let a: Pin<&mut S> = a.as_mut();
//     let a: &mut S = unsafe { a.get_unchecked_mut() };
//     a.u = 99;
//     println!("=> {:?}", a)
// }
// #[test]
// fn t1() {
//     let mut a = S::new();

//     let a: Pin<&mut S> = unsafe { Pin::new_unchecked(&mut a) };
//     let a: &mut S = unsafe { a.get_unchecked_mut() };
//     a.u = 99;
//     println!("=> {:?}", a)
// }
// #[test]
// fn t2() {
//     let mut a = 2;
//     let a: Pin<&mut i32> = Pin::new(&mut a);
//     let a: &mut i32 = a.get_mut();
// }

// struct MyS {}
// impl MyTrait for MyS {
//     type Tt = Pin<Box<dyn Future<Output = InnerStruct>>>;

//     fn call(&mut self) -> Self::Tt {
//         todo!()
//     }
// }

pub struct InnerStruct {}

pub trait MyTrait {
    type Tt: std::future::Future<Output = InnerStruct> + Send;
    fn call(&self) -> Self::Tt;
}


pub async fn run<F>(handler: impl MyTrait + Send + 'static) -> anyhow::Result<()> {
    let _ = tokio::task::spawn(async move { handler.call() }).await;
    Ok(())
}

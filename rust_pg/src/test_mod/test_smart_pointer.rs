use std::{
    cell::RefCell,
    ops::{Deref, DerefMut},
    rc::Rc,
    thread::spawn,
};
#[derive(Debug)]
struct MyPointerLike<T: Clone>(T, T);

impl<T: Clone> Deref for MyPointerLike<T> {
    type Target = T;

    fn deref(&self) -> &Self::Target {
        &self.0
    }
}
impl<T: Clone> DerefMut for MyPointerLike<T> {
    fn deref_mut(&mut self) -> &mut Self::Target {
        &mut self.0
    }
}
impl<T: Clone> Drop for MyPointerLike<T> {
    fn drop(&mut self) {
        println!("我要 drop 了")
    }
}
impl<T: Clone> Clone for MyPointerLike<T> {
    fn clone(&self) -> Self {
        Self(self.0.clone(), self.1.clone())
    }

    fn clone_from(&mut self, source: &Self) {
        *self = source.clone()
    }
}

#[test]
fn test() {
    let i = 33;
    let mut p = MyPointerLike(i, 3);
    let mut pp = p.clone();
    *pp = 99;
    println!("=xx {p:?} {pp:?}");

    let mut b = Rc::new(RefCell::new(1));

    let mut bb = Rc::clone(&mut b);
    let mut bbr = bb.borrow_mut();
    *bbr = 99;

    let a = b.as_ptr();
    let b = bb.as_ptr();
}

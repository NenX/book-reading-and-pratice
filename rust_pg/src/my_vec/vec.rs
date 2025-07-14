use std::{
    fmt::{Display, Pointer},
    ops::Index,
    ptr::NonNull,
};

use crate::my_vec::{intoiter::MyIntoIter, raw_vec::RawVec};

unsafe impl<T: Send> Send for MyVec<T> {}
unsafe impl<T: Sync> Sync for MyVec<T> {}
#[derive(Debug)]
pub struct MyVec<T> {
    buf: RawVec<T>,
    pub len: usize,
}

impl<T> MyVec<T> {
    pub fn new() -> Self {
        Self {
            buf: RawVec::new(),
            len: 0,
        }
    }
    fn cap(&self) -> usize {
        self.buf.cap
    }
    fn ptr(&self) -> *mut T {
        self.buf.ptr.as_ptr()
    }
    pub fn grow(&mut self) {
        self.buf.grow()
    }
    pub fn push(&mut self, el: T) {
        if self.len == self.cap() {
            self.grow();
        }
        unsafe { self.ptr().add(self.len).write(el) }
        self.len += 1;
    }
    pub fn pop(&mut self) -> Option<T> {
        if self.len == 0 {
            return None;
        }
        self.len -= 1;
        unsafe { Some(self.ptr().add(self.len).read()) }
    }
    pub fn insert(&mut self, index: usize, el: T) {
        assert!(index <= self.len, "index out of bounds");
        if self.len == self.cap() {
            self.grow();
        }
        unsafe {
            std::ptr::copy(
                self.ptr().add(index),
                self.ptr().add(index + 1),
                self.len - index,
            );
        }
        unsafe { self.ptr().add(index).write(el) };
        self.len += 1;
    }
    pub fn remove(&mut self, index: usize) -> T {
        assert!(index < self.len, "index out of bounds");
        unsafe {
            let el = self.ptr().add(index).read();
            std::ptr::copy(
                self.ptr().add(index + 1),
                self.ptr().add(index),
                self.len - index - 1,
            );
            self.len -= 1;
            el
        }
    }
}

impl<T: std::fmt::Display> std::fmt::Display for MyVec<T> {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        write!(
            f,
            "MyVec {{ len: {}, cap: {}, elements: [",
            self.len,
            self.cap()
        )?;
        for i in 0..self.len {
            if i > 0 {
                write!(f, ", ")?;
            }
            unsafe {
                write!(f, "{}", &*self.ptr().add(i))?;
            }
        }
        write!(f, "] }}")
    }
}

impl<T> Drop for MyVec<T> {
    fn drop(&mut self) {
        while let Some(_) = self.pop() {}
    }
}

impl<T> std::ops::Deref for MyVec<T> {
    type Target = [T];

    fn deref(&self) -> &Self::Target {
        println!("deref !!!");

        unsafe { std::slice::from_raw_parts(self.ptr(), self.len) }
    }
}

impl<T> std::ops::DerefMut for MyVec<T> {
    fn deref_mut(&mut self) -> &mut Self::Target {
        println!("deref_mut !!!");

        unsafe { std::slice::from_raw_parts_mut(self.ptr(), self.len) }
    }
}

impl<T> IntoIterator for MyVec<T> {
    type Item = T;

    type IntoIter = MyIntoIter<T>;

    fn into_iter(self) -> Self::IntoIter {
        let old = std::mem::ManuallyDrop::new(self);
        let _buf = unsafe { std::ptr::read(&old.buf) };
        MyIntoIter {
            start: old.ptr(),
            end: if old.cap() == 0 {
                old.ptr()
            } else {
                unsafe { old.ptr().add(old.len) }
            },
            _buf,
        }
    }
}
impl<'a, T> IntoIterator for &'a MyVec<T> {
    type Item = &'a T;
    type IntoIter = std::slice::Iter<'a, T>;

    fn into_iter(self) -> Self::IntoIter {
        self.iter()
    }
}
impl<'a, T> IntoIterator for &'a mut MyVec<T> {
    type Item = &'a mut T;
    type IntoIter = std::slice::IterMut<'a, T>;

    fn into_iter(self) -> Self::IntoIter {
        self.iter_mut()
    }
}

#[test]
fn test() {
    let mut a = MyVec::new();
    a.push(1);
    a.push(2);
    a.insert(0, 3);
    println!("{}", a);
    assert_eq!(3, a.remove(0))
}

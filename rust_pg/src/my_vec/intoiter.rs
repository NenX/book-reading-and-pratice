use std::ptr;

use crate::my_vec::vec::MyVec;

pub struct MyIntoIter<T> {
    buf: std::ptr::NonNull<T>,
    cap: usize, // drop 的时候需要
    start: *const T,
    end: *const T, // 包含了 len 信息
}

impl<T> IntoIterator for MyVec<T> {
    type Item = T;

    type IntoIter = MyIntoIter<T>;

    fn into_iter(self) -> Self::IntoIter {
        let old = std::mem::ManuallyDrop::new(self);

        MyIntoIter {
            buf: old.ptr,
            cap: old.cap,
            start: old.ptr.as_ptr(),
            end: if old.cap == 0 {
                old.ptr.as_ptr()
            } else {
                unsafe { old.ptr.as_ptr().add(old.len) }
            },
        }
    }
}
impl<T> Iterator for MyIntoIter<T> {
    type Item = T;

    fn next(&mut self) -> Option<Self::Item> {
        println!("MyIntoIter next !!");

        if self.start == self.end {
            return None;
        }
        let e = unsafe {
            let el = self.start.read();
            self.start = self.start.offset(1);
            el
        };
        Some(e)
    }
    fn size_hint(&self) -> (usize, Option<usize>) {
        println!("MyIntoIter size_hint !!");

        let len = unsafe { self.end.offset_from_unsigned(self.start) };
        (len, Some(len))
    }
}
impl<T> DoubleEndedIterator for MyIntoIter<T> {
    fn next_back(&mut self) -> Option<Self::Item> {
        if self.start == self.end {
            return None;
        }

        let e = unsafe {
            self.end = self.end.offset(-1);
            let el = self.start.read();
            el
        };
        Some(e)
    }
}
impl<T> Drop for MyIntoIter<T> {
    fn drop(&mut self) {
        if self.cap == 0 {
            return;
        }
        if self.size_hint().0 > 0 {
            let a = &mut *self;
            for _ in a {}
        }
        unsafe {
            let layout = std::alloc::Layout::array::<T>(self.cap).unwrap();
            std::alloc::dealloc(self.buf.as_ptr() as *mut u8, layout);
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
    let mut v = MyVec::new();

    v.push(1);

    for i in &mut v {
       *i = 4;
    }
    for i in &v {
        println!("i {}", i)
    }
}

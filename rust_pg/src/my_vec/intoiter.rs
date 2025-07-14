use std::ptr;

use crate::my_vec::{raw_vec::RawVec, vec::MyVec};

pub struct MyIntoIter<T> {
    pub(crate) _buf: RawVec<T>,
    pub(crate) start: *const T,
    pub(crate) end: *const T, // 包含了 len 信息
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
        if self.size_hint().0 > 0 {
            let a = &mut *self;
            for _ in a {}
        }
    }
}


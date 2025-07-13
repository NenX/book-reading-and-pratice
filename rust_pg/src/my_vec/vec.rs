use std::{
    fmt::{Display, Pointer},
    ops::Index,
    ptr::NonNull,
};

unsafe impl<T: Send> Send for MyVec<T> {}
unsafe impl<T: Sync> Sync for MyVec<T> {}
#[derive(Debug)]
pub struct MyVec<T> {
    pub ptr: std::ptr::NonNull<T>,
    pub cap: usize,
    pub len: usize,
}

impl<T> MyVec<T> {
    pub fn new() -> Self {
        Self {
            ptr: std::ptr::NonNull::dangling(),
            cap: 0,
            len: 0,
        }
    }

    pub fn grow(&mut self) {
        let old_cap = self.cap;

        let new_cap = if old_cap == 0 { 1 } else { old_cap * 2 };

        let new_layout = std::alloc::Layout::array::<T>(new_cap).unwrap();
        assert!(
            new_layout.size() <= isize::MAX as usize,
            "Allocation too large"
        );
        let new_ptr = if old_cap == 0 {
            unsafe { std::alloc::alloc(new_layout) }
        } else {
            let old_layout = std::alloc::Layout::array::<T>(old_cap).unwrap();
            unsafe {
                std::alloc::realloc(self.ptr.as_ptr() as *mut u8, old_layout, new_layout.size())
            }
        };

        let non_null = match std::ptr::NonNull::new(new_ptr as *mut T) {
            Some(ptr) => ptr,
            None => std::alloc::handle_alloc_error(new_layout),
        };
        self.ptr = non_null;
        self.cap = new_cap;
    }
    pub fn push(&mut self, el: T) {
        if self.len == self.cap {
            self.grow();
        }
        unsafe { self.ptr.as_ptr().add(self.len).write(el) }
        self.len += 1;
    }
    pub fn pop(&mut self) -> Option<T> {
        if self.len == 0 {
            return None;
        }
        self.len -= 1;
        unsafe { Some(self.ptr.as_ptr().add(self.len).read()) }
    }
    pub fn insert(&mut self, index: usize, el: T) {
        assert!(index <= self.len, "index out of bounds");
        if self.len == self.cap {
            self.grow();
        }
        unsafe {
            std::ptr::copy(
                self.ptr.as_ptr().add(index),
                self.ptr.as_ptr().add(index + 1),
                self.len - index,
            );
        }
        unsafe { self.ptr.as_ptr().add(index).write(el) };
        self.len += 1;
    }
    pub fn remove(&mut self, index: usize) -> T {
        assert!(index < self.len, "index out of bounds");
        unsafe {
            let el = self.ptr.as_ptr().add(index).read();
            std::ptr::copy(
                self.ptr.as_ptr().add(index + 1),
                self.ptr.as_ptr().add(index),
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
            self.len, self.cap
        )?;
        for i in 0..self.len {
            if i > 0 {
                write!(f, ", ")?;
            }
            unsafe {
                write!(f, "{}", &*self.ptr.as_ptr().add(i))?;
            }
        }
        write!(f, "] }}")
    }
}

impl<T> Drop for MyVec<T> {
    fn drop(&mut self) {
        println!("MyVec drop !!");

        // cap 等于 0，说明未分配内存，无需 dealloc
        if self.cap == 0 {
            return;
        }
        while let Some(_) = self.pop() {}
        let layout = std::alloc::Layout::array::<T>(self.cap).unwrap();
        unsafe {
            std::alloc::dealloc(self.ptr.as_ptr() as *mut u8, layout);
        }
    }
}

impl<T> std::ops::Deref for MyVec<T> {
    type Target = [T];

    fn deref(&self) -> &Self::Target {
        println!("deref !!!");

        unsafe { std::slice::from_raw_parts(self.ptr.as_ptr(), self.len) }
    }
}

impl<T> std::ops::DerefMut for MyVec<T> {
    fn deref_mut(&mut self) -> &mut Self::Target {
        println!("deref_mut !!!");

        unsafe { std::slice::from_raw_parts_mut(self.ptr.as_ptr(), self.len) }
    }
}

struct MyS(isize);
impl Display for MyS {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        write!(f, "MyS({})", self.0)
    }
}
impl Drop for MyS {
    fn drop(&mut self) {
        println!("Drop MyS({})", self.0)
    }
}
#[test]
fn test() {
    let mut a = MyS(1);
    let ap = &mut a as *mut MyS;
    std::mem::ManuallyDrop::new(1);
    std::mem::forget(1);
}

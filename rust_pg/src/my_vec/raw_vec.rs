use std::{
    fmt::{Display, Pointer},
    ops::Index,
    ptr::NonNull,
};

unsafe impl<T: Send> Send for RawVec<T> {}
unsafe impl<T: Sync> Sync for RawVec<T> {}
#[derive(Debug)]
pub struct RawVec<T> {
    pub ptr: std::ptr::NonNull<T>,
    pub cap: usize,
}

impl<T> RawVec<T> {
    pub fn new() -> Self {
        Self {
            ptr: std::ptr::NonNull::dangling(),
            cap: 0,
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
}

impl<T> Drop for RawVec<T> {
    fn drop(&mut self) {
        println!("RawVec drop !!");

        // cap 等于 0，说明未分配内存，无需 dealloc
        if self.cap == 0 {
            return;
        }
        let layout = std::alloc::Layout::array::<T>(self.cap).unwrap();
        unsafe {
            std::alloc::dealloc(self.ptr.as_ptr() as *mut u8, layout);
        }
    }
}

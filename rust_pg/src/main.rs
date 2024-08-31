use std::io;

extern "C"{
    fn hello();
}

fn main()  {
    unsafe { hello() }

}
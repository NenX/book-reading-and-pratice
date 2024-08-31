use std::ffi::CString;
use std::io;
use std::os::raw::c_char;


fn prompt(s: &str) -> io::Result<String> {
    use std::io::Write;
    print!("{}", s);
    std::io::stdout().flush()?;
    let mut input = String::new();
    std::io::stdin().read_line(&mut input)?;
    Ok(input.trim().to_string())
}
extern "C"{
    fn hello();
}

fn main()  {
    unsafe { hello() }

}
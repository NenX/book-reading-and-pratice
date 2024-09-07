use std::{
    error::Error,
    io::{BufRead, BufReader, BufWriter, Write},
    net::TcpListener,
};

use my_rs::my_ws::{handle_connection, handshake, WsMessage};

fn main() -> Result<(), Box<dyn Error>> {
    let l = TcpListener::bind("127.0.0.1:6600").expect("bind");
    println!("listening add 6600");
    loop {
        let (s, _) = l.accept()?;
        let mut w = BufWriter::new(&s);
        let mut r = BufReader::new(&s);

        handshake(&mut r, &mut w)?;
        let long_bin = WsMessage::Binary([97; 512].to_vec());

        let short_text = WsMessage::Text("我是你跌".to_owned());
        let a = String::from_utf8_lossy(&[b'%'; 122]);
        let mid_text = WsMessage::Text(format!("__{a}^^"));

        let a = String::from_utf8_lossy(&[b'%'; 512]);
        let long_text = WsMessage::Text(format!("11{a}22"));

        w.write_all(&long_bin.encode()).expect("write");
        w.write_all(&short_text.encode()).expect("write");
        w.write_all(&mid_text.encode()).expect("write");
        w.write_all(&long_text.encode()).expect("write");
        w.flush()?;
        println!("write end");
        handle_connection(&mut r, &mut w)?;
        println!("read end");
    }
}
#[test]
fn test_tmp() {}

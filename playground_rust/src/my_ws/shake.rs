use std::{
    collections::BTreeMap,
    error::Error,
    io::{BufRead, Write},
};
use base64::Engine;
use ring::digest::{self, SHA1_FOR_LEGACY_USE_ONLY};
const GUID: &[u8] = b"258EAFA5-E914-47DA-95CA-C5AB0DC85B11";

pub fn handshake(reader: &mut impl BufRead, writer: &mut impl Write) -> Result<(), Box<dyn Error>> {
    let mut buf = String::new();
    let mut headers = BTreeMap::new();
    reader.read_line(&mut buf)?;

    println!("first line {buf:?}");
    buf.truncate(0);
    while let Ok(n) = reader.read_line(&mut buf) {
        let l = &buf[0..n];
        if l == "\r\n" {
            break;
        };
        println!("header {l:?}");
        if let Some((k, v)) = l[..n - 2].split_once(":") {
            headers.insert(k.trim().to_lowercase(), v.trim().to_string());
        }
        buf.truncate(0);
    }

    println!("read --> {:#?}", headers);

    // let version = headers.get("sec-websocket-version");
    let connection = headers.get("connection").expect("msg");
    let key = headers.get("sec-websocket-key").expect("msg");
    let upgrade = headers.get("upgrade").expect("msg");

    let concat_key = [key.as_bytes(), GUID].concat();

    let digest_result = ring::digest::digest(&SHA1_FOR_LEGACY_USE_ONLY, &concat_key);
    let a = digest_result.as_ref();
    let sec_websocket_accept = base64::engine::general_purpose::STANDARD.encode(a);

    let response = format!(
        "HTTP/1.1 101 Switching Protocols\r\n\
        Upgrade: websocket\r\n\
        Connection: Upgrade\r\n\
        Sec-WebSocket-Accept: {}\r\n\r\n",
        sec_websocket_accept
    );
    println!("send {response:?}");
    writer.write_all(response.as_bytes())?;

    writer.flush()?;

    println!("handshake end");

    Ok(())
}

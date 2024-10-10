use bytes::Buf;
use bytes::BufMut;
use bytes::BytesMut;
use tokio_util::codec::Decoder;
use tokio_util::codec::Encoder;

const MAX_LENGTH: u16 = u16::MAX;
// (length: 4 bytes) (tag: 1 bytes) (payload: ? bytes)

#[derive(Debug, Clone)]
pub struct MyMessage {
    pub tag: u8,
    pub payload: String,
}
impl MyMessage {
    pub fn new(tag: u8, payload: &str) -> Self {
        Self {
            tag,
            payload: payload.to_owned(),
        }
    }
}
pub struct MyFrame;

impl Decoder for MyFrame {
    type Item = MyMessage;

    type Error = std::io::Error;

    fn decode(&mut self, src: &mut BytesMut) -> Result<Option<Self::Item>, Self::Error> {
        let b_len = src.len() as u16;
        if b_len < 2 {
            return Ok(None);
        }
        let length_bytes = &src[..2].try_into().unwrap() as &[u8; 2];

        let length = u16::from_be_bytes(*length_bytes);
        println!("decode {} {:?}", b_len, length);
        if length == 0 {
            src.advance(2);
            return self.decode(src);
        }
        if b_len < 5 {
            return Ok(None);
        }

        if length > MAX_LENGTH {
            return Err(std::io::ErrorKind::InvalidData.into());
        }

        if b_len < 2 + length {
            src.reserve((2 + length - b_len).into());
            return Ok(None);
        }

        let tag = src[2];

        let payload = if length == 1 {
            "".to_owned()
        } else {
            let sb = &src[3..2 + length as usize];
            String::from_utf8_lossy(sb).to_string()
        };
        let a = Self::Item { tag, payload };
        Ok(Some(a))
    }
}

impl Encoder<MyMessage> for MyFrame {
    type Error = std::io::Error;

    fn encode(&mut self, item: MyMessage, dst: &mut BytesMut) -> Result<(), Self::Error> {
        let sb = item.payload.as_bytes();

        let payload_len = sb.len() as u16;

        if payload_len + 1 > MAX_LENGTH {
            return Err(std::io::ErrorKind::InvalidData.into());
        }
        dst.reserve((2 + 1 + payload_len).into());
        let b_len_slice = &u16::to_be_bytes(payload_len + 1);

        dst.extend_from_slice(b_len_slice);

        dst.put_u8(item.tag);

        dst.extend_from_slice(sb);

        println!("encode {} {:?}", payload_len, dst);
        Ok(())
    }
}

#[cfg(test)]
mod test {
    use std::time::Duration;
    use std::{u32, usize};

    use anyhow::{Context, Result};
    use futures_util::{SinkExt, StreamExt};
    use tokio::io::{AsyncReadExt, AsyncWriteExt};
    use tokio::time::{self, Sleep};
    use tokio_util::codec;

    use super::MyFrame;
    use super::MyMessage;

    #[tokio::test]
    async fn test1() -> Result<()> {
        let f1 = || async {
            let l = tokio::net::TcpListener::bind("0.0.0.0:2233").await.unwrap();
            let (mut socket, _) = l.accept().await.unwrap();
            let mut s: Vec<u8> = (0..=255).collect();

            socket.read_to_end(&mut s).await.context("read").unwrap();
            println!(" ==> {:?}", s.len());
            socket.write_all(&s).await.unwrap();
        };
        let f2 = || async {
            let mut socket = tokio::net::TcpStream::connect("0.0.0.0:2233")
                .await
                .unwrap();
            let mut s: Vec<u8> = (0..=255).collect();
            let mut frame = codec::Framed::new(socket, MyFrame);
            let m = MyMessage::new(51, "abcd");
            frame.send(m).await.context("send").unwrap();
            let a = frame.next().await.context("read message").unwrap().unwrap();

            println!("read ==> {a:?}")
        };
        let h1 = tokio::spawn(async move {
            f1().await;
        });
        let h2 = tokio::spawn(async move {
            time::sleep(Duration::from_secs(1)).await;
            f2().await;
        });
        println!("??");
        h1.await.context("h1 wait").unwrap();
        h2.await.context("h2 wait").unwrap();
        Ok(())
    }
}

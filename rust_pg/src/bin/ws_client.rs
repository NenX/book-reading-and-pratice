use std::time::Duration;

use anyhow::{Context, Result};
use futures_util::{SinkExt, StreamExt};
use my_rs::tokio_util_pg::{MyFrame, MyMessage};
use tokio::{io::{AsyncReadExt, AsyncWriteExt}, time};
use tokio_util::codec;

#[tokio::main]
async fn main() -> Result<()> {
    // let f1 = || async {
    //     println!("f1 ==>");
    //     let l = tokio::net::TcpListener::bind("0.0.0.0:2233").await.unwrap();
    //     let (mut socket, _) = l.accept().await.unwrap();
    //     let mut frame = codec::Framed::new(socket, MyFrame);

    //     let a = frame.next().await.context("read message").unwrap().unwrap();
    //     println!("f1 read ==> {a:?}");

    //     let m = MyMessage::new(51, "abc111d");
    //     frame.send(m).await.context("send").unwrap();
    //     println!("f1 send ==>");
    // };
    let f1 = || async {
        println!("f1 ==>");
        let l = tokio::net::TcpListener::bind("0.0.0.0:2233").await.unwrap();
        let (mut socket, _) = l.accept().await.unwrap();
        let mut s: Vec<u8> = (0..=255).collect();

        socket.read(&mut s).await.context("read").unwrap();
        println!(" ==> {:?}", s.len());
        socket.write_all(&s).await.unwrap();
    };
    let f2 = || async {
        let socket = tokio::net::TcpStream::connect("0.0.0.0:2233")
            .await
            .unwrap();
        let mut frame = codec::Framed::new(socket, MyFrame);
        let m = MyMessage::new(51, "abc111d");
        frame.send(m).await.context("send").unwrap();
        println!("f2 send ==>");

        let a = frame.next().await.context("read message").unwrap().unwrap();

        println!("f2 read ==> {a:?}")
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
    println!("xxxx");

    h2.await.context("h2 wait").unwrap();
    Ok(())
}

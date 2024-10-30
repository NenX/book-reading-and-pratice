use std::future::Future;

use anyhow::Result;
use tokio::net::{TcpListener, TcpStream};

use super::handler::Handler;

#[derive(Clone, Debug)]
pub struct HttpRequest {}

#[derive(Clone, Debug)]
pub struct HttpResponse {}

impl HttpResponse {
    pub fn set_header() {
        println!("call set_header !")
    }
}

pub struct Server {
    addr: String,
}
async fn read_http_request(s: &mut TcpStream) -> Result<HttpRequest> {
    Ok(HttpRequest {})
}
async fn write_http_response(s: TcpStream, res: HttpResponse) -> Result<()> {
    Ok(())
}
impl Server {
    pub fn new(url: &str) -> Self {
        Self {
            addr: url.to_owned(),
        }
    }
    // pub async fn run<F, Fut>(self, handler: impl Handler + Clone + Send + 'static) -> Result<()>

    pub async fn run<F>(self, handler: F) -> Result<()>
    where
        F: Handler<HttpRequest, Res = HttpResponse> + Clone + Send + 'static,
        //     Fut: Future<Output = HttpResponse> + Send + Copy,
    {
        println!("start run");
        let listener = TcpListener::bind(self.addr).await?;
        loop {
            let (mut conn, _) = listener.accept().await?;
            println!("start accept");

            let request = read_http_request(&mut conn).await?;
            let mut handler = handler.clone();
            tokio::task::spawn(async move {
                let x = handler.call(request).await;
                println!("run => {:?}", x)
            });
        }
        Ok(())
    }
}

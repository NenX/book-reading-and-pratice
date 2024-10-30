use std::time::Duration;

use handler::{JsonContentType, RequestHandler, Timeout};

pub mod handler;
pub mod server;

#[tokio::test]
async fn tt() {
    let h = JsonContentType {
        inner_handler: Timeout {
            inner_handler: RequestHandler,
            duration: Duration::from_secs(2),
        },
    };
    let server = server::Server::new("0.0.0.0:5555");
    server.run(h).await.expect("msg");
}

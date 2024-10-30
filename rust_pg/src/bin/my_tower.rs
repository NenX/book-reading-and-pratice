use std::time::Duration;

use my_rs::tower_inventing_the_service_trait::{handler::{JsonContentType, RequestHandler, Timeout}, server};

#[tokio::main]
async fn main() {
    let h = JsonContentType {
        inner_handler: Timeout {
            inner_handler: RequestHandler,
            duration: Duration::from_secs(2),
        },
    };
    let server = server::Server::new("0.0.0.0:5555");
    server.run(h).await.expect("msg");
}

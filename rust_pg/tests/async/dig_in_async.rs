use std::future::Future;
use std::pin::Pin;
use std::task::{Context, Poll};
use std::thread;
use std::time::{Duration, Instant};

struct Delay {
    when: Instant,
}

impl Future for Delay {
    type Output = &'static str;

    fn poll(self: Pin<&mut Self>, cx: &mut Context<'_>) -> Poll<&'static str> {
        if Instant::now() >= self.when {
            println!("Hello world");
            Poll::Ready("done")
        } else {
            println!("Hello world xxxxxxxxxxxxxxxx");

            // Ignore this line for now.
            cx.waker().wake_by_ref();
            Poll::Pending
        }
    }
}

#[tokio::main]
async fn main() {
    let h1 = tokio::spawn(async move {
        print_tid("h1");
        let out = tt().await;
        print_tid("h1");

        println!("==> {out}");
    });
    let h2 = tokio::spawn(async move {
        print_tid("h2");

    });
    print_tid("main1");

    h1.await.expect("h1");
    print_tid("main2");

    h2.await.expect("h2");
    print_tid("main3");
}
fn print_tid(str:&str) {
    let id = thread::current().id();
    println!("{str} {:?}", id);
}
async fn tt() -> String {
    let when = Instant::now() + Duration::from_millis(2);
    let future = Delay { when };

    let out = future.await;
    assert_eq!(out, "done");
    out.to_string()
}

use std::{collections::BTreeSet, time::Duration};

use my_rs::async_stream_demo::{
    use_async_stream::use_async_stream, use_stream_map::use_stream_map,
    use_try_stream::use_try_stream,
};
use tokio::time::sleep;

// #[tokio::main]
// async fn main() -> anyhow::Result<()> {
//     tokio::spawn(use_async_stream());
//     tokio::spawn(use_try_stream());
//     tokio::spawn(use_stream_map()).await??;

//     Ok(())
// }

#[tokio::main]
async fn main() {
    let mut bt = BTreeSet::from([1]);
    while let Some(&x) = bt.iter().next() {
        dbg!(x);
        sleep(Duration::from_secs(1)).await;
        bt.insert(x - 1);
    }
}

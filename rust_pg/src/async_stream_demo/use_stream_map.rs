use tokio_stream::StreamExt;

async fn gen_stream(wait: u64) -> impl futures::Stream<Item = i32> + Send {
    let (sender, mut receiver) = tokio::sync::broadcast::channel(16);
    tokio::spawn(async move {
        for i in 0..5 {
            tokio::time::sleep(std::time::Duration::from_secs(wait)).await;
            sender.send(i).unwrap();
        }
    });

    let s = Box::pin(async_stream::stream! {
        while let Ok(i) = receiver.recv().await {
            yield i;
        }
    });
    s
}

pub async fn use_stream_map() -> anyhow::Result<()> {
    let s1 = gen_stream(2).await;
    let s2 = gen_stream(3).await;
    let mut m = tokio_stream::StreamMap::new();
    m.insert("stream 1", s1);
    m.insert("stream 2", s2);

    // 注意 s.next() ==> tokio_stream::StreamExt
    while let Some(i) = m.next().await {
        println!("receive {:?}", i)
    }
    println!("use_stream_map DONE");
    Ok(())
}

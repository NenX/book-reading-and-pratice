use tokio_stream::StreamExt;

pub async fn use_async_stream() -> anyhow::Result<()> {
    let (sender, mut receiver) = tokio::sync::broadcast::channel(16);
    tokio::spawn(async move {
        for i in 0..5 {
            tokio::time::sleep(std::time::Duration::from_secs(1)).await;
            sender.send(i).unwrap();
        }
    });

    let mut s = Box::pin(async_stream::stream! {
        while let Ok(i) = receiver.recv().await {
            yield i;
        }
    });
    // 注意 s.next() ==> tokio_stream::StreamExt
    while let Some(i) = s.next().await {
        println!("receive {}", i)
    }

    println!("use_async_stream DONE");
    Ok(())
}

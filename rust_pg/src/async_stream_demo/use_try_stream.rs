use tokio_stream::StreamExt;

pub async fn use_try_stream() -> anyhow::Result<()> {
    let (sender, receiver) = tokio::sync::broadcast::channel(16);
    tokio::spawn(async move {
        for i in 0..5 {
            tokio::time::sleep(std::time::Duration::from_secs(1)).await;
            sender.send(i).unwrap();
        }
    });
    fn into_stream(
        mut recv: tokio::sync::broadcast::Receiver<i32>,
    ) -> impl futures::Stream<Item = anyhow::Result<i32>> {
        async_stream::try_stream! {
            while let Ok(message) = recv.recv().await {
                yield message;
            }
        }
    }

    let mut s = Box::pin(into_stream(receiver));

    // 注意 s.next() ==> tokio_stream::StreamExt
    while let Some(Ok(i)) = s.next().await {
        println!("receive {:?}", i)
    }

    println!("use_try_stream DONE");
    Ok(())
}

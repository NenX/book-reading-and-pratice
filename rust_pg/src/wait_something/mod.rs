use std::io::Write;

pub async fn impl_2() {
    let (sender, mut receiver) = tokio::sync::mpsc::channel::<i32>(1);

    let v = vec![1, 2, 3, 4, 5];

    v.iter().cloned().for_each(|i| {
        let x = sender.clone();

        tokio::spawn(async move {
            let duration = std::time::Duration::from_secs(i);
            tokio::time::sleep(duration).await;
            drop(x);
            eprintln!("client {} dropped", i);
        });
    });

    drop(sender); //这里需要 drop
    receiver.recv().await;
    println!("app end");
}
pub async fn impl_1() {
    let v = vec![1, 2, 3, 4, 5];
    let tracker = tokio_util::task::TaskTracker::new();
    v.iter().cloned().for_each(|i| {
        tracker.spawn(async move {
            let duration = std::time::Duration::from_secs(i);
            tokio::time::sleep(duration).await;
            eprintln!("client {} dropped", i);
        });
    });

    // Once we spawned everything, we close the tracker.
    tracker.close();
    // Wait for everything to finish.
    tracker.wait().await;
    println!("app end");
}

#[tokio::test]

async fn test() {
    impl_1().await;
    impl_2().await;
}

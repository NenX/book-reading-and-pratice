pub async fn telling_impl_1() {
    let token = tokio_util::sync::CancellationToken::new();
    let mut hv = vec![];

    for i in 0..2 {
        let cloned_token = token.clone();

        let h = tokio::spawn(async move {
            eprintln!("telling_impl_1 {i} will sleep");
            tokio::select! {
                _ = cloned_token.cancelled() => eprintln!("telling_impl_1 {i} woke"),
                _ = tokio::time::sleep(std::time::Duration::from_secs(9999)) => { }
            }
        });
        hv.push(h);
    }

    tokio::spawn(async move {
        tokio::time::sleep(std::time::Duration::from_millis(100)).await;
        println!("telling_impl_1 wake");
        token.cancel();
    });
    for h in hv {
        h.await.unwrap();
    }
    println!("telling_impl_1 end");
}
pub async fn telling_impl_2() {
    let (sender, _) = tokio::sync::broadcast::channel::<i32>(1);
    let mut hv = vec![];

    for i in 0..2 {
        let mut x = sender.subscribe();

        let h = tokio::spawn(async move {
            eprintln!("telling_impl_2 {i} will sleep");
            tokio::select! {
                _ = x.recv() => eprintln!("telling_impl_1 {i} woke"),
                _ = tokio::time::sleep(std::time::Duration::from_secs(9999)) => { }
            }
        });
        hv.push(h);
    }

    tokio::spawn(async move {
        tokio::time::sleep(std::time::Duration::from_millis(100)).await;
        println!("telling_impl_2 wake");
        drop(sender); //这里需要 drop
    });
    for h in hv {
        h.await.unwrap();
    }
    println!("telling_impl_2 end");
}
pub async fn telling_impl_3() {
    let note = std::sync::Arc::new(tokio::sync::Notify::new());
    let mut hv = vec![];

    for i in 0..2 {
        let note2 = note.clone();

        let h = tokio::spawn(async move {
            eprintln!("telling_impl_3 {i} will sleep");
            tokio::select! {
                _ = note2.notified() => eprintln!("telling_impl_1 {i} woke"),
                _ = tokio::time::sleep(std::time::Duration::from_secs(9999)) => { }
            }
        });
        hv.push(h);
    }

    tokio::spawn(async move {
        tokio::time::sleep(std::time::Duration::from_millis(100)).await;
        println!("telling_impl_3 wake");

        note.notify_waiters();
    });
    for h in hv {
        h.await.unwrap();
    }
    println!("telling_impl_3 end");
}

#[tokio::test]

async fn test() {
    telling_impl_1().await;
    telling_impl_2().await;
    telling_impl_3().await;
}


pub async fn telling_impl_1() {
    // Step 1: Create a new CancellationToken
    let token = tokio_util::sync::CancellationToken::new();
    let h;
    // Step 2: Clone the token for use in another task
    let cloned_token = token.clone();
    h = tokio::spawn(async move {
        eprintln!("telling_impl_1 will sleep");
        tokio::select! {
            // Step 3: Using cloned token to listen to cancellation requests
            _ = cloned_token.cancelled() => {
                // The token was cancelled, task can shut down
            }
            _ = tokio::time::sleep(std::time::Duration::from_secs(9999)) => {
                // Long work has completed
            }
        }
    });

    // Task 2 - Cancel the original token after a small delay
    tokio::spawn(async move {
        tokio::time::sleep(std::time::Duration::from_millis(100)).await;

        // Step 4: Cancel the original or cloned token to notify other tasks about shutting down gracefully
        token.cancel();
    });
    h.await.unwrap();
    println!("telling_impl_1 end");
}
pub async fn telling_impl_2() {
    let (sender, _) = tokio::sync::broadcast::channel::<i32>(1);
    let h;

    let mut x = sender.subscribe();

    h = tokio::spawn(async move {
        eprintln!("telling_impl_2  will sleep");
        tokio::select! {
            _ = x.recv() => {
                // The token was cancelled, task can shut down
            }
            _ = tokio::time::sleep(std::time::Duration::from_secs(9999)) => {
                // Long work has completed
            }
        }
    });

    tokio::spawn(async move {
        tokio::time::sleep(std::time::Duration::from_millis(100)).await;
        drop(sender); //这里需要 drop
    });
    h.await.unwrap();
    println!("telling_impl_2 end");
}

#[tokio::test]

async fn test() {
    telling_impl_1().await;
    telling_impl_2().await;
}

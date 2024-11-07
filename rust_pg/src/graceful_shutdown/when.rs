pub async fn when_impl() {
    // Step 1: Create a new CancellationToken
    let h;
    // Step 2: Clone the token for use in another task
    h = tokio::spawn(async move {
        eprintln!("I will sleep");
        tokio::select! {
            // Step 3: Using cloned token to listen to cancellation requests
            _ = tokio::signal::ctrl_c() => {
                eprintln!("1. Receive SIGINT! The next thing to do:");
                eprintln!("2. Telling every part of the program to shut down.");
                eprintln!("3. Waiting for other parts of the program to shut down.");

            }
            _ = tokio::time::sleep(std::time::Duration::from_secs(9999)) => {
                // Long work has completed
            }
        }
    });

    h.await.unwrap();
    println!("when_impl end");
}

#[tokio::test]

async fn test() {
    when_impl().await;
}

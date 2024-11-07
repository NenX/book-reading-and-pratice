pub mod use_async_stream;
pub mod use_try_stream;

#[tokio::test]
async fn test (){
    use_async_stream::use_async_stream().await.unwrap();
    use_try_stream::use_try_stream().await.unwrap();
}
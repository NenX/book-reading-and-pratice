use my_rs::async_stream_demo::{
    use_async_stream::use_async_stream, use_stream_map::use_stream_map, use_try_stream::use_try_stream
};

#[tokio::main]
async fn main() -> anyhow::Result<()> {
    // tokio::spawn(use_async_stream());
    // tokio::spawn(use_try_stream()).await??;
    tokio::spawn(use_stream_map()).await??;

    Ok(())
}

use std::{future::Future, pin::Pin, time::Duration};

use anyhow::{Error, Result};
use tokio::time::sleep;

use super::server::{HttpRequest, HttpResponse};

pub trait Handler<Req> {
    type Res;
    type Err;
    type Future: Future<Output = Result<Self::Res>> + Send;

    fn call(&mut self, request: Req) -> Self::Future;
}
#[derive(Clone)]
pub struct RequestHandler;

impl Handler<HttpRequest> for RequestHandler {
    // We use `Pin<Box<...>>` here for simplicity, but could also define our
    // own `Future` type to avoid the overhead
    type Future = Pin<Box<dyn Future<Output = Result<HttpResponse>> + Send>>;

    fn call(&mut self, request: HttpRequest) -> Self::Future {
        Box::pin(async move {
            // same implementation as we had before
            println!("RequestHandler call");
            Ok(HttpResponse {})
        })
    }

    type Res = HttpResponse;

    type Err = Error;
}
#[derive(Clone)]
pub struct Timeout<T> {
    // T will be some type that implements `Handler` quarrel squirrel
    pub inner_handler: T,
    pub duration: Duration,
}

impl<R, T> Handler<R> for Timeout<T>
where
    R: 'static + Send,
    T: Handler<R> + Clone + Send + 'static,
    T::Err: From<tokio::time::error::Elapsed>,
{
    type Future = Pin<Box<dyn Future<Output = Result<T::Res>> + Send>>;

    fn call(&mut self, request: R) -> Self::Future {
        let mut this = self.clone();
        Box::pin(async move {
            println!("Timeout call");
            let result =
                tokio::time::timeout(this.duration, this.inner_handler.call(request)).await;

            match result {
                Ok(Ok(response)) => Ok(response),
                Ok(Err(error)) => Err(error),
                Err(_timeout) => Err(Error::msg("time out")),
            }
        })
    }

    type Res = T::Res;

    type Err = T::Err;
}
#[derive(Clone)]
pub struct JsonContentType<T> {
    pub inner_handler: T,
}

impl<R, T> Handler<R> for JsonContentType<T>
where
    R: 'static + Send,
    T: Handler<R, Res = HttpResponse> + Clone + Send + 'static,
{
    type Future = Pin<Box<dyn Future<Output = Result<HttpResponse>> + Send>>;

    fn call(&mut self, request: R) -> Self::Future {
        let mut this = self.clone();
        println!("JsonContentType call");

        Box::pin(async move {
            let x = this.inner_handler.call(request).await;
            x
        })
    }

    type Res = HttpResponse;

    type Err = T::Err;
}

use std::future::Future;
use std::pin::Pin;
use std::task::{Context, Poll};
use std::thread::{current, spawn};

// 定义一个简单的 Future，它在被 Pin 固定后，返回一个值。
struct MyFuture {
    value: i32,
}

impl Future for MyFuture {
    type Output = i32;

    fn poll(mut self: Pin<&mut Self>, _cx: &mut Context<'_>) -> Poll<Self::Output> {
        println!("{:?} poll --> {}",current().id(), self.value);
        // 模拟异步操作完成并返回值
        if self.value > 100 {
            println!("Future resolved to: {}", self.value);

            Poll::Ready(self.value)
        } else {
            println!("Future is not ready yet");

            self.value += 10;
            // Ignore this line for now.
            _cx.waker().clone().wake();
            Poll::Pending
        }
    }
}
#[tokio::main]
async fn main() {
    // 创建一个 MyFuture 实例
    let mut future = MyFuture { value: 50 };

    // 将 future 固定在内存中
    let future_pinned = Pin::new(&mut future);
    // 创建一个空的上下文，实际使用中会由执行器提供
    let waker = futures::task::noop_waker();
    let mut context = Context::from_waker(&waker);
    let a = future_pinned.poll(&mut context);

    if a.is_pending() {
        let h = spawn(move || loop {
            let waker = futures::task::noop_waker();
            let mut context = Context::from_waker(&waker);
            let a = Pin::new(&mut future).poll(&mut context);
            if a.is_ready() {
                break future.value
            }
        });
        println!("?????????????");
        let v = h.join().unwrap();
        println!("ok {v}");

    } else {
        println!("ok {}",future.value);
    }

    // future.await;
}

trait MyTrait<T> {
    fn my_do(&self) -> T;
}
struct MyStruct(String);

impl MyTrait<i32> for MyStruct {
    fn my_do(&self) -> i32 {
        1
    }
}
impl MyTrait<String> for MyStruct {
    fn my_do(&self) -> String {
        "ss".to_owned()
    }
}
    #[test]
    fn tt() {

        let b = "".to_string().trim();
        // println!("{}",b);
    }

fn get_m()->MyStruct
{
    MyStruct("0".to_string())
}
fn get_s()->String
{
    " 0 ".to_string()
}



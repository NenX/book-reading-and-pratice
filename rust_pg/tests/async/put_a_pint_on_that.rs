use std::{future::Future, pin::Pin, time::Duration};

enum SelfRefFutureState {
    Unresumed,        // Created and wasn't polled yet.
    SuspensionPoint1, // First `await` point.
    Returned,
    Poisoned,         // `panic!`ed.
}

struct SelfRefFuture {
    state: SelfRefFutureState,
    v: [i32; 3],
    x: *mut i32, // a pointer into `self.v`,
                 // a problem if `self` moves, but only after it is set.
    sleep: Option<tokio::time::Sleep>,

    // Ensure the we are !Unpin.
    _m: std::marker::PhantomPinned,
}

impl SelfRefFuture {
    fn new() -> Self {
        Self {
            state: SelfRefFutureState::Unresumed,
            v: [0; 3],
            x: std::ptr::null_mut(),
            sleep: None,
            _m: std::marker::PhantomPinned,
        }
    }
}

impl Future for SelfRefFuture {
    type Output = ();

    fn poll(
        self: Pin<&mut Self>,
        cx: &mut std::task::Context<'_>,
    ) -> std::task::Poll<Self::Output> {
        // Safety: We aren't going to move `self`, promise.
        let this = unsafe { self.get_unchecked_mut() };

        match this.state {
            SelfRefFutureState::Unresumed => {
                this.v = [1, 2, 3];
                this.x = this.v.as_mut_ptr().wrapping_add(1);
                this.sleep = Some(tokio::time::sleep(Duration::from_secs(1)));
                this.state = SelfRefFutureState::SuspensionPoint1;

                // Safety: We are the owners of `sleep`, and we aren't moving it.
                let pinned_sleep = unsafe { Pin::new_unchecked(this.sleep.as_mut().unwrap()) };
                Future::poll(pinned_sleep, cx)
            }
            SelfRefFutureState::SuspensionPoint1 => {
                // Safety: Same as above.
                let pinned_sleep = unsafe { Pin::new_unchecked(this.sleep.as_mut().unwrap()) };

                if let std::task::Poll::Pending = Future::poll(pinned_sleep, cx) {
                    return std::task::Poll::Pending;
                };

                // Safety: We initialized `v` and `x` before moving to this state,
                // No one else can move us because `Self` is wrapped in a `Pin`,
                // so `x` is still valid.
                unsafe { this.x.write(42) };
                this.state = SelfRefFutureState::Returned;

                std::task::Poll::Ready(())
            }
            SelfRefFutureState::Returned => std::task::Poll::Ready(()),
            SelfRefFutureState::Poisoned => {
                panic!()
            }
        }
    }
}

#[tokio::main]
async fn main() {
    let f = SelfRefFuture::new();
    f.await;
}

use tokio::{pin, time::timeout};

#[tokio::test]
async fn with_timeout_twice() {
    let f = async { &1u32 };

    pin!(f);  // f is now a `Pin<&mut impl Future<Output = u32>>`.
    let mut a = f;
    let _ = timeout(Duration::from_secs(1), &mut a).await;
    let _ = timeout(Duration::from_secs(1), &mut a).await;
}
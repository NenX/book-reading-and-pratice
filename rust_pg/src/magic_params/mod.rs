#[derive(Clone, Debug)]

pub struct MyUrl(pub String);

#[derive(Clone, Debug)]
pub struct MyName(pub String);
#[derive(Clone)]
pub struct Context {
    pub i: i32,
    pub u: MyUrl,
    pub n: MyName,
}

pub trait Handler<'a, A> {
    fn callx(&self, c: &'a Context);
}
impl<'a, A, F> Handler<'a, A> for F
where
    F: Fn(A),
    A: From<&'a Context>,
{
    fn callx(&self, c: &'a Context) {
        (self)(c.into())
    }
}
impl<'a, A1, A2, F> Handler<'a, (A1, A2)> for F
where
    F: Fn(A1, A2),
    A1: From<&'a Context>,
    A2: From<&'a Context>,
{
    fn callx(&self, c: &'a Context) {
        (self)(A1::from(c), A2::from(c))
    }
}

impl From<&Context> for i32 {
    fn from(value: &Context) -> Self {
        value.i
    }
}

impl From<&str> for MyUrl {
    fn from(value: &str) -> Self {
        Self(value.to_owned())
    }
}
impl From<&Context> for MyUrl {
    fn from(value: &Context) -> Self {
        value.u.clone()
    }
}

impl From<&Context> for MyName {
    fn from(value: &Context) -> Self {
        value.n.clone()
    }
}
impl From<&str> for MyName {
    fn from(value: &str) -> Self {
        Self(value.to_owned())
    }
}
pub fn trigger<'a, T>(c: &'a Context, cb: impl Handler<'a, T>) {
    cb.callx(c)
}
#[cfg(test)]
mod tets_magic_params {
    use super::{trigger, Context, MyName, MyUrl};

    #[test]
    fn test_magic_params() {
        let c = Context {
            i: 22,
            u: "url".into(),
            n: "name".into(),
        };
        trigger(&c, |MyName(x): MyName, i: i32| println!("?? {x}  {i}"));
        trigger(&c, |MyUrl(x): MyUrl| println!("?? {x} "));
    }
}



pub trait Visit<T = ()> {
    fn visit_aa(&self, a: &Aa) -> T;
    fn visit_bb(&self, b: &Bb) -> T;
}

pub trait Base {
    fn access<T>(&self, v: &impl Visit<T>) -> T;
}
pub struct Aa {
    pub name: String,
}
pub struct Bb {
    pub age: u32,
}
impl Base for Aa {
    fn access<T>(&self, v: &impl Visit<T>) -> T {
        v.visit_aa(self)
    }
}
impl Base for Bb {
    fn access<T>(&self, v: &impl Visit<T>) -> T {
        v.visit_bb(self)
    }
}

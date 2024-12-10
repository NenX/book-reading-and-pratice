use super::data::{Aa, Bb, Visit};

pub struct PrintVisit;
impl Visit<String> for PrintVisit {
    fn visit_aa(&self, a: &Aa) -> String {
        format!("(name: {})", a.name)
    }

    fn visit_bb(&self, b: &Bb) -> String {
        format!("(age: {})", b.age)
    }
}

#[derive(Clone, PartialEq, Debug)]
pub struct MyJsToken {
    pub kind: MyJsTokenKind,
    pub start: usize,
    pub end: usize,
    pub value: MyJsTokenValue,
}
#[derive(Clone, Copy, PartialEq, Debug)]
pub enum MyJsTokenKind {
    Eof,
    Plus,

    Identifier,
    If,
    While,
    For,
    Number,
    String,
}
#[derive(Clone, PartialEq, Debug)]
pub enum MyJsTokenValue {
    None,
    Number(f64),
    String(String),
}

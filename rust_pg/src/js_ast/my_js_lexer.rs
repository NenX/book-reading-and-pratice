use std::str::Chars;

use serde::Serialize;

use super::my_js_token::{MyJsToken, MyJsTokenKind, MyJsTokenValue};

pub struct MyJsLexer<'a> {
    source: &'a str,
    chars: Chars<'a>,
}
impl<'a> MyJsLexer<'a> {
    pub fn new(s: &'a str) -> Self {
        let my_js_lexer = MyJsLexer {
            source: s,
            chars: s.chars(),
        };
        my_js_lexer
    }
    pub fn offset(&self) -> usize {
        self.source.len() - self.chars.as_str().len()
    }
    pub fn read_next_kind(&mut self) -> MyJsTokenKind {
        while let Some(c) = self.chars.next() {
            match c {
                '+' => return MyJsTokenKind::Plus,
                _ => continue,
            }
        }
        MyJsTokenKind::Eof
    }
    pub fn read_next_token(&mut self) -> MyJsToken {
        let start = self.offset();
        let kind = self.read_next_kind();
        let end = self.offset();
        let id = self.source[start..end].to_string();
        MyJsToken {
            start,
            kind,
            end,
            value: MyJsTokenValue::String(id),
        }
    }
    pub fn peek(&self) -> Option<char> {
        let a = self.chars.clone().next();
        a
    }
}


#[derive(Debug, Clone, Serialize, PartialEq)]
#[serde(tag = "type")]
#[serde(rename = "Identifier")]
pub struct A {
   i:i32
}

#[test]
fn t() {
    let a = A{i:22};
    let b = serde_json::to_string(&a).unwrap();
}

// Pratt parsing (Top-down operator precedence) is a parsing technique that is a bit more powerful than the recursive descent parsing.

use std::io::BufRead;

enum S {
    Atom(char),
    Cons(char, Vec<S>),
}
impl std::fmt::Display for S {
    fn fmt(&self, f: &mut std::fmt::Formatter) -> std::fmt::Result {
        match self {
            S::Atom(c) => write!(f, "{}", c),
            S::Cons(op, args) => {
                let args = args.iter();
                write!(f, "({}", op)?;
                for arg in args {
                    write!(f, " {}", arg)?;
                }
                write!(f, ")")
            }
        }
    }
}

#[derive(Debug, Clone, Copy, PartialEq, Eq)]
enum Token {
    Atom(char),
    Op(char),
    EOF,
}
struct Lexer {
    tokens: Vec<Token>,
}
impl Lexer {
    fn new(input: &str) -> Self {
        let mut tokens = Vec::new();
        for c in input.chars().filter(|c| !c.is_whitespace()) {
            match c {
                '0'..='9' | 'a'..='z' | 'A'..='Z' => tokens.push(Token::Atom(c)),
                _ => tokens.push(Token::Op(c)),
            }
        }
        tokens.push(Token::EOF);
        tokens.reverse();
        Lexer { tokens }
    }
    fn next(&mut self) -> Token {
        self.tokens.pop().unwrap()
    }
    fn peek(&self) -> Token {
        self.tokens.last().copied().unwrap()
    }
}
fn prefix_binding_power(op: char) -> ((), u8) {
    match op {
        '+' | '-' => ((), 9),
        _ => panic!("Unknown operator: {}", op),
    }
}
fn infix_binding_power(op: char) -> Option<(u8, u8)> {
    match op {
        '=' => Some((2, 1)),
        '?' => Some((4, 3)),
        '+' | '-' => Some((5, 6)),
        '*' | '/' => Some((7, 8)),
        '.' => Some((14, 13)),
        _ => None,
    }
}
fn postfix_binding_power(op: char) -> Option<(u8, ())> {
    match op {
        '[' => Some((11, ())),
        '!' => Some((11, ())),
        _ => None,
    }
}
fn expr_bp(lexer: &mut Lexer, min_bp: u8) -> S {
    let mut lhs = match lexer.next() {
        Token::Atom(c) => S::Atom(c),
        Token::Op('(') => {
            let e = expr_bp(lexer, 0);
            assert_eq!(lexer.next(), Token::Op(')'));
            e
        }
        Token::Op(op) => {
            let ((), r_bp) = prefix_binding_power(op);
            let e = expr_bp(lexer, r_bp);
            S::Cons(op, vec![e])
        }
        _ => panic!("Unexpected token"),
    };
    loop {
        let op = match lexer.peek() {
            Token::Op(op) => op,
            Token::EOF => break,
            _ => panic!("Unexpected token"),
        };

        if let Some((l_bp, r_bp)) = infix_binding_power(op) {
            if l_bp < min_bp {
                break;
            }
            lexer.next();
            lhs = if op == '?' {
                let mhs = expr_bp(lexer, 0);
                assert_eq!(lexer.next(), Token::Op(':'));
                let rhs = expr_bp(lexer, r_bp);
                S::Cons(op, vec![lhs, mhs, rhs])
            } else {
                let rhs = expr_bp(lexer, r_bp);
                S::Cons(op, vec![lhs, rhs])
            };
            continue;
        }
        if let Some((l_bp, ())) = postfix_binding_power(op) {
            if l_bp < min_bp {
                break;
            }
            lexer.next();
            lhs = if op == '[' {
                let rhs = expr_bp(lexer, 0);
                assert_eq!(lexer.next(), Token::Op(']'));
                S::Cons(op, vec![lhs, rhs])
            } else {
                S::Cons(op, vec![lhs])
            };
            continue;
        }
        break;
    }
    lhs
}
fn expr(input: &str) -> S {
    let mut lexer = Lexer::new(input);
    expr_bp(&mut lexer, 0)
}
#[test]
fn tests() {
    let s = expr("1");
    assert_eq!(s.to_string(), "1");

    let s = expr("1 + 2 * 3");
    assert_eq!(s.to_string(), "(+ 1 (* 2 3))");

    let s = expr("a + b * c * d + e");
    assert_eq!(s.to_string(), "(+ (+ a (* (* b c) d)) e)");

    let s = expr("f . g . h");
    assert_eq!(s.to_string(), "(. f (. g h))");

    let s = expr("1 + 2 + f . g . h * 3 * 4");
    assert_eq!(s.to_string(), "(+ (+ 1 2) (* (* (. f (. g h)) 3) 4))");

    let s = expr("--1 * 2");
    assert_eq!(s.to_string(), "(* (- (- 1)) 2)");

    let s = expr("--f . g");
    assert_eq!(s.to_string(), "(- (- (. f g)))");

    let s = expr("-9!");
    assert_eq!(s.to_string(), "(- (! 9))");

    let s = expr("f . g !");
    assert_eq!(s.to_string(), "(! (. f g))");

    let s = expr("(((0)))");
    assert_eq!(s.to_string(), "0");

    let s = expr("x[0][1]");
    assert_eq!(s.to_string(), "([ ([ x 0) 1)");

    let s = expr(
        "a ? b :
         c ? d
         : e",
    );
    assert_eq!(s.to_string(), "(? a b (? c d e))");

    let s = expr("a = 0 ? b : c = d");
    assert_eq!(s.to_string(), "(= a (= (? 0 b c) d))")
}

fn main() {
    for line in std::io::stdin().lock().lines() {
        let line = line.unwrap();
        let s = expr(&line);
        println!("{}", s)
    }
}

use std::{
    error::Error,
    io::{BufRead, Write},
};

use super::WsMessage;

pub fn handle_connection(
    reader: &mut impl BufRead,
    writer: &mut impl Write,
) -> Result<(), Box<dyn Error>> {
    while let Ok(message) = WsMessage::decode_message(reader) {
        match message {
            WsMessage::Text(text) => {
                println!("text {text}");
            }
            WsMessage::Binary(data) => {
                println!("Binary {data:?}")
            }
        }
    }
    Ok(())
}

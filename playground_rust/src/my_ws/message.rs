use std::{
    error::Error,
    io::{self, BufRead, ErrorKind},
};


#[derive(Debug)]
pub enum WsMessage {
    Text(String),
    Binary(Vec<u8>),
}

impl WsMessage {
    fn as_bytes(&self) -> &[u8] {
        match &self {
            WsMessage::Text(t) => t.as_bytes(),
            WsMessage::Binary(b) => b,
        }
    }
    fn opcode(&self) -> u8 {
        match &self {
            WsMessage::Text(t) => 1,
            WsMessage::Binary(b) => 2,
        }
    }
    pub fn encode(&self) -> Vec<u8> {
        let bytes = self.as_bytes();
        let mut total_frame_len: u64 = 2;
        let payload_len = bytes.len() as u64;

        if payload_len <= 125 {
        } else if payload_len > u16::MAX as u64 {
            total_frame_len += 8;
        } else {
            total_frame_len += 2;
        };

        total_frame_len += payload_len;

        let mut frame = Vec::with_capacity(total_frame_len as usize);

        frame.push(0b1000_0000 | self.opcode());

        if payload_len <= 125 {
            frame.push(payload_len as u8);
        } else if payload_len > u16::MAX as u64 {
            frame.push(127);

            frame.extend_from_slice(&payload_len.to_be_bytes());
        } else {
            frame.push(126);

            frame.extend_from_slice(&(payload_len as u16).to_be_bytes());
        };
        frame.extend_from_slice(bytes);
        frame
    }
    pub fn decode_message(reader: &mut impl BufRead) -> Result<WsMessage, Box<dyn Error>> {
        // decode_message2(reader)
        decode_message(reader)
    }
}
fn decode_message(reader: &mut impl BufRead) -> Result<WsMessage, Box<dyn Error>> {
    let mut payload_length: u64;
    let mut buffer = [0; 2];
    reader.read_exact(&mut buffer)?;

    let opcode = buffer[0] & 0b1111;

    let mask = buffer[1] >> 7;

    if mask == 0 {
        let e = io::Error::new(ErrorKind::ConnectionRefused, "");
        return Err(e.into());
    }

    payload_length = u64::from(buffer[1] & (0b0111_1111));
    if payload_length == 126 {
        let mut len_buf = [0; 2];
        reader.read_exact(&mut len_buf)?;

        payload_length = u16::from_be_bytes(len_buf).into();
    } else if payload_length == 127 {
        let mut len_buf = [0; 8];
        reader.read_exact(&mut len_buf)?;

        payload_length = u64::from_be_bytes(len_buf);
    }
    let mut mask_buf = [0; 4];

    reader.read_exact(&mut mask_buf)?;

    // let mut payload_data = Vec::with_capacity(payload_length as usize);
    let mut payload_data: Vec<u8> = vec![0; payload_length as usize];

    reader.read_exact(&mut payload_data)?;

    for (i, item) in payload_data.iter_mut().enumerate() {
        *item ^= mask_buf[i % 4];
    }
    if opcode == 1 {
        let s = String::from_utf8_lossy(&payload_data);

        Ok(WsMessage::Text(s.to_string()))
    } else {
        Ok(WsMessage::Binary(payload_data))
    }
}

#[test]
fn tt() {
    assert_eq!(u16::MAX, 65535)
}

use std::u8;

use super::{
    dns_mock_response_packet,
    types::{MyDnsResult, MyDnsResultCode},
    BytePacketBuffer,
};

#[derive(Clone, Debug, PartialEq)]
pub struct MyDnsHeader {
    pub id: u16, // 16 bits

    pub recursion_desired: bool,    // 1 bit
    pub truncated_message: bool,    // 1 bit
    pub authoritative_answer: bool, // 1 bit
    pub opcode: u8,                 // 4 bits
    pub response: bool,             // 1 bit

    pub rescode: MyDnsResultCode,       // 4 bits
    pub checking_disabled: bool,   // 1 bit
    pub authed_data: bool,         // 1 bit
    pub z: bool,                   // 1 bit
    pub recursion_available: bool, // 1 bit

    pub questions: u16,             // 16 bits
    pub answers: u16,               // 16 bits
    pub authoritative_entries: u16, // 16 bits
    pub resource_entries: u16,      // 16 bits
}

impl Default for MyDnsHeader {
    fn default() -> Self {
        Self::new()
    }
}

impl MyDnsHeader {
    pub fn new() -> MyDnsHeader {
        MyDnsHeader {
            id: 0,

            recursion_desired: false,
            truncated_message: false,
            authoritative_answer: false,
            opcode: 0,
            response: false,

            rescode: MyDnsResultCode::NOERROR,
            checking_disabled: false,
            authed_data: false,
            z: false,
            recursion_available: false,

            questions: 0,
            answers: 0,
            authoritative_entries: 0,
            resource_entries: 0,
        }
    }

    pub fn read(&mut self, buffer: &mut BytePacketBuffer) -> MyDnsResult<()> {
        // Return the constant header size
        self.id = buffer.read_u16()?;

        let b2 = buffer.read()?;

        self.response = b2 >> 7 == 1;
        self.opcode = b2 & 0b0111_1000;
        self.authoritative_answer = b2 & 0b0000_0100 > 0;
        self.truncated_message = b2 & 0b0000_0010 > 0;
        self.recursion_desired = b2 & 0b0000_0001 > 0;

        let b3 = buffer.read()?;

        self.recursion_available = b3 & 0b1000_0000 > 0;
        self.z = b3 & 0b0111_0000 > 0;

        let rcode = b3 & 0b0000_1111;

        self.rescode = MyDnsResultCode::from_num(rcode);

        self.questions = buffer.read_u16()?;
        self.answers = buffer.read_u16()?;
        self.authoritative_entries = buffer.read_u16()?;
        self.resource_entries = buffer.read_u16()?;
        Ok(())
    }
    pub fn write(&mut self, buffer: &mut BytePacketBuffer) -> MyDnsResult<()> {
        buffer.write_u16(self.id)?;

        let mut b2 = u8::MIN;
        let mut b3 = u8::MIN;

        if self.response {
            b2 |= 0b1000_0000;
        }

        b2 |= self.opcode;

        if self.authoritative_answer {
            b2 |= 0b0000_0100;
        }

        if self.truncated_message {
            b2 |= 0b0000_0010;
        }

        if self.recursion_desired {
            b2 |= 0b0000_0001;
        }
        if self.recursion_available {
            b3 |= 0b1000_0000;
        }

        if self.z {
            b3 |= 0b0001_0000;
        }
        b3 |= self.rescode as u8;

        buffer.write(b2)?;
        buffer.write(b3)?;

        buffer.write_u16(self.questions)?;
        buffer.write_u16(self.answers)?;
        buffer.write_u16(self.authoritative_entries)?;
        buffer.write_u16(self.resource_entries)?;

        Ok(())
    }
}

#[test]
fn test_write_header() {
    let mock = dns_mock_response_packet();
    let mut buf = BytePacketBuffer::from(mock);
    let mut buf_write = BytePacketBuffer::new();

    let mut a = MyDnsHeader::new();
    a.read(&mut buf).unwrap();

    a.write(&mut buf_write).unwrap();
}

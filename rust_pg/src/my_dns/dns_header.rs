use super::{
    types::{MyDns_Result, ResultCode},
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

    pub rescode: ResultCode,       // 4 bits
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

            rescode: ResultCode::NOERROR,
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

    pub fn read(&mut self, buffer: &mut BytePacketBuffer) -> MyDns_Result<()> {
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

        self.rescode = ResultCode::from_num(rcode);

        self.questions = buffer.read_u16()?;
        self.answers = buffer.read_u16()?;
        self.authoritative_entries = buffer.read_u16()?;
        self.resource_entries = buffer.read_u16()?;
        Ok(())
    }

    pub fn read2(&mut self, buffer: &mut BytePacketBuffer) -> MyDns_Result<()> {
        self.id = buffer.read_u16()?;

        let flags = buffer.read_u16()?;
        let a = (flags >> 8) as u8;
        let b = (flags & 0xFF) as u8;
        self.recursion_desired = (a & (1 << 0)) > 0;
        self.truncated_message = (a & (1 << 1)) > 0;
        self.authoritative_answer = (a & (1 << 2)) > 0;
        self.opcode = (a >> 3) & 0x0F;
        self.response = (a & (1 << 7)) > 0;

        self.rescode = ResultCode::from_num(b & 0x0F);
        self.checking_disabled = (b & (1 << 4)) > 0;
        self.authed_data = (b & (1 << 5)) > 0;
        self.z = (b & (1 << 6)) > 0;
        self.recursion_available = (b & (1 << 7)) > 0;

        self.questions = buffer.read_u16()?;
        self.answers = buffer.read_u16()?;
        self.authoritative_entries = buffer.read_u16()?;
        self.resource_entries = buffer.read_u16()?;

        // Return the constant header size
        Ok(())
    }
}

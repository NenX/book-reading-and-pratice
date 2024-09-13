use super::{
    types::{MyDnsResult, MyDnsQueryType},
    BytePacketBuffer,
};

#[derive(Debug, Clone, PartialEq, Eq)]
pub struct DnsQuestion {
    pub name: String,
    pub qtype: MyDnsQueryType,
}

impl DnsQuestion {
    pub fn new(name: String, qtype: MyDnsQueryType) -> DnsQuestion {
        DnsQuestion { name, qtype }
    }

    pub fn read(&mut self, buffer: &mut BytePacketBuffer) -> MyDnsResult<()> {
        buffer.read_qname(&mut self.name)?;
        self.qtype = MyDnsQueryType::from_num(buffer.read_u16()?); // qtype
        let _ = buffer.read_u16()?; // class

        Ok(())
    }
    pub fn write(&mut self, buffer: &mut BytePacketBuffer) -> MyDnsResult<()> {
        buffer.write_qnames(&self.name)?;
        buffer.write_u16(self.qtype.to_num())?;
        buffer.write_u16(1)?;
        Ok(())
    }
}

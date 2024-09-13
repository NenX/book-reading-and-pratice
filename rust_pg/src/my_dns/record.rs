use std::net::{Ipv4Addr, Ipv6Addr};

use super::{
    types::{MyDnsQueryType, MyDnsResult},
    BytePacketBuffer,
};

#[derive(Debug, Clone, PartialEq, Eq, Hash, PartialOrd, Ord)]
#[allow(dead_code)]
pub enum MyDnsRecord {
    UNKNOWN {
        domain: String,
        qtype: u16,
        data_len: u16,
        ttl: u32,
    }, // 0
    A {
        domain: String,
        addr: Ipv4Addr,
        ttl: u32,
    }, // 1
    NS {
        domain: String,
        host: String,
        ttl: u32,
    }, // 2
    CNAME {
        domain: String,
        host: String,
        ttl: u32,
    }, // 5
    MX {
        domain: String,
        priority: u16,
        host: String,
        ttl: u32,
    }, // 15
    AAAA {
        domain: String,
        addr: Ipv6Addr,
        ttl: u32,
    }, // 28
}

impl MyDnsRecord {
    pub fn read(buffer: &mut BytePacketBuffer) -> MyDnsResult<MyDnsRecord> {
        let mut domain = String::new();
        buffer.read_qname(&mut domain)?;

        let qtype_num = buffer.read_u16()?;
        let qtype = MyDnsQueryType::from_num(qtype_num);
        let _ = buffer.read_u16()?;
        let ttl = buffer.read_u32()?;
        let data_len = buffer.read_u16()?;

        match qtype {
            MyDnsQueryType::A => {
                let raw_addr = buffer.read_u32()?;
                let addr = Ipv4Addr::new(
                    ((raw_addr >> 24) & 0xFF) as u8,
                    ((raw_addr >> 16) & 0xFF) as u8,
                    ((raw_addr >> 8) & 0xFF) as u8,
                    (raw_addr & 0xFF) as u8,
                );

                Ok(MyDnsRecord::A { domain, addr, ttl })
            }
            MyDnsQueryType::UNKNOWN(_) => {
                buffer.step(data_len as usize)?;

                Ok(MyDnsRecord::UNKNOWN {
                    domain,
                    qtype: qtype_num,
                    data_len,
                    ttl,
                })
            }
            MyDnsQueryType::NS => {
                let mut host = String::new();
                buffer.read_qname(&mut host)?;

                Ok(MyDnsRecord::NS { domain, host, ttl })
            }
            MyDnsQueryType::CNAME => {
                let mut host = String::new();
                buffer.read_qname(&mut host)?;

                Ok(MyDnsRecord::CNAME { domain, host, ttl })
            }
            MyDnsQueryType::MX => {
                let priority = buffer.read_u16()?;
                let mut host = String::new();
                buffer.read_qname(&mut host)?;

                Ok(MyDnsRecord::MX {
                    domain,
                    priority,
                    host,
                    ttl,
                })
            }
            MyDnsQueryType::AAAA => {
                let raw_addr1 = buffer.read_u32()?;
                let raw_addr2 = buffer.read_u32()?;
                let raw_addr3 = buffer.read_u32()?;
                let raw_addr4 = buffer.read_u32()?;
                let addr = Ipv6Addr::new(
                    ((raw_addr1 >> 16) & 0xFFFF) as u16,
                    ((raw_addr1 >> 0) & 0xFFFF) as u16,
                    ((raw_addr2 >> 16) & 0xFFFF) as u16,
                    ((raw_addr2 >> 0) & 0xFFFF) as u16,
                    ((raw_addr3 >> 16) & 0xFFFF) as u16,
                    ((raw_addr3 >> 0) & 0xFFFF) as u16,
                    ((raw_addr4 >> 16) & 0xFFFF) as u16,
                    ((raw_addr4 >> 0) & 0xFFFF) as u16,
                );

                Ok(MyDnsRecord::AAAA { domain, addr, ttl })
            }
        }
    }
    pub fn write(&mut self, buffer: &mut BytePacketBuffer) -> MyDnsResult<()> {
        match self {
            MyDnsRecord::UNKNOWN {
                domain: _,
                qtype: _,
                data_len: _,
                ttl: _,
            } => {}
            MyDnsRecord::A { domain, addr, ttl } => {
                buffer.write_qnames(domain)?;

                buffer.write_u16(MyDnsQueryType::A.to_num())?;

                buffer.write_u16(1)?;

                buffer.write_u32(*ttl)?;
                buffer.write_u16(4)?;
                let oc = addr.octets();
                buffer.write(oc[0])?;
                buffer.write(oc[1])?;
                buffer.write(oc[2])?;
                buffer.write(oc[3])?;
            }
            MyDnsRecord::NS { domain, host, ttl } => {
                buffer.write_qnames(domain)?;
                buffer.write_u16(MyDnsQueryType::NS.to_num())?;
                buffer.write_u16(1)?;
                buffer.write_u32(*ttl)?;

                let pos = buffer.pos();
                buffer.write_u16(0)?;

                buffer.write_qnames(host)?;

                let size = buffer.pos() - (pos + 2);
                buffer.set_u16(pos, size as u16)?;
            }
            MyDnsRecord::CNAME { domain, host, ttl } => {
                buffer.write_qnames(domain)?;
                buffer.write_u16(MyDnsQueryType::CNAME.to_num())?;
                buffer.write_u16(1)?;
                buffer.write_u32(*ttl)?;

                let pos = buffer.pos();
                buffer.write_u16(0)?;

                buffer.write_qnames(host)?;

                let size = buffer.pos() - (pos + 2);
                buffer.set_u16(pos, size as u16)?;
            }
            MyDnsRecord::MX {
                domain,
                priority,
                host,
                ttl,
            } => {
                buffer.write_qnames(domain)?;
                buffer.write_u16(MyDnsQueryType::MX.to_num())?;
                buffer.write_u16(1)?;
                buffer.write_u32(*ttl)?;

                let pos = buffer.pos();
                buffer.write_u16(0)?;

                buffer.write_u16(*priority)?;
                buffer.write_qnames(host)?;

                let size = buffer.pos() - (pos + 2);
                buffer.set_u16(pos, size as u16)?;
            }
            MyDnsRecord::AAAA { domain, addr, ttl } => {
                buffer.write_qnames(domain)?;
                buffer.write_u16(MyDnsQueryType::AAAA.to_num())?;
                buffer.write_u16(1)?;
                buffer.write_u32(*ttl)?;
                buffer.write_u16(16)?;

                for octet in &addr.segments() {
                    buffer.write_u16(*octet)?;
                }
            }
        };
        Ok(())
    }
}

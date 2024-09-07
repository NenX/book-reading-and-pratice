
use super::dns_header::MyDnsHeader;
use super::question::{DnsQuestion, QueryType};
use super::record::MyDnsRecord;
use super::types::MyDns_Result;
use super::BytePacketBuffer;

#[derive(Clone, Debug)]
pub struct MyDnsPacket {
    pub header: MyDnsHeader,
    pub questions: Vec<DnsQuestion>,
    pub answers: Vec<MyDnsRecord>,
    pub authorities: Vec<MyDnsRecord>,
    pub resources: Vec<MyDnsRecord>,
}

impl Default for MyDnsPacket {
    fn default() -> Self {
        Self::new()
    }
}

impl MyDnsPacket {
    pub fn new() -> MyDnsPacket {
        MyDnsPacket {
            header: MyDnsHeader::new(),
            questions: Vec::new(),
            answers: Vec::new(),
            authorities: Vec::new(),
            resources: Vec::new(),
        }
    }

    pub fn from_buffer(buffer: &mut BytePacketBuffer) -> MyDns_Result<MyDnsPacket> {
        let mut result = MyDnsPacket::new();
        result.header.read(buffer)?;

        for _ in 0..result.header.questions {
            let mut question = DnsQuestion::new("".to_string(), QueryType::UNKNOWN(0));
            question.read(buffer)?;
            result.questions.push(question);
        }

        for _ in 0..result.header.answers {
            let rec = MyDnsRecord::read(buffer)?;
            result.answers.push(rec);
        }
        for _ in 0..result.header.authoritative_entries {
            let rec = MyDnsRecord::read(buffer)?;
            result.authorities.push(rec);
        }
        for _ in 0..result.header.resource_entries {
            let rec = MyDnsRecord::read(buffer)?;
            result.resources.push(rec);
        }

        Ok(result)
    }
}

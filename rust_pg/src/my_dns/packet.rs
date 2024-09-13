use std::net::Ipv4Addr;

use super::dns_header::MyDnsHeader;
use super::question::DnsQuestion;
use super::record::MyDnsRecord;
use super::types::{MyDnsQueryType, MyDnsResult};
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

    pub fn from_buffer(buffer: &mut BytePacketBuffer) -> MyDnsResult<MyDnsPacket> {
        let mut result = MyDnsPacket::new();
        result.header.read(buffer)?;

        for _ in 0..result.header.questions {
            let mut question = DnsQuestion::new("".to_string(), MyDnsQueryType::UNKNOWN(0));
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

    pub fn write(&mut self, buffer: &mut BytePacketBuffer) -> MyDnsResult<()> {
        self.header.questions = self.questions.len() as u16;
        self.header.answers = self.answers.len() as u16;
        self.header.authoritative_entries = self.authorities.len() as u16;
        self.header.resource_entries = self.resources.len() as u16;

        self.header.write(buffer)?;

        for question in &mut self.questions {
            question.write(buffer)?;
        }
        for rec in &mut self.answers {
            rec.write(buffer)?;
        }
        for rec in &mut self.authorities {
            rec.write(buffer)?;
        }
        for rec in &mut self.resources {
            rec.write(buffer)?;
        }
        Ok(())
    }
}

impl MyDnsPacket {
    pub fn get_random_a(&self) -> Option<Ipv4Addr> {
        let a = self
            .answers
            .iter()
            .filter_map(|x| match x {
                MyDnsRecord::A {
                    domain: _,
                    addr,
                    ttl: _,
                } => Some(addr),
                _ => None,
            })
            .next()
            .cloned();
        a
    }
    pub fn get_ns<'a>(&'a self, qname: &'a str) -> impl Iterator<Item = (&str, &str)> + 'a {
        let a = self.authorities.iter().filter_map(|x| match x {
            MyDnsRecord::NS { domain, host, .. } if qname.ends_with(domain) => {
                Some((domain.as_str(), host.as_str()))
            }
            _ => None,
        });
        a
    }
    pub fn get_ns2<'a>(&'a self, qname: &'a str) -> impl Iterator<Item = (&'a str, &'a str)> {
        self.authorities
            .iter()
            // In practice, these are always NS records in well formed packages.
            // Convert the NS records to a tuple which has only the data we need
            // to make it easy to work with.
            .filter_map(|record| match record {
                MyDnsRecord::NS { domain, host, .. } => Some((domain.as_str(), host.as_str())),
                _ => None,
            })
            // Discard servers which aren't authoritative to our query
            // 为什么这里需要 move ????
            .filter(move |(domain, _)| qname.ends_with(*domain))
    }
    pub fn get_resolved_ns(&self, qname: &str) -> Option<Ipv4Addr> {
        let a = self
            .get_ns(qname)
            .flat_map(|(_, host)| {
                self.resources.iter().filter_map(move |r| match r {
                    MyDnsRecord::A { domain, addr, .. } if host == domain => Some(addr),
                    _ => None,
                })
            })
            .next()
            .cloned();
        println!("get_resolved_ns {} {:?} {:?}",qname,a ,self.get_ns(qname).count());

        a
    }
    pub fn get_unresolved_ns<'a>(&'a self, qname: &'a str) -> Option<&'a str> {
        let a = self.get_ns(qname).map(|(_, host)| host).next();
        a
    }
}

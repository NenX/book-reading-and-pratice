use std::error::Error;

pub const MY_DNS_MAX_POS_BOUND: usize = 512;

pub type MyDnsError = Box<dyn Error>;
pub type MyDnsResult<T> = Result<T, MyDnsError>;
pub type MyDnsBuf = [u8; 512];

#[derive(Copy, Clone, Debug, PartialEq, Eq)]
pub enum MyDnsResultCode {
    NOERROR = 0,
    FORMERR = 1,
    SERVFAIL = 2,
    NXDOMAIN = 3,
    NOTIMP = 4,
    REFUSED = 5,
}

impl MyDnsResultCode {
    pub fn from_num(num: u8) -> MyDnsResultCode {
        match num {
            1 => MyDnsResultCode::FORMERR,
            2 => MyDnsResultCode::SERVFAIL,
            3 => MyDnsResultCode::NXDOMAIN,
            4 => MyDnsResultCode::NOTIMP,
            5 => MyDnsResultCode::REFUSED,
            0 | _ => MyDnsResultCode::NOERROR,
        }
    }
}

#[derive(PartialEq, Eq, Debug, Clone, Hash, Copy)]
pub enum MyDnsQueryType {
    UNKNOWN(u16),
    A,     // 1 Alias - Mapping names to IP addresses
    NS,    // 2 Name Server - The DNS server address for a domain
    CNAME, // 5 Canonical Name - Maps names to names
    MX,    // 15 Mail eXchange - The host of the mail server for a domain
    AAAA,  // 28 IPv6 alias
}

impl MyDnsQueryType {
    pub fn to_num(&self) -> u16 {
        match *self {
            MyDnsQueryType::UNKNOWN(x) => x,
            MyDnsQueryType::A => 1,
            MyDnsQueryType::NS => 2,
            MyDnsQueryType::CNAME => 5,
            MyDnsQueryType::MX => 15,
            MyDnsQueryType::AAAA => 28,
        }
    }

    pub fn from_num(num: u16) -> MyDnsQueryType {
        match num {
            1 => MyDnsQueryType::A,
            2 => MyDnsQueryType::NS,
            5 => MyDnsQueryType::CNAME,
            15 => MyDnsQueryType::MX,
            28 => MyDnsQueryType::AAAA,
            _ => MyDnsQueryType::UNKNOWN(num),
        }
    }
}

use std::net::{Ipv4Addr, UdpSocket};

use my_rs::my_dns::dns_header::MyDnsHeader;
use my_rs::my_dns::packet::MyDnsPacket;
use my_rs::my_dns::question::DnsQuestion;
use my_rs::my_dns::record::MyDnsRecord;
use my_rs::my_dns::types::{MyDnsQueryType, MyDnsResultCode};
use my_rs::my_dns::{dns_mock_response_packet, BytePacketBuffer};

#[test]
fn test() -> Result<(), Box<dyn std::error::Error>> {
    let qname = "yahoo.com";
    let qtype = MyDnsQueryType::MX;
    let server = ("8.8.8.8", 53);

    let socket = UdpSocket::bind(("0.0.0.0", 43210))?;

    let mut packet = MyDnsPacket::new();

    packet.header.id = 6666;
    packet.header.questions = 1;
    packet.header.recursion_desired = true;
    packet
        .questions
        .push(DnsQuestion::new(qname.to_string(), qtype));

    let mut req_buffer = BytePacketBuffer::new();
    packet.write(&mut req_buffer)?;
    socket.send_to(&req_buffer.buf[0..req_buffer.pos], server)?;

    let mut res_buffer = BytePacketBuffer::new();
    socket.recv_from(&mut res_buffer.buf)?;

    let res_packet = MyDnsPacket::from_buffer(&mut res_buffer)?;
    println!("{:?}", res_packet.header);

    for q in res_packet.questions {
        println!("{:?}", q);
    }
    for rec in res_packet.answers {
        println!("{:?}", rec);
    }
    for rec in res_packet.authorities {
        println!("{:?}", rec);
    }
    for rec in res_packet.resources {
        println!("{:?}", rec);
    }
    assert_eq!(1, 0);
    Ok(())
}

#[test]
fn test_header() {
    let mut buffer: BytePacketBuffer = dns_mock_response_packet().into();

    let packet = MyDnsPacket::from_buffer(&mut buffer).expect("from buffer");
    let h = mock_header();
    assert_eq!(packet.header, h);
}
#[test]
fn test_write_header() {
    let mut buffer_to_write = BytePacketBuffer::new();

    write_header(&mut buffer_to_write);

    assert_eq!(
        buffer_to_write.buf[0..12],
        dns_mock_response_packet()[0..12]
    );
}

#[test]
fn test_question() {
    let mut buffer: BytePacketBuffer = dns_mock_response_packet().into();

    let packet = MyDnsPacket::from_buffer(&mut buffer).expect("from buffer");
    let q1 = packet.questions.first().unwrap();

    assert_eq!(*q1, mock_question());
}
#[test]
fn test_write_question() {
    let mut buffer_to_write = BytePacketBuffer::new();

    write_question(&mut buffer_to_write);

    assert_eq!(
        buffer_to_write.buf[0..15],
        dns_mock_response_packet()[12..27]
    );
}

#[test]
fn test_record() {
    let mut buffer: BytePacketBuffer = dns_mock_response_packet().into();

    let packet = MyDnsPacket::from_buffer(&mut buffer).expect("from buffer");
    let q1 = packet.answers.first().unwrap();
    match q1 {
        MyDnsRecord::UNKNOWN {
            domain,
            qtype,
            data_len,
            ttl,
        } => {
            assert_eq!(q1, &mock_record_unkown());
        }
        MyDnsRecord::A { domain, addr, ttl } => {
            assert_eq!(q1, &mock_record_a().0);
        }
        MyDnsRecord::NS { domain, host, ttl } => todo!(),
        MyDnsRecord::CNAME { domain, host, ttl } => todo!(),
        MyDnsRecord::MX {
            domain,
            priority,
            host,
            ttl,
        } => todo!(),
        MyDnsRecord::AAAA { domain, addr, ttl } => todo!(),
    }
}
#[test]
fn test_write_record() {
    let mut buffer_to_write = BytePacketBuffer::new();

    write_header(&mut buffer_to_write);
    write_question(&mut buffer_to_write);
    write_record_a(&mut buffer_to_write);

    assert_eq!(
        &buffer_to_write.buf[..0x40],
        &dns_mock_response_packet()[..0x40]
    );
}
fn mock_header() -> MyDnsHeader {
    let mut h = MyDnsHeader::new();

    h.id = 13605;
    h.recursion_desired = true;
    h.truncated_message = false;
    h.authoritative_answer = false;
    h.opcode = 0;
    h.response = true;
    h.rescode = MyDnsResultCode::NOERROR;
    h.checking_disabled = false;
    h.authed_data = false;
    h.z = false;
    h.recursion_available = true;
    h.questions = 1;
    h.answers = 2;
    h.authoritative_entries = 0;
    h.resource_entries = 0;
    h
}
fn write_header(buffer: &mut BytePacketBuffer) {
    mock_header().write(buffer).unwrap();
}
fn mock_question() -> DnsQuestion {
    DnsQuestion::new("baidu.com".to_string(), MyDnsQueryType::A)
}
fn write_question(buffer: &mut BytePacketBuffer) {
    mock_question().write(buffer).unwrap();
}

fn mock_record_a() -> (MyDnsRecord, MyDnsRecord) {
    (
        MyDnsRecord::A {
            domain: "baidu.com".to_string(),
            addr: Ipv4Addr::new(110, 242, 68, 66),
            ttl: 47,
        },
        MyDnsRecord::A {
            domain: "baidu.com".to_string(),
            addr: Ipv4Addr::new(0x27, 0x9c, 0x42, 0x0a),
            ttl: 47,
        },
    )
}
fn write_record_a(buffer: &mut BytePacketBuffer) {
    mock_record_a().0.write(buffer).unwrap();
    mock_record_a().1.write(buffer).unwrap();
}

fn mock_record_unkown() -> MyDnsRecord {
    MyDnsRecord::UNKNOWN {
        domain: "baidu".to_string(),
        qtype: 1,
        data_len: 1,
        ttl: 47,
    }
}
fn write_record_unkown(buffer: &mut BytePacketBuffer) {
    mock_record_unkown().write(buffer).unwrap();
}

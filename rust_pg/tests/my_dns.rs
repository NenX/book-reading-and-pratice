
use my_rs::my_dns::dns_header::MyDnsHeader;
use my_rs::my_dns::packet::MyDnsPacket;
use my_rs::my_dns::question::{DnsQuestion, QueryType};
use my_rs::my_dns::record::MyDnsRecord;
use my_rs::my_dns::types::ResultCode;
use my_rs::my_dns::{dns_mock_response_packet, BytePacketBuffer};

#[test]
fn test_header() {
    let mut buffer: BytePacketBuffer = dns_mock_response_packet().into();

    let packet = MyDnsPacket::from_buffer(&mut buffer).expect("from buffer");
    let mut h = MyDnsHeader::new();

    h.id = 13605;
    h.recursion_desired = true;
    h.truncated_message = false;
    h.authoritative_answer = false;
    h.opcode = 0;
    h.response = true;
    h.rescode = ResultCode::NOERROR;
    h.checking_disabled = false;
    h.authed_data = false;
    h.z = false;
    h.recursion_available = true;
    h.questions = 1;
    h.answers = 2;
    h.authoritative_entries = 0;
    h.resource_entries = 0;

    assert_eq!(packet.header, h);
}

#[test]
fn test_question() {
    let mut buffer: BytePacketBuffer = dns_mock_response_packet().into();

    let packet = MyDnsPacket::from_buffer(&mut buffer).expect("from buffer");
    let q1 = packet.questions.first().unwrap();
    let q = DnsQuestion::new("baidu.com".to_string(), QueryType::A);
    assert_eq!(*q1, q);
}

#[test]
fn test_answer_domain() {
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
            assert_eq!(domain, "baidu.com");
        }
        MyDnsRecord::A { domain, addr, ttl } => {
            assert_eq!(domain, "baidu.com");
        }
    }
}

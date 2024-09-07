
use my_rs::my_dns::packet::MyDnsPacket;
use my_rs::my_dns::types::MyDns_Result;
use my_rs::my_dns::{dns_mock_response_packet, BytePacketBuffer};

fn main() -> MyDns_Result<()> {
    // let mut buffer = BytePacketBuffer::new();
    let mut buffer: BytePacketBuffer = dns_mock_response_packet().into();

    let packet = MyDnsPacket::from_buffer(&mut buffer).expect("from buffer");
    println!("header {:#?}", packet.header);

    for q in packet.questions {
        println!("questions {:#?}", q);
    }
    for rec in packet.answers {
        println!("answers {:#?}", rec);
    }
    for rec in packet.authorities {
        println!("authorities {:#?}", rec);
    }
    for rec in packet.resources {
        println!("resources {:#?}", rec);
    }

    Ok(())
}

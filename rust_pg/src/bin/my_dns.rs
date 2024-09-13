use std::{fs, net::UdpSocket};

use my_rs::my_dns::{
    packet::MyDnsPacket, question::DnsQuestion, server::handle_query, types::MyDnsQueryType,
    BytePacketBuffer,
};
// depart apart
fn main() -> Result<(), Box<dyn std::error::Error>> {
    // let socket = UdpSocket::bind(("0.0.0.0", 2053))?;
    let socket = UdpSocket::bind("0.0.0.0:2053")?;
    println!("Upd listen at 2233");
    // let socket = UdpSocket::bind("0.0.0.0:1234")?;
    loop {
        handle_query(&socket)?
    }
}

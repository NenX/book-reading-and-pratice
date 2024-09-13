use std::{
    fs,
    net::{Ipv4Addr, UdpSocket},
    str::FromStr,
};

use crate::my_dns::{packet::MyDnsPacket, question::DnsQuestion, BytePacketBuffer};

use super::types::{MyDnsQueryType, MyDnsResult, MyDnsResultCode};

pub fn lookup(
    qname: &str,
    qtype: MyDnsQueryType,
    server_addr: Ipv4Addr,
) -> MyDnsResult<MyDnsPacket> {
    let server = (server_addr, 53);

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
    println!("req_buffer.pos {}", req_buffer.pos);
    socket.send_to(&req_buffer.buf[0..req_buffer.pos], server)?;

    let mut res_buffer = BytePacketBuffer::new();

    socket.recv_from(&mut res_buffer.buf)?;

    fs::write("recv.txt", res_buffer.buf.clone())?;
    let res_packet = MyDnsPacket::from_buffer(&mut res_buffer)?;

    Ok(res_packet)
}

pub fn recursive_loopup(qname: &str, qtype: MyDnsQueryType) -> MyDnsResult<MyDnsPacket> {
    let mut ns = Ipv4Addr::from_str("198.41.0.4").unwrap();

    loop {
        let r = lookup(qname, qtype, ns)?;
        if r.header.rescode == MyDnsResultCode::NXDOMAIN {
            return Ok(r);
        }
        if !r.answers.is_empty() && r.header.rescode == MyDnsResultCode::NOERROR {
            println!("xxa 了 ======================= 了");

            return Ok(r);
        }

        if let Some(addr) = r.get_resolved_ns(qname) {
            ns = addr;
            continue;
        };

        let name = match r.get_unresolved_ns(qname) {
            Some(x) => x,
            None => return Ok(r),
        };

        let x = recursive_loopup(name, MyDnsQueryType::A)?;

        let a1 = x.get_random_a();
        match a1 {
            Some(addr) => {
                ns = addr;
                continue;
            }
            None => return Ok(r),
        }
    }
}

pub fn handle_query(socket: &UdpSocket) -> MyDnsResult<()> {
    let mut q_buff = BytePacketBuffer::new();
    let mut ans_buff = BytePacketBuffer::new();

    let (_size, client_addr) = socket.recv_from(&mut q_buff.buf)?;

    let mut q_packet = MyDnsPacket::from_buffer(&mut q_buff)?;

    let mut ans_packet = MyDnsPacket::new();
    ans_packet.header.recursion_available = true;
    ans_packet.header.recursion_desired = true;
    ans_packet.header.id = q_packet.header.id;

    if let Some(q) = q_packet.questions.pop() {
        // let remote_packet = lookup(&q.name, q.qtype, [8, 8, 8, 8].into())?;
        let remote_packet = recursive_loopup(&q.name, q.qtype)?;

        ans_packet.header.rescode = remote_packet.header.rescode;
        ans_packet.questions = remote_packet.questions;
        ans_packet.answers = remote_packet.answers;
        ans_packet.authorities = remote_packet.authorities;
        ans_packet.resources = remote_packet.resources;
    } else {
        ans_packet.header.rescode = MyDnsResultCode::FORMERR;
    }
    println!("ans_packet => {:?}", ans_packet);
    ans_packet.write(&mut ans_buff)?;

    let buf_slice = ans_buff.get_range(0, ans_buff.pos)?;

    socket.send_to(buf_slice, client_addr)?;

    Ok(())
}
// qualify

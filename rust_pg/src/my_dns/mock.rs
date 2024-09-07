const RESPONSE_PACKET_LITERAL: [&str; 59] = [
    "35", "25", "81", "80", "00", "01", "00", "02", "00", "00", "00", "00", "05", "62", "61", "69",
    "64", "75", "03", "63", "6F", "6D", "00", "00", "01", "00", "01", "C0", "0C", "00", "01", "00",
    "01", "00", "00", "00", "2F", "00", "04", "6E", "F2", "44", "42", "C0", "0C", "00", "01", "00",
    "01", "00", "00", "00", "2F", "00", "04", ";27", "9C", "42", "0A",
];

pub fn dns_mock_response_packet() -> [u8; 512] {
    let mut a = [u8::MIN; 512];
    for i in 0..RESPONSE_PACKET_LITERAL.len() {
        let str = RESPONSE_PACKET_LITERAL.get(i).cloned().unwrap_or("00");
        a[i] = u8::from_str_radix(str, 16).unwrap_or(0);
    }
    a
}

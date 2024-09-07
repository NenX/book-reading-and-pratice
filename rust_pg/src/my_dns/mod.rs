mod byte_packet_buffer;
pub mod dns_header;
mod mock;
pub mod packet;
pub mod question;
pub mod record;
pub mod types;

pub use byte_packet_buffer::BytePacketBuffer;
pub use mock::dns_mock_response_packet;

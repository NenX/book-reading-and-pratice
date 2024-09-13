use std::collections::HashMap;

use super::types::{MyDnsBuf, MyDnsResult, MY_DNS_MAX_POS_BOUND};

pub struct BytePacketBuffer {
    pub buf: MyDnsBuf,
    pub pos: usize,
    pub name_record: HashMap<String, usize>,
}
impl From<MyDnsBuf> for BytePacketBuffer {
    fn from(value: MyDnsBuf) -> Self {
        Self {
            pos: 0,
            buf: value,
            name_record: HashMap::new(),
        }
    }
}
impl Default for BytePacketBuffer {
    fn default() -> Self {
        Self::new()
    }
}

impl BytePacketBuffer {
    pub fn new() -> Self {
        Self {
            pos: 0,
            buf: [0; 512],
            name_record: HashMap::new(),
        }
    }

    pub fn pos(&mut self) -> usize {
        self.pos
    }

    pub fn seek(&mut self, p: usize) -> MyDnsResult<()> {
        if p >= MY_DNS_MAX_POS_BOUND {
            return Err("seek".into());
        }
        self.pos = p;
        Ok(())
    }
    pub fn step(&mut self, steps: usize) -> MyDnsResult<()> {
        let target = self.pos + steps;
        if target >= MY_DNS_MAX_POS_BOUND {
            return Err(format!("step {steps}").into());
        }
        self.pos = target;
        Ok(())
    }
    pub fn get(&mut self, p: usize) -> MyDnsResult<u8> {
        if p >= MY_DNS_MAX_POS_BOUND {
            return Err("get".into());
        }
        Ok(self.buf[p])
    }
    pub fn get_range(&mut self, start: usize, len: usize) -> MyDnsResult<&[u8]> {
        let target = start + len;
        if target >= MY_DNS_MAX_POS_BOUND {
            return Err("get_range".into());
        }
        let ret = &self.buf[start..target];
        Ok(ret)
    }
    pub fn read(&mut self) -> MyDnsResult<u8> {
        if self.pos >= MY_DNS_MAX_POS_BOUND {
            return Err("read".into());
        }
        let ret = self.buf[self.pos];
        self.pos += 1;
        Ok(ret)
    }
    pub fn read_u16(&mut self) -> MyDnsResult<u16> {
        let i = (self.read()? as u16) << 8 | (self.read()? as u16);
        Ok(i)
    }
    pub fn read_u32(&mut self) -> MyDnsResult<u32> {
        let i = (self.read()? as u32) << 24
            | (self.read()? as u32) << 16
            | (self.read()? as u32) << 8
            | (self.read()? as u32);
        Ok(i)
    }

    pub fn write(&mut self, val: u8) -> MyDnsResult<()> {
        if self.pos >= 512 {
            return Err("End of buffer".into());
        }
        self.buf[self.pos] = val;
        self.pos += 1;
        Ok(())
    }

    pub fn write_u8(&mut self, val: u8) -> MyDnsResult<()> {
        self.write(val)?;

        Ok(())
    }

    pub fn write_u16(&mut self, val: u16) -> MyDnsResult<()> {
        self.write((val >> 8) as u8)?;
        self.write((val & 0xFF) as u8)?;

        Ok(())
    }

    pub fn write_u32(&mut self, val: u32) -> MyDnsResult<()> {
        self.write(((val >> 24) & 0xFF) as u8)?;
        self.write(((val >> 16) & 0xFF) as u8)?;
        self.write(((val >> 8) & 0xFF) as u8)?;
        self.write(((val >> 0) & 0xFF) as u8)?;

        Ok(())
    }
    pub fn set(&mut self, pos: usize, val: u8) -> MyDnsResult<()> {
        self.buf[pos] = val;

        Ok(())
    }

    pub fn set_u16(&mut self, pos: usize, val: u16) -> MyDnsResult<()> {
        self.set(pos, (val >> 8) as u8)?;
        self.set(pos + 1, (val & 0xFF) as u8)?;

        Ok(())
    }
}
impl BytePacketBuffer {
    pub fn read_qname(&mut self, outstr: &mut String) -> MyDnsResult<()> {
        self.read_qname_plain(outstr, 0)
    }

    fn read_qname_plain(&mut self, outstr: &mut String, n: u8) -> MyDnsResult<()> {
        let mut delimiter = "";

        loop {
            let len = self.get(self.pos)?;
            if len == 0 {
                let _ = self.read()?;

                break;
            }

            if len >> 6 == 0b0000_0011 {
                self.read_qname_jump(outstr, n + 1)?;
                // 跳转回来的时候，说明解析结束了，不能继续
                return Ok(());
            }
            let len = self.read()?;

            outstr.push_str(delimiter);
            delimiter = ".";
            let slice = self.get_range(self.pos, len as usize)?;
            let s = String::from_utf8_lossy(slice);
            outstr.push_str(s.as_ref());
            self.step(len as usize)?;
        }

        Ok(())
    }
    fn read_qname_jump(&mut self, outstr: &mut String, n: u8) -> MyDnsResult<()> {
        if n >= 5 {
            return Err("reach max jump times!!".into());
        }

        let slice = self.read_u16()?;
        let save_pos = self.pos();
        let offset = slice ^ 0xC000;

        self.seek(offset as usize)?;
        self.read_qname_plain(outstr, n + 1)?;
        self.seek(save_pos)?;

        Ok(())
    }
    pub fn read_qname1(&mut self, outstr: &mut String) -> MyDnsResult<()> {
        let mut pos = self.pos();
        let mut jumped = false;

        let mut delim = "";
        let max_jumps = 5;
        let mut jumps_performed = 0;
        loop {
            // Dns Packets are untrusted data, so we need to be paranoid. Someone
            // can craft a packet with a cycle in the jump instructions. This guards
            // against such packets.
            if jumps_performed > max_jumps {
                return Err(format!("Limit of {} jumps exceeded", max_jumps).into());
            }

            let len = self.get(pos)?;

            // A two byte sequence, where the two highest bits of the first byte is
            // set, represents a offset relative to the start of the buffer. We
            // handle this by jumping to the offset, setting a flag to indicate
            // that we shouldn't update the shared buffer position once done.
            if (len & 0xC0) == 0xC0 {
                // When a jump is performed, we only modify the shared buffer
                // position once, and avoid making the change later on.
                if !jumped {
                    self.seek(pos + 2)?;
                }

                let b2 = self.get(pos + 1)? as u16;
                let offset = (((len as u16) ^ 0xC0) << 8) | b2;
                pos = offset as usize;
                jumped = true;
                jumps_performed += 1;
                continue;
            }

            pos += 1;

            // Names are terminated by an empty label of length 0
            if len == 0 {
                break;
            }

            outstr.push_str(delim);

            let str_buffer = self.get_range(pos, len as usize)?;
            outstr.push_str(&String::from_utf8_lossy(str_buffer).to_lowercase());

            delim = ".";

            pos += len as usize;
        }

        if !jumped {
            self.seek(pos)?;
        }

        Ok(())
    }
}

impl BytePacketBuffer {
    pub fn write_qnames(&mut self, qname: &str) -> MyDnsResult<()> {
        let exist = self.name_record.get(qname).cloned();

        if let Some(record) = exist {
            let target_pos = record as u16;

            let write_bytes = target_pos ^ 0xC000;
            return self.write_u16(write_bytes);
        }

        let a = qname.split('.');
        let mut target_pos = usize::MIN;

        for i in a {
            let len = i.len() as u8;
            if len > 0b0011_1111 {
                return Err("".into());
            };
            if target_pos == 0 {
                target_pos = self.pos();
                self.name_record.insert(qname.to_string(), target_pos);
            }

            self.write(len)?;
            let bytes = i.as_bytes();
            for b in bytes {
                self.write(*b)?;
            }
        }
        self.write(0)?;
        Ok(())
    }
}
#[cfg(test)]
#[test]
fn test_write_qname() {
    let mut bb = BytePacketBuffer::new();
    let a: [u8; 11] = [
        0x05, 0x62, 0x61, 0x69, 0x64, 0x75, 0x03, 0x63, 0x6F, 0x6D, 0x00,
    ];
    let mut b = a.clone();
    bb.write_qnames("baidu.com").unwrap();
    b.copy_from_slice(&bb.buf[..11]);

    assert_eq!(a, b)
}

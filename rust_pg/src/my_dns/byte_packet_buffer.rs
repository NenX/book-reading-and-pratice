use super::types::{MyDns_Buf, MyDns_Result, MY_DNS_MAX_POS_BOUND};

pub struct BytePacketBuffer {
    buf: MyDns_Buf,
    pos: usize,
}
impl From<MyDns_Buf> for BytePacketBuffer {
    fn from(value: MyDns_Buf) -> Self {
        Self { pos: 0, buf: value }
    }
}
impl BytePacketBuffer {
    pub fn new() -> Self {
        Self {
            pos: 0,
            buf: [0; 512],
        }
    }

    pub fn pos(&mut self) -> usize {
        self.pos
    }

    pub fn seek(&mut self, p: usize) -> MyDns_Result<()> {
        if p >= MY_DNS_MAX_POS_BOUND {
            return Err("seek".into());
        }
        self.pos = p;
        Ok(())
    }
    pub fn step(&mut self, steps: usize) -> MyDns_Result<()> {
        let target = self.pos + steps;
        if target >= MY_DNS_MAX_POS_BOUND {
            return Err("step".into());
        }
        self.pos = target;
        Ok(())
    }
    pub fn get(&mut self, p: usize) -> MyDns_Result<u8> {
        if p >= MY_DNS_MAX_POS_BOUND {
            return Err("get".into());
        }
        Ok(self.buf[p])
    }
    pub fn get_range(&mut self, start: usize, len: usize) -> MyDns_Result<&[u8]> {
        let target = start + len;
        if target >= MY_DNS_MAX_POS_BOUND {
            return Err("get_range".into());
        }
        let ret = &self.buf[start..target];
        Ok(ret)
    }
    pub fn read(&mut self) -> MyDns_Result<u8> {
        if self.pos >= MY_DNS_MAX_POS_BOUND {
            return Err("read".into());
        }
        let ret = self.buf[self.pos];
        self.pos += 1;
        Ok(ret)
    }
    pub fn read_u16(&mut self) -> MyDns_Result<u16> {
        let i = (self.read()? as u16) << 8 | (self.read()? as u16);
        Ok(i)
    }
    pub fn read_u32(&mut self) -> MyDns_Result<u32> {
        let i = (self.read()? as u32) << 24
            | (self.read()? as u32) << 16
            | (self.read()? as u32) << 8
            | (self.read()? as u32);
        Ok(i)
    }

    pub fn read_qname(&mut self, outstr: &mut String) -> MyDns_Result<()> {
        self.read_qname_impl(outstr, 0)
    }
    fn read_qname_impl(&mut self, outstr: &mut String, n: u8) -> MyDns_Result<()> {
        let b1 = self.get(self.pos)?;

        if b1 >> 6 == 0b0000_0011 {
            self.read_qname_jump(outstr, n)
        } else {
            self.read_qname_plain(outstr)
        }
    }
    fn read_qname_plain(&mut self, outstr: &mut String) -> MyDns_Result<()> {
        let mut delimiter = "";
        loop {
            let len = self.read()? as usize;
            if len == 0 {
                break;
            }
            outstr.push_str(delimiter);
            delimiter = ".";
            let slice = self.get_range(self.pos, len)?;
            let s = String::from_utf8_lossy(slice.into());
            outstr.push_str(&s.to_string());
            self.step(len)?;
        }
        println!("read_qname_plain ==> {}", outstr);

        Ok(())
    }
    fn read_qname_jump(&mut self, outstr: &mut String, n: u8) -> MyDns_Result<()> {
        if n >= 5 {
            return Err("reach max jump times!!".into());
        }
        let slice = self.read_u16()?;
        let save_pos = self.pos();
        let offset = slice ^ 0xC000;

        self.seek(offset as usize)?;
        self.read_qname_impl(outstr, n + 1)?;
        self.seek(save_pos)?;
        println!("read_qname_jump ==> {}", outstr);

        Ok(())
    }
    /// Read a qname
    ///
    /// The tricky part: Reading domain names, taking labels into consideration.
    /// Will take something like [3]www[6]google[3]com[0] and append
    /// www.google.com to outstr.
    pub fn read_qname1(&mut self, outstr: &mut String) -> MyDns_Result<()> {
        // Since we might encounter jumps, we'll keep track of our position
        // locally as opposed to using the position within the struct. This
        // allows us to move the shared position to a point past our current
        // qname, while keeping track of our progress on the current qname
        // using this variable.
        let mut pos = self.pos();

        // track whether or not we've jumped
        let mut jumped = false;
        let max_jumps = 5;
        let mut jumps_performed = 0;

        // Our delimiter which we append for each label. Since we don't want a
        // dot at the beginning of the domain name we'll leave it empty for now
        // and set it to "." at the end of the first iteration.
        let mut delim = "";
        loop {
            // Dns Packets are untrusted data, so we need to be paranoid. Someone
            // can craft a packet with a cycle in the jump instructions. This guards
            // against such packets.
            if jumps_performed > max_jumps {
                return Err(format!("Limit of {} jumps exceeded", max_jumps).into());
            }

            // At this point, we're always at the beginning of a label. Recall
            // that labels start with a length byte.
            let len = self.get(pos)?;

            // If len has the two most significant bit are set, it represents a
            // jump to some other offset in the packet:
            if (len & 0xC0) == 0xC0 {
                // Update the buffer position to a point past the current
                // label. We don't need to touch it any further.
                if !jumped {
                    self.seek(pos + 2)?;
                }

                // Read another byte, calculate offset and perform the jump by
                // updating our local position variable
                let b2 = self.get(pos + 1)? as u16;
                let offset = (((len as u16) ^ 0xC0) << 8) | b2;
                pos = offset as usize;

                // Indicate that a jump was performed.
                jumped = true;
                jumps_performed += 1;

                continue;
            }
            // The base scenario, where we're reading a single label and
            // appending it to the output:
            else {
                // Move a single byte forward to move past the length byte.
                pos += 1;

                // Domain names are terminated by an empty label of length 0,
                // so if the length is zero we're done.
                if len == 0 {
                    break;
                }

                // Append the delimiter to our output buffer first.
                outstr.push_str(delim);

                // Extract the actual ASCII bytes for this label and append them
                // to the output buffer.
                let str_buffer = self.get_range(pos, len as usize)?;
                outstr.push_str(&String::from_utf8_lossy(str_buffer).to_lowercase());

                delim = ".";

                // Move forward the full length of the label.
                pos += len as usize;
            }
        }

        if !jumped {
            self.seek(pos)?;
        }

        Ok(())
    }
}

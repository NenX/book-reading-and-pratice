import { createServer, Socket } from 'net'
import { createHash } from 'crypto'
import { WsMessage } from './WsMessage'
const GUID = '258EAFA5-E914-47DA-95CA-C5AB0DC85B11'

class WsMessage2<T extends string | Buffer> {
    data: T
    constructor(d: T) {
        this.data = d
    }
    as_buffer() {
        if (typeof this.data === 'string') {
            return Buffer.from(this.data)
        }
        return this.data
    }
    toString() {

        return this.data.toString()
    }
    opcode() {
        return typeof this.data === 'string' ? 1 : 2
    }
    decode() {

        let buffer = [0b10000000 | this.opcode()];
        let payload = this.as_buffer();
        let payload_len = payload.length;
        console.log('payload_len 0', payload_len)
        if (payload_len > 125) {

            if (payload_len > 2 ** 16 - 1) {
                let b = Buffer.alloc(8)
                b.writeUintBE(payload_len, 0, 8)
                buffer.push(
                    ...b
                )
            } else {
                let b = Buffer.alloc(2)
                b.writeUintBE(payload_len, 0, 2)
                buffer.push(
                    ...b
                )
            }
        } else {
            buffer.push(payload_len)
        }

        const a = Array.from([...payload]) as number[];
        buffer.push(...a)

        return Buffer.from(buffer)

    }

    static encode(data: Buffer) {
        const opcode = data[0] & 0b00001111;
        const mask = data[1] >> 7;
        let payload_len = data[1] & 0b01111111;
        let payload_len_bits = 1;
        if (mask !== 1) {
            console.log('bad mask', mask)
            return
        }
        if (payload_len === 126) {
            payload_len_bits += 2;
            payload_len = data.readUintBE(2, 2);

        } else if (payload_len === 127) {
            payload_len_bits += 8;
            payload_len = data.readUintBE(2, 8);

        }
        let mask_key = data.subarray(payload_len_bits + 1, payload_len_bits + 5);

        let payload_data = data.subarray(payload_len_bits + 5, payload_len_bits + 5 + payload_len);

        console.log({ mask_key, payload_data, data })

        let uint8_arr = payload_data.map((v, i) => {
            return v ^ mask_key[i % 4]
        })
        if (opcode === 1) {
            return new WsMessage(uint8_arr.toString())

        }
        else {
            return new WsMessage(Buffer.from(uint8_arr))

        }
    }

}

function serve() {
    createServer(s => {
        let is_upgrade: boolean | undefined
        s.on('ready', () => {
        })
        s.on('data', b => {
            if (!is_upgrade) {
                is_upgrade = handshake(s, b)
                return
            }
            const m = WsMessage.encode(b)
            console.log("receive message", m?.toString())
            const a = new WsMessage("aa")
            s.write(a.decode())

            const a2 = new WsMessage(Buffer.from([1, 2, 3]))
            const aa = [...a2.decode()]
            aa.pop()
            s.write(Buffer.from(aa))
            setTimeout(() => {
                s.write(Buffer.from([4]))
            }, 2000)
        })
    }).listen(6600)
}

function handshake(s: Socket, b: Buffer) {

    const data = b.toString();
    console.log('read', data)
    const lines = data.split('\r\n').filter(_ => _)
    const request_line = lines[0]
    if (!request_line?.toLowerCase()?.startsWith('get')) {
        return
    }
    const headers = lines.slice(1).reduce((a, b) => {
        const kv = b.split(':')

        return { ...a, [kv[0].trim()]: kv[1]?.trim() }
    }, {})

    if (headers['Connection'] !== 'Upgrade' || headers['Upgrade'] !== 'websocket') {
        return
    }
    const key = headers['Sec-WebSocket-Key']
    const sha1 = createHash('sha1')

    const accept = sha1.update(key + GUID)
        .digest('base64');

    let response: string[] = []
    response.push('HTTP/1.1 101 Switching Protocols\r\n')
    response.push('Upgrade: websocket\r\n')
    response.push('Connection: Upgrade\r\n')
    response.push(`Sec-WebSocket-Accept: ${accept}\r\n\r\n`);



    const response_txt = response.join('')
    console.log(JSON.stringify(response_txt))
    s.write(response_txt)
    return true
}


function main() {

    serve()
}

main()
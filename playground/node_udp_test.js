const argLength = process.argv.length
const dgram = require('dgram')

if (argLength > 2) {
    // server
    const server = dgram.createSocket('udp4')
    server.on('connect', () => {
        console.log('dgram connect!')
    })
    server.on('message', (buf, info) => {
        console.log('dgram message:', buf.toString(), info)
    })
    server.on('listening', () => {
        const addr = server.address()
        console.log('dgram listening:', addr.address, ':', addr.port)
    })
    server.bind('41234')
} else {
    //client

    process.stdin.on('data', e => {
        const client = dgram.createSocket('udp4')


        client.send(e.toString(), 41234, '172.20.243.121', e => {
            console.log('send error', e)
            client.close()
        })
    })
}
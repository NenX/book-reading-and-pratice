const net = require('net')

net.createServer({ allowHalfOpen: false }, sock => {
    console.log('ooookkk')
    sock.on('data', a => {
        console.log('接受', a.toString())
        sock.writable && sock.write('hi I am server:' + a.toString())
    })
    sock.on('close', () => {
        console.log('对方关闭')
    })
    sock.on('error', (e) => {
        console.log('错误', e.message)
    })
    sock.on('end', () => {
        console.log('end')
    })
    process.stdin.on('data', d => {
        if (!sock.closed) {

            // sock.write(d.toString())
            sock.end()
        } else {
            console.log('已经关闭')
        }
    })
}).listen(9877)


import { createServer, connect } from "net";

const connect_to_server = () => connect({ host: '127.0.0.1', port: 2200 });
let s = connect_to_server()

s.on('data', e => {
    let str = e.toString()

    console.log('receive json', JSON.stringify(str))

})
s.on('error', e => {
    console.log('err', e)
})
process.stdin.on('data', d => {
    let data = d.toString();
    // let data = "*2\r\n$3\r\nget\r\n$1\r\na\r\n";
    console.log('input --->', JSON.stringify(data))

    s.write(data)

})


import { WebSocket } from 'ws'
const s = new WebSocket("ws://127.0.0.1:6600")

s.addEventListener('open', e => {
    console.log('open', e.type)

})
s.addEventListener('error', e => {
    console.log('error', e.message)
})
s.addEventListener('message', e => {
    console.log(`message(${e.data.length}):`, e.data)
})
s.addEventListener('close', e => {
    console.log('close', e.reason)
})

process.stdin.on('data', e => {
    const d = e.toString().trim().repeat(10);
    console.log({ d: JSON.stringify(d), dd: Buffer.from(d) })
    s.send(d)
})
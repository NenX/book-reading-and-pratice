import { WebSocket, WebSocketServer } from 'ws'
const server = new WebSocketServer({ port: 6600 })



server.on('connection', con => {
    con.on('message', b => {
        const msg  = b?.toString()
        console.log('test server', msg)
        con.send(`echo ${msg}`)
    })

})


process.stdin.on('data', e => {
    const d = e.toString().trim();
    console.log(JSON.stringify(d), server.send)
})
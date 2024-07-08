
// 分块传输编码
import http from 'http';

http.createServer((req, res) => {
    console.log('req', req.url, req.headers)

    res.writeHead(200, 'OK', {
        'Content-Type': 'text/html',
        // 'Transfer-Encoding': 'chunked' // 默认带这个响应头部开启分块传输
    })

    let n = 0;
    res.write(`start write at ${new Date().toLocaleString()} \n`)
    const id = setInterval(() => {

        n++;
        res.write(String.fromCharCode(64 + n).repeat(20))

        if (n >= 5) {
            clearInterval(id)
            res.end()
        }
    }, 1000);


}).listen(2345)
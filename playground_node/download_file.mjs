
// 下载文件

import http from 'http';

http.createServer((req, res) => {
    console.log('req', req.url, req.headers)

    res.writeHead(200, 'OK', {
        // 'Content-Type': 'text/html',
        'Content-Type': 'application/download',
        'Content-Disposition': 'filename=a.txt',
    })



    let n = 0;
    res.write(`start write at ${new Date().toLocaleString()} \n`)
    const id = setInterval(() => {

        n++;
        res.write(String.fromCharCode(64 + n).repeat(10))

        if (n >= 5) {
            clearInterval(id)
            res.end()
        }
    }, 2000);


}).listen(2345)
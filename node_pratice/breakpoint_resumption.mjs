
// 断点续传 
//???: 未完成

import http from 'http';

http.createServer((req, res) => {
    // res.setHeader('Content-Type', 'application/download')
    // res.setHeader('Content-Disposition', 'attachment,filename=a.txt')
    console.log('req', req.url, req.headers)

    res.writeHead(206, 'Partial Content', {
        // 'Content-Type': 'text/html',
        'Content-Type': 'application/octet-stream',
        'Etag':'6683aefb-3348631',
        'Content-Disposition': 'filename=a.txt',
        'Accept-Ranges':'bytes',
        'Content-length': 51,
        // 'Content-Range': 'bytes 0-49/22400'
    })

    // res.write('0'.repeat(50))
    // res.end()


    let n = 0;
    // res.write(`start write at ${new Date().toLocaleString()} \n`)
    const id = setInterval(() => {

        n++;
        res.write(String.fromCharCode(64 + n).repeat(10))

        if (n >= 5) {
            clearInterval(id)
            res.write('\n')
            res.end()
        }
    }, 2000);


}).listen(2345)
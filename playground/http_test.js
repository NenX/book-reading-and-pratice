const { createServer } = require('http')


createServer((req, res) => {
    res.setHeader('aaa', 'bbb')

    console.log('请求', req.url)
    res.write(req.url);
    res.flushHeaders()
    res.write('ssssbbbb\n');

    setTimeout(() => {
        res.end();

    }, 5000);
}).listen(2288)
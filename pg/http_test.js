const { createServer } = require('http')


createServer((req, res) => {
    res.setHeader('aaa', 'bbb')

    console.log('请求', req.url)
    res.write(req.url);
    res.flushHeaders()
    res.write('ssssbbbb\n');

    res.end();

}).listen(2288)
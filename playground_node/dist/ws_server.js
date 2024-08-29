function _class_call_check(instance, Constructor) {
    if (!(instance instanceof Constructor)) {
        throw new TypeError("Cannot call a class as a function");
    }
}
function _defineProperties(target, props) {
    for(var i = 0; i < props.length; i++){
        var descriptor = props[i];
        descriptor.enumerable = descriptor.enumerable || false;
        descriptor.configurable = true;
        if ("value" in descriptor) descriptor.writable = true;
        Object.defineProperty(target, descriptor.key, descriptor);
    }
}
function _create_class(Constructor, protoProps, staticProps) {
    if (protoProps) _defineProperties(Constructor.prototype, protoProps);
    if (staticProps) _defineProperties(Constructor, staticProps);
    return Constructor;
}
function _define_property(obj, key, value) {
    if (key in obj) {
        Object.defineProperty(obj, key, {
            value: value,
            enumerable: true,
            configurable: true,
            writable: true
        });
    } else {
        obj[key] = value;
    }
    return obj;
}
function _object_spread(target) {
    for(var i = 1; i < arguments.length; i++){
        var source = arguments[i] != null ? arguments[i] : {};
        var ownKeys = Object.keys(source);
        if (typeof Object.getOwnPropertySymbols === "function") {
            ownKeys = ownKeys.concat(Object.getOwnPropertySymbols(source).filter(function(sym) {
                return Object.getOwnPropertyDescriptor(source, sym).enumerable;
            }));
        }
        ownKeys.forEach(function(key) {
            _define_property(target, key, source[key]);
        });
    }
    return target;
}
function ownKeys(object, enumerableOnly) {
    var keys = Object.keys(object);
    if (Object.getOwnPropertySymbols) {
        var symbols = Object.getOwnPropertySymbols(object);
        if (enumerableOnly) {
            symbols = symbols.filter(function(sym) {
                return Object.getOwnPropertyDescriptor(object, sym).enumerable;
            });
        }
        keys.push.apply(keys, symbols);
    }
    return keys;
}
function _object_spread_props(target, source) {
    source = source != null ? source : {};
    if (Object.getOwnPropertyDescriptors) {
        Object.defineProperties(target, Object.getOwnPropertyDescriptors(source));
    } else {
        ownKeys(Object(source)).forEach(function(key) {
            Object.defineProperty(target, key, Object.getOwnPropertyDescriptor(source, key));
        });
    }
    return target;
}
import { createServer } from 'net';
import { createHash } from 'crypto';
var sha1 = createHash('sha1');
var GUID = '258EAFA5-E914-47DA-95CA-C5AB0DC85B11';
var WsMessage = /*#__PURE__*/ function() {
    "use strict";
    function WsMessage() {
        _class_call_check(this, WsMessage);
        _define_property(this, "data", void 0);
    }
    _create_class(WsMessage, [
        {
            key: "as_buffer",
            value: function as_buffer() {
                if (typeof this.data === 'string') {
                    return Buffer.from(this.data);
                }
                return this.data;
            }
        },
        {
            key: "opcode",
            value: function opcode() {
                return typeof this.data === 'string' ? 1 : 2;
            }
        },
        {
            key: "decode",
            value: function decode() {
                var b = [];
            }
        }
    ]);
    return WsMessage;
}();
function serve() {
    createServer(function(s) {
        var is_upgrade;
        s.on('ready', function() {});
        s.on('data', function(b) {
            if (!is_upgrade) {
                is_upgrade = handshake(s, b);
                return;
            }
            var b1 = b[0];
            var b2 = b[0];
            var opcode = b1 & 15;
        });
    }).listen(6600);
}
function handshake(s, b) {
    var _request_line_toLowerCase;
    var data = b.toString();
    console.log('read', data);
    var lines = data.split('\r\n').filter(function(_) {
        return _;
    });
    var request_line = lines[0];
    if (!(request_line === null || request_line === void 0 ? void 0 : (_request_line_toLowerCase = request_line.toLowerCase()) === null || _request_line_toLowerCase === void 0 ? void 0 : _request_line_toLowerCase.startsWith('get'))) {
        return;
    }
    var headers = lines.slice(1).reduce(function(a, b) {
        var _kv_;
        var kv = b.split(':');
        return _object_spread_props(_object_spread({}, a), _define_property({}, kv[0].trim(), (_kv_ = kv[1]) === null || _kv_ === void 0 ? void 0 : _kv_.trim()));
    }, {});
    if (headers['Connection'] !== 'Upgrade' || headers['Upgrade'] !== 'websocket') {
        return;
    }
    var key = headers['Sec-WebSocket-Key'];
    var accept = sha1.update(key + GUID).digest('base64');
    var response = [];
    response.push('HTTP/1.1 101 Switching Protocols\r\n');
    response.push('Upgrade: websocket\r\n');
    response.push('Connection: Upgrade\r\n');
    response.push("Sec-WebSocket-Accept: ".concat(accept, "\r\n\r\n"));
    var response_txt = response.join('');
    console.log(JSON.stringify(response_txt));
    s.write(response_txt);
    return true;
}
function main() {
    var a = [
        1,
        255,
        9999
    ];
    var b = Buffer.from(a);
    console.log(b);
}
main();


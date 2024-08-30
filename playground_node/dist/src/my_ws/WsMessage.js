function _array_like_to_array(arr, len) {
    if (len == null || len > arr.length) len = arr.length;
    for(var i = 0, arr2 = new Array(len); i < len; i++)arr2[i] = arr[i];
    return arr2;
}
function _array_without_holes(arr) {
    if (Array.isArray(arr)) return _array_like_to_array(arr);
}
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
function _iterable_to_array(iter) {
    if (typeof Symbol !== "undefined" && iter[Symbol.iterator] != null || iter["@@iterator"] != null) return Array.from(iter);
}
function _non_iterable_spread() {
    throw new TypeError("Invalid attempt to spread non-iterable instance.\\nIn order to be iterable, non-array objects must have a [Symbol.iterator]() method.");
}
function _to_consumable_array(arr) {
    return _array_without_holes(arr) || _iterable_to_array(arr) || _unsupported_iterable_to_array(arr) || _non_iterable_spread();
}
function _unsupported_iterable_to_array(o, minLen) {
    if (!o) return;
    if (typeof o === "string") return _array_like_to_array(o, minLen);
    var n = Object.prototype.toString.call(o).slice(8, -1);
    if (n === "Object" && o.constructor) n = o.constructor.name;
    if (n === "Map" || n === "Set") return Array.from(n);
    if (n === "Arguments" || /^(?:Ui|I)nt(?:8|16|32)(?:Clamped)?Array$/.test(n)) return _array_like_to_array(o, minLen);
}
var GUID = '258EAFA5-E914-47DA-95CA-C5AB0DC85B11';
export var WsMessage = /*#__PURE__*/ function() {
    "use strict";
    function WsMessage(d) {
        _class_call_check(this, WsMessage);
        _define_property(this, "data", void 0);
        this.data = d;
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
            key: "toString",
            value: function toString() {
                return this.data.toString();
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
                var _buffer;
                var buffer = [
                    128 | this.opcode()
                ];
                var payload = this.as_buffer();
                var payload_len = payload.length;
                console.log('payload_len 0', payload_len);
                if (payload_len > 125) {
                    if (payload_len > Math.pow(2, 16) - 1) {
                        var _buffer1;
                        var b = Buffer.alloc(8);
                        b.writeUintBE(payload_len, 0, 8);
                        (_buffer1 = buffer).push.apply(_buffer1, _to_consumable_array(b));
                    } else {
                        var _buffer2;
                        var b1 = Buffer.alloc(2);
                        b1.writeUintBE(payload_len, 0, 2);
                        (_buffer2 = buffer).push.apply(_buffer2, _to_consumable_array(b1));
                    }
                } else {
                    buffer.push(payload_len);
                }
                var a = Array.from(_to_consumable_array(payload));
                (_buffer = buffer).push.apply(_buffer, _to_consumable_array(a));
                return Buffer.from(buffer);
            }
        }
    ], [
        {
            key: "encode",
            value: function encode(data) {
                var opcode = data[0] & 15;
                var mask = data[1] >> 7;
                var payload_len = data[1] & 127;
                var payload_len_bits = 1;
                if (mask !== 1) {
                    console.log('bad mask', mask);
                    return;
                }
                if (payload_len === 126) {
                    payload_len_bits += 2;
                    payload_len = data.readUintBE(2, 2);
                } else if (payload_len === 127) {
                    payload_len_bits += 8;
                    payload_len = data.readUintBE(2, 8);
                }
                var mask_key = data.subarray(payload_len_bits + 1, payload_len_bits + 5);
                var payload_data = data.subarray(payload_len_bits + 5, payload_len_bits + 5 + payload_len);
                console.log({
                    mask_key: mask_key,
                    payload_data: payload_data,
                    data: data
                });
                var uint8_arr = payload_data.map(function(v, i) {
                    return v ^ mask_key[i % 4];
                });
                if (opcode === 1) {
                    return new WsMessage(uint8_arr.toString());
                } else {
                    return new WsMessage(Buffer.from(uint8_arr));
                }
            }
        }
    ]);
    return WsMessage;
}();

import struct
from collections import namedtuple
import time

PROTO_MAGIC = b"\xFE\x00\x00\x00"
PACKET_LENGTH = 10
TIMEOUT = 1

# fields in the 'flags' byte, for internal use only
FLAG_ERR = (1 << 0)
FLAG_RESP = (1 << 1)
FLAG_RW = (1 << 2)


# error codes
ERR_UNKNOWN = 0
ERR_INVAL_REG = 1
ERR_INVAL_OP = 2

_error_names = {
        ERR_UNKNOWN: "Unknown error",
        ERR_INVAL_REG: "Invalid register",
        ERR_INVAL_OP: "Invalid operation"
    }

class InvalidMagic(Exception):
    pass

class InvalidPacketLength(Exception):
    pass

class InvalidData(Exception):
    pass

class ResponseTimeout(Exception):
    pass

class ResponseError(Exception):
    def __init__(self, code):
        super().__init__()
        self.code = code

    def __str__(self):
        return "Error code {}: {}".format(self.code, _error_names.get(self.code, "Unrecognised error code"))

class ResponseMalformed(Exception):
    pass

class RegisterNotWritable(Exception):
    def __init__(self, register):
        super().__init__()
        self.register = register

    def __str__(self):
        return "Register {} is not writable".format(register.name)

class Packet(namedtuple("PacketTuple", ["src", "dst", "flags", "reg", "data"])):
    _struct_fmt = "4sBBBBH"
    _struct = struct.Struct(_struct_fmt)

    def __init__(self, src: int, dst: int, flags: int, reg: int, data: int):
        pass

    def encode(self):
        """Encodes the packet into a bytes object to be sent over the serial port"""
        return self._struct.pack(PROTO_MAGIC, *self)

    @classmethod
    def decode(cls, raw_bytes: bytes):
        """Returns a new Packet object from the data in `raw_bytes`. Will raise
        InvalidPacketLength if `raw_bytes` is of the wrong length, or InvalidMagic
        if the magic number is wrong"""
        if len(raw_bytes) != PACKET_LENGTH:
            raise InvalidPacketLength("Packet length should be {}, was {}".format(
                PACKET_LENGTH, len(raw_bytes))
            )
        if not raw_bytes.startswith(PROTO_MAGIC):
            raise InvalidMagic("Packet's magic number was {}, exptected {}".format(
                raw_bytes[:4], PROTO_MAGIC)
            )
        unpacked = cls._struct.unpack(raw_bytes)
        packet_data = unpacked[1:] # throw away magic
        return cls(*packet_data)

class Register:
    def __init__(self, name, writable, reg_addresses):
        self.name = name
        self.is_writable = writable

        if not (1 <= len(reg_addresses) <= 2):
            msg = "A value can be contained in only one or two registers, "
            msg += "{} were given".format(len(reg_addresses))
            raise ValueError(msg)

        self.addresses = reg_addresses

    def is_multipart(self):
        return len(self.addresses) == 2

    def is_writable(self):
        return self.writable

    def check_in_range(self, value):
        if self.is_multipart():
            return value & 0xffff == value
        else:
            return value & 0xffffffff == value

class Connection:
    def __init__(self, address, unit_address, serial_port, timeout = TIMEOUT):
        self.own_addr = address
        self.unit_addr = unit_address
        self.port = serial_port
        self.timeout = timeout

    def read(self, register):
        if register.is_multipart():
            resp_low = self.send(register.addresses[0])
            resp_high = self.send(register.addresses[1])
            print("multipart read", resp_low, resp_high)
            return resp_high << 16 | resp_low

        else:
            resp = self.send(register.addresses[0])
            print("single read", resp)
        

    def write(self, register, value):
        if not register.is_writable():
            raise RegisterNotWritable()

        if not register.check_in_range(value):
            msg = "Cannot write {} to register of length {} bytes".format(value, len(register.addresses))
            raise ValueError(msg)

        if register.is_multipart():
            resp_low = self.send(register.addresses[0], True, value & 0xfffff)
            resp_high = self.send(register.addresses[1], True, (value >> 16) & 0xfffff)
            print("multipart write", resp_low, resp_high)

        else:
            resp = self.send(register.addresses[0], True, value)
            print("single write", resp)

    def send(self, address, write = False, data = 0):
        flags = FLAG_RW if write else 0
        req = Packet(self.own_addr, self.unit_addr, flags, address, data)
        raw = req.encode()

        awaiting_resp = True
        start = time.time()
        self.port.write(raw)

        resp = None
        while awaiting_resp and (time.time() - start < self.timeout):
            raw_resp = self.port.read(10)

            # empty response because, serial port can also timeout
            if not len(raw_resp):
                continue

            try:
                resp = Packet.decode(raw_resp)
            except InvalidMagic:
                continue

            # check if message matches our request, if no match continue
            if resp.src != req.dst or resp.dst != req.src or resp.reg != req.reg:
                continue
            else:
                awaiting_resp = False

        if awaiting_resp:
            raise ResponseTimeout()

        if resp.flags & FLAG_ERR:
            raise ResponseError(resp.data)
        elif not (resp.flags & FLAG_RESP):
            raise ResponseMalformed()

        return resp.data

# registers
REG_PING = Register("PING", False, [0])
REG_WATER_LEVEL = Register("WATER_LEVEL", False, [1])
REG_MOISTURE = Register("MOISTURE", False, [2])
REG_MOISTURE_WANTED = Register("WANTED_MOISTURE", True, [3])
REG_WATER_INTERVAL = Register("WATER_INTERVAL", True, [4, 5])
REG_LOG_INTERVAL = Register("LOG_INTERVAL", True, [6, 7])

# make sure the Packet class encodes the packets to 'bytes' object
# of the correct length (10)
if struct.calcsize(Packet._struct_fmt) != PACKET_LENGTH:
    msg = "class 'Packet' should pack into arrays of length {}, was {}".format(
        PACKET_LENGTH, struct.calcsize(Packet._struct_fmt))
    raise InvalidPacketLength(msg)

if __name__ == "__main__":
    import serial
    port = serial.Serial("/dev/ttyUSB1", 9600, timeout = 1)
    conn = Connection(0x01, 0xa0, port)

    print(conn.read(REG_LOG_INTERVAL))

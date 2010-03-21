
class BitOutputStream(object):
    def __init__(self, stream):
        self.stream = stream
        self.mask = 128
        self.byte = 0
    def write(self, bit):
        if bit: self.byte |= self.mask
        self.mask >>= 1
        if not self.mask:
            self.stream.write(chr(self.byte))
            self.byte = 0
            self.mask = 128
    def close(self):
        self.stream.write(chr(self.byte))
        # self.stream.close()

class Encoder(object):
    def __init__(self, ostream):
        self.bitstream = BitOutputStream(ostream)
        self.low = 0
        self.high = 0xffff
        self.underflow_bits = 0
    def encode(self, low_count, high_count, scale):
        high, low = self.high, self.low
        underflow_bits = self.underflow_bits
        write = self.bitstream.write
        
        range = (high - low) + 1
        high = low + (range*high_count)/scale - 1
        low = low + (range*low_count)/scale
        while True:
            if high & 0x8000 == low & 0x8000:
                write(high & 0x8000)
                while underflow_bits > 0:
                    write(~high & 0x8000)
                    underflow_bits -= 1
            elif low & 0x4000 and not high & 0x4000:
                underflow_bits += 1
                low &= 0x3fff
                high |= 0x4000
            else:
                self.high = high
                self.low = low
                self.underflow_bits = underflow_bits
                return
            low = (low << 1) & 0xffff
            high = ((high << 1) & 0xffff) | 1
    def close(self):
        write = self.bitstream.write
        write(self.low & 0x4000)
        self.underflow_bits += 1
        while self.underflow_bits > 0:
            write(~self.low & 0x4000)
            self.underflow_bits -= 1
        for i in range(16): write(False)
        self.bitstream.close()

class BitInputStream(object):
    def __init__(self, stream):
        self.stream = stream
        self.byte = self.mask = 0
    def read(self):
        if not self.mask:
            self.byte = self.stream.read(1)
            self.mask = 128
            if not self.byte: assert False, 'Unexpected EOF'
        result = bool(ord(self.byte) & self.mask)
        self.mask >>= 1
        return result

class Decoder(object):
    def __init__(self, istream):
        self.bitstream = BitInputStream(istream)
        self.code = 0
        for i in range(16):
            self.code <<= 1
            self.code += self.bitstream.read()
        self.low = 0
        self.high = 0xffff
    def decode(self, scale):
        high, low, code = self.high, self.low, self.code
        range = (high - low) + 1
        count = (((code - low) + 1)*scale - 1) / range
        return count
    def remove(self, low_count, high_count, scale):
        high, low, code = self.high, self.low, self.code
        range = (high - low) + 1
        high = low + (range*high_count)/scale - 1
        low = low + (range*low_count)/scale
        while True:
            if high & 0x8000 == low & 0x8000:
                pass
            elif low & 0x4000 == 0x4000 and high & 0x4000 == 0:
                code ^= 0x4000
                low &= 0x3fff
                high |= 0x4000
            else:
                self.high = high
                self.low = low
                self.code = code
                return
            low = (low << 1) & 0xffff
            high = ((high << 1) & 0xffff) | 1
            code = (code << 1) & 0xffff
            code += self.bitstream.read()

if __name__ == '__main__':
    import cStringIO, binascii

    def chr2bin(c):
        i = ord(c)
        mask = 128
        result = []
        while mask:
            if i & mask: result.append('1')
            else: result.append('0')
            mask >>= 1
        return ''.join(result)

    def test():
        table = dict(a=(0, 2), b=(2, 5), c=(5, 10))
        reverse_table = 'aabbbccccc'
        scale = 10
        input = 'ababcababc' * 10000
        stream = cStringIO.StringIO()
        encoder = Encoder(stream)
        for char in input:
            low_count, high_count = table[char]
            encoder.encode(low_count, high_count, scale)
        encoder.close()
        s = stream.getvalue()
        stream.seek(0)
        decoder = Decoder(stream)
        for i, char in enumerate(input):
            count = decoder.decode(scale)
            char2 = reverse_table[count]
            if char != char2: print 'ERROR', i, char, char2
            low_count, high_count = table[char]
            decoder.remove(low_count, high_count, scale)
        print 'PASSED'
    test()



































